#include "Request.h"

namespace Http
{
    Request::Request(QObject* parent/* = nullptr*/)
        : QObject(parent)
    {
    }

    void Request::SetUrl(const QString& url)
    {
        m_url.setUrl(url);
    }

    QString Request::GetUrl()
    {
        return m_url.url();
    }

    QUrl Request::GetQUrl()
    {
        return m_url;
    }

    QString Request::GetHost()
    {
        return m_url.host();
    }

    QString Request::GetPath()
    {
        return m_url.path();
    }

    int Request::GetPort()
    {
        return m_url.port();
    }

    void Request::SetHeader(const QString& key, const QString& value)
    {
        m_header[key] = value;
    }

    const QMap<QString, QString>& Request::GetHeader() const
    {
        return m_header;
    }

    QJsonObject Request::GetHeaderJson() const
    {
        QJsonObject json;
        for(auto& key : m_header.keys())
            json.insert(key, m_header.value(key));
        return json;
    }

    void Request::SetMultiPart(QSharedPointer<QHttpMultiPart> multiPart)
    {
        m_multiPart = multiPart;
    }

    QSharedPointer<QHttpMultiPart> Request::GetMultiPart() const
    {
        return m_multiPart;
    }

    void Request::SetData(const QByteArray& data)
    {
        m_data = data;
    }

    const QByteArray& Request::GetData() const
    {
        return m_data;
    }

    void Request::SetCompleteCallback(CompleteCallback callback)
    {
        m_completeCallback = callback;
    }

    CompleteCallback Request::GetCompleteCallback() const
    {
        return m_completeCallback;
    }

    void Request::SetProgressCallback(ProgressCallback callback)
    {
        m_progressCallback = callback;
    }

    ProgressCallback Request::GetProgressCallback() const
    {
        return m_progressCallback;
    }

    void Request::SerializeForPost(QByteArray& ba)
    {}
}