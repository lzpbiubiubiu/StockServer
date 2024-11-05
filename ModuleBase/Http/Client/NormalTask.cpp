#include <QJsonDocument>

#include "ModuleBase/Log/Log.h"
#include "NormalTask.h"

// 任务超时轮询时间间隔(ms)
const int TIMEOUT_CHECK_INTERVAL = 100;

namespace Http
{
    NormalTask::NormalTask(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent/* = nullptr*/)
        : Task(request, response, parent)
    {
    }

    void NormalTask::Run()
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

        QNetworkRequest requset;
        requset.setUrl(m_request->GetUrl());

        if(m_request->IsSslEnabled())
        {
            QSslConfiguration ssl;
            ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
            ssl.setProtocol(QSsl::TlsV1SslV3);
            requset.setSslConfiguration(ssl);
        }

        // 当网络适配器从禁用状态恢复时，QNetworkAccessManager组件可能不可用，需要手动启用
        if(m_networkAccessManager->networkAccessible() != QNetworkAccessManager::Accessible)
            m_networkAccessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

        if(m_request->GetVerb() == Verb::GET)
            ProcessGet(requset);
        else if(m_request->GetVerb() == Verb::POST)
            ProcessPost(requset);
        else
            Q_ASSERT(false);

        m_timestamp = QDateTime::currentMSecsSinceEpoch();

        connect(m_networkReply, &QNetworkReply::readyRead, this, &NormalTask::onReplyReadReady);
        connect(m_networkReply, &QNetworkReply::finished, this, &NormalTask::onReplyFinished);
        connect(m_networkReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &NormalTask::onReplyError);
        connect(m_networkReply, &QNetworkReply::sslErrors, this, &NormalTask::onReplySslErrors);
        connect(m_networkReply, &QNetworkReply::downloadProgress, this, &NormalTask::onReplyDownloadProgress);
        connect(m_networkReply, &QNetworkReply::uploadProgress, this, &NormalTask::onReplyUploadProgress);

        connect(m_timer, &QTimer::timeout, this, &NormalTask::onTimeout, Qt::UniqueConnection);
        m_timer->start(TIMEOUT_CHECK_INTERVAL);
    }

    void NormalTask::ProcessGet(QNetworkRequest& request)
    {
        SetRawHeader(request);
        m_networkReply = m_networkAccessManager->get(request);

        if(m_request->IsLogEnabled())
            PrintRequestLog();
    }

    void NormalTask::ProcessPost(QNetworkRequest& request)
    {
        // 优先以MultiPart类型数据作为POST内容
        if(!m_request->GetMultiPart().isNull())
        {
            SetRawHeader(request);
            m_networkReply = m_networkAccessManager->post(request, m_request->GetMultiPart().data());

            if(m_request->IsLogEnabled())
                PrintRequestLog();

            return;
        }
        
        QByteArray ba;
        m_request->SerializeForPost(ba);
        m_request->SetData(ba);

        //// gzip压缩
        //if(m_request->GetHeader().contains("Content-Encoding"))
        //{
        //    if(m_request->GetHeader()["Content-Encoding"] == "gzip")
        //        ba = Base::Compression::GzipCompress(ba);
        //}

        SetRawHeader(request);
        m_networkReply = m_networkAccessManager->post(request, ba);

        if(m_request->IsLogEnabled())
            PrintRequestLog();
    }

    void NormalTask::Finished()
    {
        if(m_networkReply)
        {
            m_networkReply->disconnect();
            m_networkReply->deleteLater();
            m_networkReply = nullptr;
        }

        auto result = QSharedPointer<Http::Result>::create();
        result->trace = m_uuid;
        result->url = m_request->GetUrl();
        result->method = (m_request->GetVerb() == Verb::GET) ? "GET" : "POST";
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

    void NormalTask::onReplyReadReady()
    {
        m_response->AppendData(m_networkReply->readAll());
    }

    void NormalTask::onReplyFinished()
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

        // 302重定向
        const QVariant redirection = m_networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if(m_request->IsRedirectionEnabled() && !redirection.isNull())
        {
            m_networkReply->disconnect();
            m_networkReply->deleteLater();
            m_networkReply = nullptr;

            m_request->SetUrl(redirection.toUrl().toString());
            Run();

            return;
        }

        QString error = NETWORK_ERROR_CODE_MAP[QNetworkReply::NoError];
        QString message = NETWORK_ERROR_MESSAGE_MAP[QNetworkReply::NoError];

        m_response->SetError(error);
        m_response->SetErrorMessage(message);
        m_response->SetElapsed(m_elapsed);
        m_response->SetStatusCode(m_networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).value<int>());
        m_response->Deserialize(m_response->GetData());

        if(m_response->IsLogEnabled())
            PrintResponseLog();

        Finished();
    }

    void NormalTask::onReplyError(QNetworkReply::NetworkError code)
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
            m_networkReply->deleteLater();
            m_networkReply = nullptr;

            Run();
            LOG_ERROR("http request timeout, retry: {}", count);

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

    void NormalTask::onReplySslErrors(const QList<QSslError>& errors)
    {
        m_timer->disconnect();
        m_timer->stop();

        for(const auto& i : errors)
        {
            LOG_ERROR("SSL ERROR: {}", i.errorString().toStdString());
        }

        Finished();
    }

    void NormalTask::onReplyDownloadProgress(qint64 recv, qint64 total)
    {
        if(m_request->GetProgressCallback() != nullptr && total > 0)
            m_request->GetProgressCallback()(m_request, m_response, recv, total, false);
    }

    void NormalTask::onReplyUploadProgress(qint64 sent, qint64 total)
    {
        if(m_request->GetProgressCallback() != nullptr && total > 0)
            m_request->GetProgressCallback()(m_request, m_response, sent, total, true);
    }

    void NormalTask::onTimeout()
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