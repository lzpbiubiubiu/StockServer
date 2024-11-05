#include <QFileInfo>
#include <QJsonDocument>

#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Util/MathUtil.h"
#include "UploadTask.h"

// 任务超时轮询时间间隔(ms)
const int TIMEOUT_CHECK_INTERVAL = 100;

namespace Http
{
    UploadTask::UploadTask(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent/* = nullptr*/)
        : Task(request, response, parent)
    {
    }

    UploadTask::~UploadTask()
    {
        if(m_file)
        {
            m_file->close();
            m_file->deleteLater();
            m_file = nullptr;
        }
    }

    void UploadTask::Run()
    {
        Q_ASSERT(m_request);
        Q_ASSERT(m_response);
        Q_ASSERT(m_networkAccessManager);

        // 请求重试复用同一个HttpResponse，所以每次请求前需要清空数据
        m_response->ClearData();
        m_response->SetError("");
        m_response->SetErrorMessage("");
        m_response->SetStatusCode(0);
        m_response->SetDownloadSize(0);

        QNetworkRequest request;
        request.setUrl(m_request->GetUrl());

        // SSL设置
        if(m_request->IsSslEnabled())
        {
            QSslConfiguration ssl;
            ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
            ssl.setProtocol(QSsl::TlsV1SslV3);
            request.setSslConfiguration(ssl);
        }

        m_file = new QFile(m_request->GetUploadFilePath());
        if(!m_file->open(QIODevice::ReadOnly))
        {
            QString message = QString("open file %1 failed").arg(m_request->GetUploadFilePath());
            LOG_ERROR(message.toStdString());
            m_response->SetError(ERR_NETWORK_CREATE_FILE_FAILED);
            m_response->SetErrorMessage(message);
            Finished();
            return;
        }

        QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart filePart;
        QString header = QString("form-data; name=\"file\"; filename=\"%1\"").arg(QFileInfo(m_file->fileName()).fileName());

        filePart.setBodyDevice(m_file);
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header));
        multiPart->append(filePart);

        SetRawHeader(request);
        m_networkReply = m_networkAccessManager->post(request, multiPart);
        multiPart->setParent(m_networkReply);
        if(m_request->IsLogEnabled())
            PrintRequestLog();

        m_timestamp = QDateTime::currentMSecsSinceEpoch();

        connect(m_networkReply, &QNetworkReply::readyRead, this, &UploadTask::onReplyReadReady);
        connect(m_networkReply, &QNetworkReply::finished, this, &UploadTask::onReplyFinished);
        connect(m_networkReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &UploadTask::onReplyError);
        connect(m_networkReply, &QNetworkReply::sslErrors, this, &UploadTask::onReplySslErrors);
        connect(m_networkReply, &QNetworkReply::uploadProgress, this, &UploadTask::onReplyUploadProgress);

        connect(m_timer, &QTimer::timeout, this, &UploadTask::onTimeout, Qt::UniqueConnection);
        m_timer->start(TIMEOUT_CHECK_INTERVAL);
    }

    void UploadTask::PrintRequestLog()
    {
        QByteArray data = QByteArray::fromPercentEncoding(m_request->GetData());
        QString header = QJsonDocument(m_request->GetHeaderJson()).toJson(QJsonDocument::Compact);
        QString url = m_request->GetUrl();

        LOG_INFO("HTTP-CLIENT-UPLOAD [{}] Request: {} Header: {} File: {} Size: {} KB",
                    m_uuid.toStdString(),
                    m_request->GetUrl().toStdString(),
                    header.toStdString(),
                    m_request->GetUploadFilePath().toStdString(),
                    QString::number(m_file->size() / 1024.0, 'f', 2).toStdString());
    }

    void UploadTask::PrintResponseLog()
    {
        QByteArray data = m_response->GetData();
        qint64 elapsed = m_response->GetElapsed();
        int code = m_response->GetStatusCode();

        if(m_response->GetLogFormat() == LogFormat::DETAIL)
        {
            LOG_INFO("HTTP-CLIENT-UPLOAD [{}] Response: Time:{} ms Status:{} Size:{} KB Content: {}",
                m_uuid.toStdString(),
                elapsed,
                code,
                QString::number(data.size() / 1024.0, 'f', 2).toStdString(),
                data.toStdString());
        }
        else
        {
            LOG_INFO("HTTP-CLIENT-UPLOAD [{}] Response: Time:{} ms Status:{} Size:{} KB",
                m_uuid.toStdString(),
                elapsed,
                code,
                QString::number(m_response->GetData().size() / 1024.0, 'f', 2).toStdString());
        }
    }

    void UploadTask::Finished()
    {
        if(m_file)
        {
            m_file->close();
            m_file->deleteLater();
            m_file = nullptr;
        }

        if(m_networkReply)
        {
            m_networkReply->disconnect();
            m_networkReply->deleteLater();
            m_networkReply = nullptr;
        }

        auto result = QSharedPointer<Http::Result>::create();
        result->trace = m_uuid;
        result->url = m_request->GetUrl();
        result->method = "POST";
        result->ok = m_response->GetError() == ERR_NETWORK_SUCCESS;
        result->status = m_response->GetStatusCode();
        result->error = m_response->GetError();;
        result->message = m_response->GetErrorMessage();
        result->elapsed = m_elapsed;
        result->sent = m_request->GetData().size();
        result->recv = m_response->GetData().size();

        if(m_request->GetCompleteCallback() != nullptr)
            m_request->GetCompleteCallback()(m_request, m_response);

        emit signalFinished(result);
    }

    void UploadTask::onReplyReadReady()
    {
        m_response->AppendData(m_networkReply->readAll());
    }

    void UploadTask::onReplyFinished()
    {
        // 用户abort()
        QNetworkReply::NetworkError code = m_networkReply->error();
        if(code != QNetworkReply::NetworkError::NoError)
        {
            onReplyError(code);
            return;
        }

        m_timer->disconnect();
        m_timer->stop();
        m_elapsed += (QDateTime::currentMSecsSinceEpoch() - m_timestamp);

        QString error = NETWORK_ERROR_CODE_MAP[QNetworkReply::NoError];
        QString message = NETWORK_ERROR_MESSAGE_MAP[QNetworkReply::NoError];

        m_response->SetError(error);
        m_response->SetErrorMessage(message);
        m_response->SetElapsed(m_elapsed);
        m_response->SetStatusCode(m_networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).value<int>());

        if(m_response->IsLogEnabled())
            PrintResponseLog();

        Finished();
    }

    void UploadTask::onReplyError(QNetworkReply::NetworkError code)
    {
        m_timer->disconnect();
        m_timer->stop();
        m_elapsed += (QDateTime::currentMSecsSinceEpoch() - m_timestamp);

        // abort()的错误码替换为TimeoutError
        if(code == QNetworkReply::NetworkError::OperationCanceledError)
            code = QNetworkReply::NetworkError::TimeoutError;

        // 超时重试
        int count = m_request->GetTimeoutRetryCount();
        if(count > 0 && code == QNetworkReply::NetworkError::TimeoutError)
        {
            m_request->SetTimeoutRetryCount(--count);
            m_networkReply->disconnect();
            m_networkReply->close();
            m_networkReply->deleteLater();
            m_networkReply = nullptr;

            LOG_ERROR("http upload timeout, retry: {}", count);
            Run();

            return;
        }

        QString error = NETWORK_ERROR_CODE_MAP[code];
        const QString message = NETWORK_ERROR_MESSAGE_MAP[code];

        m_response->SetError(error);
        m_response->SetErrorMessage(message);
        m_response->SetElapsed(m_elapsed);
        m_response->SetStatusCode(m_networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).value<int>());

        if(m_response->IsLogEnabled())
            PrintResponseLog();

        Finished();
    }

    void UploadTask::onReplySslErrors(const QList<QSslError>& errors)
    {
        m_timer->disconnect();
        m_timer->stop();
        m_elapsed += (QDateTime::currentMSecsSinceEpoch() - m_timestamp);

        m_response->SetError(ERR_NETWORK_SSL_ERROR);
        for(const auto& i : errors)
        {
            LOG_ERROR("SSL ERROR: {}", i.errorString().toStdString());
            m_response->SetErrorMessage(i.errorString());
        }

        Finished();
    }

    void UploadTask::onReplyUploadProgress(qint64 sent, qint64 total)
    {
        if(m_request->GetProgressCallback() != nullptr && total > 0)
            m_request->GetProgressCallback()(m_request, m_response, sent, total, true);
    }

    void UploadTask::onTimeout()
    {
        Q_ASSERT(m_networkReply);

        if(IsExpired(QDateTime::currentMSecsSinceEpoch()))
        {
            m_timer->disconnect();
            m_timer->stop();

            if(m_networkReply)
                m_networkReply->abort();
        }
    }
}