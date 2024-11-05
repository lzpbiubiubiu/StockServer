// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "ModuleProtocol/ModuleProtocolApi.h"
#include "PosRequest.h"

namespace Http
{
    /** 自动对账，${domain}/api/posman/offlineCheckAccounts*/
    class MODULE_PROTOCOL_API OfflineCheckAccountsRequest : public PosRequest
    {
    public:
        OfflineCheckAccountsRequest(QObject* parent = Q_NULLPTR);
        ~OfflineCheckAccountsRequest();

        void SetExternalCode(const QString& code);

        void SetOfflineCheckInfo(const QJsonArray& info);
    protected:
        virtual void MakeJsonParam(QJsonObject& val) override;

    private:
        // 设备MAC地址
        QString m_externalCode;

        // 对账信息列表
        QJsonArray m_offlineCheckInfo;
    };
}