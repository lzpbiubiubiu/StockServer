#include <QJsonDocument>
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "PosRequest.h"

namespace Http
{
    PosRequest::PosRequest(QObject *parent)
        : Request(parent)
    {
        SetVerb(Verb::POST);
        SetSslEnabled(true);

        auto config = Core::GlobalData::Get()->GetConfig();
        SetNonce(Base::MathUtil::GetNonce());
        SetChannel(QStringLiteral("dpos"));
        SetPlatform(QStringLiteral("Dpos"));
        SetAppVersion(config.version.string);
        SetAppKey(Core::GlobalData::Get()->GetAppKey());
        SetDeviceCode(config.device.deviceCode);
        SetDeviceId(config.device.deviceId);
        SetVenderId(config.device.venderId);
        SetStoreId(config.device.storeId);
        SetTimeout(5000);
    }

    PosRequest::~PosRequest()
    {}

    void PosRequest::MakeJsonParam(QJsonObject& val)
    {}

    void PosRequest::SerializeForPost(QByteArray& ba)
    {
        QJsonObject val;
        MakeJsonParam(val);

        // 网关公共参数
        ASSIGN_FROM_STRING_IF_NOT_EMPTY(val, "nonceStr", m_nonce);
        ASSIGN_FROM_STRING_IF_NOT_EMPTY(val, "platform", m_platform);
        ASSIGN_FROM_STRING_IF_NOT_EMPTY(val, "deviceCode", m_deviceCode);
        ASSIGN_FROM_STRING_IF_NOT_EMPTY(val, "deviceId", m_deviceId);
        ASSIGN_FROM_STRING_IF_NOT_EMPTY(val, "posId", m_deviceId);
        ASSIGN_FROM_INT64_IF_GREATER_THAN_ZERO(val, "tenantId", m_venderId.toLongLong());
        ASSIGN_FROM_INT64_IF_GREATER_THAN_ZERO(val, "orgId", m_storeId.toLongLong());

        QJsonDocument doc;
        doc.setObject(val);
        QByteArray jsonstr = doc.toJson(QJsonDocument::Compact).toPercentEncoding();
        ba.append("param=").append(jsonstr);

        QString signstr;
        for(auto& key : val.keys())
        {
            QJsonDocument valueDoc;
            QJsonValue value = val.value(key);
            if(value.isArray())
            {
                // List to String
                valueDoc.setArray(value.toArray());
                signstr += QString("%1=%2&").arg(key).arg(QString(valueDoc.toJson(QJsonDocument::Compact)));
            }
            else if(value.isObject())
            {
                // Object to String
                valueDoc.setObject(value.toObject());
                signstr += QString("%1=%2&").arg(key).arg(QString(valueDoc.toJson(QJsonDocument::Compact)));
            }
            else
            {
                // Basic types
                QString str = value.isDouble() ? QString::number(value.toVariant().toLongLong()) : value.toVariant().toString();
                if(!str.isEmpty())
                    signstr += QString("%1=%2&").arg(key).arg(str);
            }
        }
        signstr += QString("key=%1").arg(m_appKey);
        QByteArray sign = QCryptographicHash::hash(signstr.toUtf8(), QCryptographicHash::Md5).toHex().toUpper();

        // header参数
        SetHeader("Content-Type", "application/x-www-form-urlencoded");
        SetHeader("posId", m_deviceId);
        SetHeader("deviceId", m_deviceCode);
        SetHeader("tenantId", m_venderId);
        SetHeader("orgId", m_storeId);
        SetHeader("version", m_appVersion);
        SetHeader("dmall-locale", "en_US");
        SetHeader("sign", sign);
        SetHeader("groupId", m_venderId);
        SetHeader("traceId", Base::MathUtil::GetUUID());
        SetHeader("channel", m_channel);
    }

    QString PosRequest::GetAppVersion() const
    {
        return m_appVersion;
    }

    void PosRequest::SetAppVersion(const QString& version)
    {
        m_appVersion = version;
    }

    QString PosRequest::GetNonce() const
    {
        return m_nonce;
    }

    void PosRequest::SetNonce(const QString& nonce)
    {
        m_nonce = nonce;
    }

    QString PosRequest::GetChannel() const
    {
        return m_channel;
    }

    void PosRequest::SetChannel(const QString& channel)
    {
        m_channel = channel;
    }

    QString PosRequest::GetPlatform() const
    {
        return m_platform;
    }

    void PosRequest::SetPlatform(const QString& platform)
    {
        m_platform = platform;
    }

    QString PosRequest::GetAppKey() const
    {
        return m_appKey;
    }

    void PosRequest::SetAppKey(const QString& appKey)
    {
        m_appKey = appKey;
    }

    QString PosRequest::GetDeviceCode() const
    {
        return m_deviceCode;
    }

    void PosRequest::SetDeviceCode(const QString& deviceCode)
    {
        m_deviceCode = deviceCode;
    }

    QString PosRequest::GetDeviceId() const
    {
        return m_deviceId;
    }

    void PosRequest::SetDeviceId(const QString& deviceId)
    {
        m_deviceId = deviceId;
    }

    QString PosRequest::GetVenderId() const
    {
        return m_venderId;
    }

    void PosRequest::SetVenderId(const QString& venderId)
    {
        m_venderId = venderId;
    }

    QString PosRequest::GetStoreId() const
    {
        return m_storeId;
    }

    void PosRequest::SetStoreId(const QString& storeId)
    {
        m_storeId = storeId;
    }
}