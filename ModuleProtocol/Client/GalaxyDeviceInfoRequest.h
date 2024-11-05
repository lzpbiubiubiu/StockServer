// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "ModuleProtocol/ModuleProtocolApi.h"
#include "PosRequest.h"

namespace Http
{
    /** 银河设备信息请求，${domain}/galaxy/deviceInfo */
    class MODULE_PROTOCOL_API GalaxyDeviceInfoRequest : public PosRequest
    {
    public:
        GalaxyDeviceInfoRequest(QObject* parent = Q_NULLPTR);
        ~GalaxyDeviceInfoRequest();

        void SetExternalCode(const QString& code);

    protected:
        virtual void MakeJsonParam(QJsonObject& val) override;

    private:
        // 设备MAC地址
        QString m_externalCode;
    };
}