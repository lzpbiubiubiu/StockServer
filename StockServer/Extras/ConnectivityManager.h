// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include <QTimer>
#include <QUrl>
#include <QTcpSocket>
#include <QHostInfo>
#include "ModuleBase/Service/AsyncService.h"

namespace Extra
{
    /** 网络连通性检测服务，实时监控网关连接状态 */
    class ConnectivityManager : public Base::AsyncService
    {
        Q_OBJECT
    public:
        ConnectivityManager(QObject* parent = nullptr);
        ~ConnectivityManager();

        /** 获取在线服务是否已连接 */
        bool IsOnlineServiceConnected() const;

    Q_SIGNALS:
        /** 在线服务已连接信号 */
        void signalOnlineServiceConnected();

        /** 在线服务已断开信号 */
        void signalOnlineServiceDisconnected();

    protected:
        virtual bool OnStart() override;
        virtual void OnStop() override;

        /**
         * @brief DetectNetwork 检测网络是否畅通
         * @param delaySec 延迟delaySec后执行检测
         */
        void DetectNetwork(quint64 delaySec);

        void SetOnlineServiceConnected(bool connected);

    private:
        // 网关是否已连接
        bool m_onlineServiceConnected = false;
    };
}