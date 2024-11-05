// Copyright (c) 2022 Dmall Pos Team

#pragma once

#include "ModuleProtocol/ModuleProtocolApi.h"
#include "PosRequest.h"

namespace Http
{
    /** 检测升级，${domain}/galaxy/version */
    class MODULE_PROTOCOL_API GalaxyVersionRequest : public PosRequest
    {
    public:
        GalaxyVersionRequest(QObject* parent = Q_NULLPTR);
        ~GalaxyVersionRequest();

        void SetDeviceId(const QString& deviceId);

        void SetCurrentVersion(const QString& currentVersion);

        void SetPackageName(const QString& packageName);

        void SetAppId(const QString& appId);

    protected:
        virtual void MakeJsonParam(QJsonObject& val) override;

        // 设备ID
        QString  m_deviceId;

        // 当前版本号
        QString  m_currentVersion;

        //包名
        QString m_packageName;

        //appId
        QString m_appId;
    };

}