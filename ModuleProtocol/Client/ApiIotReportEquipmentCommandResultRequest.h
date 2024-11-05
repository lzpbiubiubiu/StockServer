#pragma once

/**
* @file         ApiIotReportEquipmentCommandResultRequest.h
* @brief        设备指令回执上报请求
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "ModuleProtocol/ModuleProtocolApi.h"
#include "PosRequest.h"

namespace Http
{
    /** 设备指令回执上报请求，${domain}/api/iot/reportEquipmentCommandResult */
    class MODULE_PROTOCOL_API ApiIotReportEquipmentCommandResultRequest : public PosRequest
    {
    public:
        ApiIotReportEquipmentCommandResultRequest(QObject* parent = Q_NULLPTR);
        ~ApiIotReportEquipmentCommandResultRequest();

        void SetExternalCode(const QString& externalCode);
        void SetProductKey(const QString& productKey);
        void SetPayload(const QString& payload);

    protected:
        virtual void MakeJsonParam(QJsonObject& val) override;

    private:
        QString m_externalCode;
        QString m_productKey;
        QString m_payload;
    };
}