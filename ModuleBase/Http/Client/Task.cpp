#include <QJsonDocument>

#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Util/MathUtil.h"
#include "Task.h"

// 任务超时轮询时间间隔(ms)
const int TIMEOUT_CHECK_INTERVAL = 100;

namespace Http
{
    Task::Task(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent/* = nullptr*/)
        : QObject(parent)
        , m_request(request)
        , m_response(response)
    {
        m_timer = new QTimer(this);
        m_uuid = Base::MathUtil::GetUUID();
    }

    Task::~Task()
    {
        if(m_networkReply)
        {
            m_networkReply->disconnect();
            m_networkReply->close();
            m_networkReply->deleteLater();
            m_networkReply = nullptr;
        }
    }

    void Task::SetNetworkAccessManager(QNetworkAccessManager* networkAccessManager)
    {
        m_networkAccessManager = networkAccessManager;
    }

    void Task::SetRawHeader(QNetworkRequest& request)
    {
        for(const auto& key : m_request->GetHeader().keys())
            request.setRawHeader(key.toUtf8(), m_request->GetHeader()[key].toUtf8());
    }

    bool Task::IsExpired(qint64 timestamp) const
    {
        Q_ASSERT(m_request);

        if(m_request->GetTimeout() <= 0)
            return false;

        return (timestamp - m_timestamp >= m_request->GetTimeout());
    } 

    void Task::PrintRequestLog()
    {
        Q_ASSERT(m_request);

        QByteArray contentType = m_request->GetHeader()["Content-Type"].toUtf8();
        QByteArray data = QByteArray::fromPercentEncoding(m_request->GetData());
        QString method = m_request->GetVerb() == Verb::GET ? "GET" : "POST";
        QString header = QJsonDocument(m_request->GetHeaderJson()).toJson(QJsonDocument::Compact);
        QString url = m_request->GetUrl();

        if(contentType == "application/x-www-form-urlencoded")
        {
            if(m_request->GetLogFormat() == LogFormat::DETAIL)
            {
                LOG_INFO("HTTP-CLIENT-{} [{}] Request: {}?{} Header: {}", 
                    method.toStdString(),
                    m_uuid.toStdString(), 
                    url.toStdString(), 
                    data.toStdString(), 
                    header.toStdString());
            }
            else
            {
                LOG_INFO("HTTP-CLIENT-{} [{}] Request: {} Size: {} KB", 
                    method.toStdString(), 
                    m_uuid.toStdString(), 
                    m_request->GetUrl().toStdString(), 
                    QString::number(data.size() / 1024.0, 'f', 2).toStdString());
            }
        }
        else if(contentType == "application/json")
        {
            LOG_INFO("HTTP-CLIENT-{} [{}] Request: {} Header: {} Content: {}", 
                method.toStdString(),
                m_uuid.toStdString(),
                url.toStdString(), 
                header.toStdString(), 
                data.toStdString());
        }
        else
        {
            Q_ASSERT(0);
        }
    }

    void Task::PrintResponseLog()
    {
        Q_ASSERT(m_request);
        Q_ASSERT(m_response);

        QString method = m_request->GetVerb() == Verb::GET ? "GET" : "POST";
        QByteArray data = m_response->GetData();
        qint64 elapsed = m_response->GetElapsed();
        int code = m_response->GetStatusCode();

        if(m_response->GetLogFormat() == LogFormat::DETAIL)
        {
            LOG_INFO("HTTP-CLIENT-{} [{}] Response: Time:{} ms Status:{} Size:{} KB Content: {}",
                method.toStdString(),
                m_uuid.toStdString(),
                elapsed,
                code,
                QString::number(data.size() / 1024.0, 'f', 2).toStdString(),
                data.toStdString());
        }
        else
        {
            LOG_INFO("HTTP-CLIENT-{} [{}] Response: Time:{} ms Status:{} Size:{} KB",
                method.toStdString(),
                m_uuid.toStdString(),
                elapsed,
                code,
                QString::number(m_response->GetData().size() / 1024.0, 'f', 2).toStdString());
        }
    }
}