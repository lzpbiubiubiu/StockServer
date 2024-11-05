#pragma once

/**
* @file         HeartResponse.h
* @brief        心跳连接Response
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "PosResponse.h"
#include "ModuleProtocol/ModuleProtocolApi.h"

namespace Http
{
    /** 心跳返回 */
    class MODULE_PROTOCOL_API HeartResponse : public PosResponse
    {
        Q_OBJECT

    public:
        struct Result
        {
            /** 指令合集 */
            QStringList commandList;
        };

    public:
        HeartResponse(QObject *parent = Q_NULLPTR);
        ~HeartResponse();

        const Result& GetResult() const;

    protected:
        virtual void ParseJsonParam(const QJsonObject& val) override;

    private:
        Result m_result;
    };
}