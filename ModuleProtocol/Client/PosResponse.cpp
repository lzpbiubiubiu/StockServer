#include <QJsonDocument>

#include "PosResponse.h"

namespace Http
{
    PosResponse::PosResponse(QObject* parent)
        : Response(parent)
    {
       // qRegisterMetaType<PosResponse>();
    }

    PosResponse::~PosResponse()
    {}

    void PosResponse::ParseJsonParam(const QJsonObject& val)
    {}

    void PosResponse::ParseJsonParam(const QJsonArray& val)
    {}

    void PosResponse::ParseJsonParam(bool val)
    {}

    void PosResponse::ParseJsonParam(double val)
    {}

    bool PosResponse::IsSuccess() const
    {
        return Response::IsSuccess() && (GetStatusCode() == 200) && (m_resultCode == "0000");
    }

    void PosResponse::Deserialize(const QByteArray& ba)
    {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(ba, &error);
        if(error.error != QJsonParseError::NoError)
            return;

        QJsonObject root = doc.object();

        m_resultCode = root["code"].toString();
        m_resultMessage = root["displayMsg"].toString();

        if(m_resultCode == "0000" && root["success"].toBool())
        {
            if(root["data"].isArray())
                ParseJsonParam(root["data"].toArray());
            else if(root["data"].isObject())
                ParseJsonParam(root["data"].toObject());
            else if(root["data"].isBool())
                ParseJsonParam(root["data"].toBool());
            else if(root["data"].isDouble())
                ParseJsonParam(root["data"].toDouble());
            else
                Q_ASSERT(false);
        }
    }

    QString PosResponse::GetError() const
    {
        // 网络错误返回网络错误码，否则返回网关业务错误码
        if(!Response::IsSuccess())
            return m_error;
        else
            return m_resultCode;
    }

    QString PosResponse::GetErrorMessage() const
    {
        // 网络错误返回网络错误描述，否则返回网关业务错误描述
        if(!Response::IsSuccess())
            return m_errorMessage;
        else
            return m_resultMessage;
    }

    QString PosResponse::GetDisplayErrorCode() const
    {
        // 网络错误返回网络错误码，否则返回网关业务错误码
        if(!Response::IsSuccess())
            return m_error;
        else
            return m_resultDisplayCode;
    }
}