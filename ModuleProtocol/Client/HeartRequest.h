#pragma once

/**
* @file         HeartRequest.h
* @brief        心跳连接Request
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "PosRequest.h"
#include "ModuleProtocol/ModuleProtocolApi.h"

namespace Http
{
    /** 心跳请求，${domain}/heart */
    class MODULE_PROTOCOL_API HeartRequest : public PosRequest
    {
        Q_OBJECT

    public:
        /** 操作类型 */
        enum OperateType
        {
            NOMAL = 0,  // 正常
            ONLION = 1, // 上线
            OFFLINE = 2 // 下线
        };

    public:
        HeartRequest(QObject* parent = Q_NULLPTR);
        ~HeartRequest();

        void SetTimestamp(qint64 timestamp);
        void SetProductKey(const QString& productKey);
        void SetOperateType(OperateType operateType);

    protected:
        virtual void MakeJsonParam(QJsonObject& val) override;

    private:
        // 时间戳
        qint64 m_timestamp = 0;

        // 产品key
        QString m_productKey;

        // 操作类型
        OperateType m_operateType = NOMAL;
    };
}