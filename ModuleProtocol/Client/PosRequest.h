#pragma once

/**
* @file         PosRequest.h
* @brief        Pos请求基类
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "ModuleBase/Http/Client/Request.h"
#include "ModuleProtocol/ModuleProtocolApi.h"

namespace Http
{
    /** 交易网关HTTP请求基类 */
    class MODULE_PROTOCOL_API PosRequest : public Request
    {
        Q_OBJECT

    public:
        PosRequest(QObject* parent = nullptr);
        virtual ~PosRequest();

        /** 序列化请求数据 */
        virtual void SerializeForPost(QByteArray& ba) override;

        QString GetAppVersion() const;
        void SetAppVersion(const QString& version);

        QString GetNonce() const;
        void SetNonce(const QString& nonce);

        QString GetChannel() const;
        void SetChannel(const QString& channel);

        QString GetPlatform() const;
        void SetPlatform(const QString& platform);

        QString GetAppKey() const;
        void SetAppKey(const QString& appKey);

        QString GetDeviceCode() const;
        void SetDeviceCode(const QString& deviceCode);

        QString GetDeviceId() const;
        void SetDeviceId(const QString& deviceId);

        QString GetVenderId() const;
        void SetVenderId(const QString& venderId);

        QString GetStoreId() const;
        void SetStoreId(const QString& storeId);

    protected:
        /** 请求数据json组装 */
        virtual void MakeJsonParam(QJsonObject& val);

    protected:

        // 平台
        QString m_platform;

        // 系统来源
        QString m_source;

        // 设备编码（设备MAC地址）
        QString m_deviceCode;

        // 设备ID（设备唯一标识）
        QString m_deviceId;

        // 商家ID
        QString m_venderId;

        // 门店ID
        QString m_storeId;

        // 客户端版本号 (放请求头)
        QString m_appVersion;

        // 请求来源（dpos、cpos、pos）
        QString m_channel;

        // 客户端授权key
        QString m_appKey;

        // 16位随机字符串
        QString m_nonce;
    };
}

//Q_DECLARE_METATYPE(Http::Request)