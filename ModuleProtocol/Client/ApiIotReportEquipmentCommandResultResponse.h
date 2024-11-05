#pragma once

/**
* @file         ApiIotReportEquipmentCommandResultResponse.h
* @brief        设备指令回执上报响应
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "ModuleProtocol/ModuleProtocolApi.h"
#include "PosResponse.h"

namespace Http
{
    /** 设备指令回执上报返回 */
    class MODULE_PROTOCOL_API ApiIotReportEquipmentCommandResultResponse : public PosResponse
    {
    public:
        struct Result
        {
        };

        ApiIotReportEquipmentCommandResultResponse(QObject *parent = Q_NULLPTR);
        ~ApiIotReportEquipmentCommandResultResponse();

        const Result& GetResult() const;

    protected:
        virtual void ParseJsonParam(const QJsonObject& val) override;

    private:
        Result m_result;
    };
}