#include <QJsonDocument>

#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Util/MathUtil.h"
#include "DownloadTask.h"

// 任务超时轮询时间间隔(ms)
const int TIMEOUT_CHECK_INTERVAL = 100;

namespace Http
{
    DownloadTask::DownloadTask(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent/* = nullptr*/)
        : Task(request, response, parent)
    {
    }

    DownloadTask::~DownloadTask()
    {
        if(m_file)
        {
            m_file->close();
            m_file->deleteLater();
            m_file = nullptr;
        }
    }

    void DownloadTask::Run()
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

        // 当网络适配器从禁用状态恢复时，QNetworkAccessManager组件可能不可用，需要手动启用
        if(m_networkAccessManager->networkAccessible() != QNetworkAccessManager::Accessible)
            m_networkAccessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

        // 创建下载文件副本
        QString tmpFileName = m_request->GetDownloadFilePath() + ".tmp";
        m_file = new QFile(tmpFileName);
        if(!m_file->open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QString message = QString("create tmp file %1 failed").arg(tmpFileName);
            LOG_ERROR(message.toStdString());
            m_response->SetError(ERR_NETWORK_CREATE_FILE_FAILED);
            m_response->SetErrorMessage(message);
            Finished();
            return;
        }

        SetRawHeader(request);
        m_networkReply = m_networkAccessManager->get(request);
        if(m_request->IsLogEnabled())
            PrintRequestLog();

        m_timestamp = QDateTime::currentMSecsSinceEpoch();

        connect(m_networkReply, &QNetworkReply::readyRead, this, &DownloadTask::onReplyReadReady);
        connect(m_networkReply, &QNetworkReply::finished, this, &DownloadTask::onReplyFinished);
        connect(m_networkReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &DownloadTask::onReplyError);
        connect(m_networkReply, &QNetworkReply::sslErrors, this, &DownloadTask::onReplySslErrors);
        connect(m_networkReply, &QNetworkReply::downloadProgress, this, &DownloadTask::onReplyDownloadProgress);

        connect(m_timer, &QTimer::timeout, this, &DownloadTask::onTimeout, Qt::UniqueConnection);
        m_timer->start(TIMEOUT_CHECK_INTERVAL);
    }

    void DownloadTask::PrintRequestLog()
    {
        QByteArray data = QByteArray::fromPercentEncoding(m_request->GetData());
        QString header = QJsonDocument(m_request->GetHeaderJson()).toJson(QJsonDocument::Compact);
        QString url = m_request->GetUrl();

        if(m_request->GetLogFormat() == LogFormat::DETAIL)
        {
            LOG_INFO("HTTP-CLIENT-DOWNLOAD [{}] Request: {}?{} Header: {}",
                m_uuid.toStdString(),
                url.toStdString(),
                data.toStdString(),
                header.toStdString());
        }
        else
        {
            LOG_INFO("HTTP-CLIENT-DOWNLOAD [{}] Request: {} Size: {} KB",
                m_uuid.toStdString(),
                m_request->GetUrl().toStdString(),
                QString::number(data.size() / 1024.0, 'f', 2).toStdString());
        }
    }

    void DownloadTask::PrintResponseLog()
    {
        qint64 elapsed = m_response->GetElapsed();
        int code = m_response->GetStatusCode();

        LOG_INFO("HTTP-CLIENT-DOWNLOAD [{}] Response: Time:{} ms Status:{} Size:{} KB",
                m_uuid.toStdString(),
                elapsed,
                code,
                QString::number(m_response->GetDownloadSize() / 1024.0, 'f', 2).toStdString());
    }

    void DownloadTask::Finished()
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
        result->method = "GET";
        result->ok = m_response->GetError() == ERR_NETWORK_SUCCESS;
        result->status = m_response->GetStatusCode();
        result->error = m_response->GetError();;
        result->message = m_response->GetErrorMessage();
        result->elapsed = m_elapsed;
        result->sent = m_request->GetData().size();
        result->recv = m_response->GetDownloadSize();

        if(m_request->GetCompleteCallback() != nullptr)
            m_request->GetCompleteCallback()(m_request, m_response);

        emit signalFinished(result);
    }

    void DownloadTask::onReplyReadReady()
    {
        QByteArray data = m_networkReply->readAll();
        m_response->SetDownloadSize(m_response->GetDownloadSize() + data.size());
        if(m_file->write(data) < data.size())
        {
            QString message = QString("write tmp file %1 failed").arg(m_file->fileName());
            LOG_ERROR(message.toStdString());
            m_networkReply->abort();
        }
    }

    void DownloadTask::onReplyFinished()
    {
        // 更新文件内容
        m_file->flush();

        // 用户abort()
        QNetworkReply::NetworkError code = m_networkReply->error();
        if(code != QNetworkReply::NetworkError::NoError)
        {
            onReplyError(code);
            return;
        }

        // md5校验不通过
        if(!m_request->GetMd5().isEmpty())
        {
            QString md5 = Base::MathUtil::GetFileMD5(m_file->fileName());
            if(md5 != m_request->GetMd5())
            {
                onReplyError(QNetworkReply::NetworkError::OperationCanceledError);
                return;
            }
        }

        // 重命名文件
        m_file->close();
        QString newFileName = QString(m_file->fileName()).mid(0, m_file->fileName().size() - QString(".tmp").size());
        QFile::remove(newFileName);
        QFile::rename(m_file->fileName(), newFileName);

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

    void DownloadTask::onReplyError(QNetworkReply::NetworkError code)
    {
        m_file->flush();
        m_timer->disconnect();
        m_timer->stop();
        QFile::remove(m_file->fileName());
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

            LOG_ERROR("http download timeout, retry: {}", count);
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

    void DownloadTask::onReplySslErrors(const QList<QSslError>& errors)
    {
        m_file->flush();
        m_timer->disconnect();
        m_timer->stop();
        QFile::remove(m_file->fileName());
        m_elapsed += (QDateTime::currentMSecsSinceEpoch() - m_timestamp);

        m_response->SetError(ERR_NETWORK_SSL_ERROR);
        for(const auto& i : errors)
        {
            LOG_ERROR("SSL ERROR: {}", i.errorString().toStdString());
            m_response->SetErrorMessage(i.errorString());
        }

        Finished();
    }

    void DownloadTask::onReplyDownloadProgress(qint64 recv, qint64 total)
    {
        if(m_request->GetProgressCallback() != nullptr && total > 0)
            m_request->GetProgressCallback()(m_request, m_response, recv, total, false);
    }

    void DownloadTask::onTimeout()
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