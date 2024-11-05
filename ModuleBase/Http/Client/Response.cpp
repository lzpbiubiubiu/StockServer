#include "Response.h"

namespace Http
{
    Response::Response(QObject* parent/* = nullptr*/)
        : QObject(parent)
    {
        //qRegisterMetaType<Response>();
    }

    void Response::AppendData(const QByteArray& ba)
    {
        m_data.append(ba);
    }

    const QByteArray& Response::GetData() const
    {
        return m_data;
    }

    void Response::ClearData()
    {
        m_data.clear();
    }

    void Response::SetError(const QString& error)
    {
        m_error = error;
    }

    void Response::SetErrorMessage(const QString& errorMessage)
    {
        m_errorMessage = errorMessage;
    }

    QString Response::GetError() const
    {
        return m_error;
    }

    QString Response::GetErrorMessage() const
    {
        return m_errorMessage;
    }

    void Response::Reset()
    {
        m_data.clear();
        m_error.clear();
        m_errorMessage.clear();
        m_elapsed = 0;
        m_statusCode = 0;
        m_logEnabled = true;
        m_logFormat = LogFormat::DETAIL;
        m_downloadSize = 0;
    }

    bool Response::IsSuccess() const
    {
        return m_error == ERR_NETWORK_SUCCESS;
    }

    void Response::Deserialize(const QByteArray& ba) 
    {}
}