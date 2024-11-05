#pragma once

/**
* @file         RemoteConnection.h
* @brief        远端伪长连接实现
* @details      通过Http实现远端伪长连接，通过定时器定时发送心跳包，保持连接，并执行相应的远端指令
*               如：日志提取、数据库提取、崩溃Dump上报等
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QJsonObject>
#include <QObject>
#include <QThread>
#include <QTimer>

#include "ModuleBase/Service/AsyncService.h"
#include "ModuleProtocol/Client/PosRequest.h"
#include "ModuleProtocol/Client/PosResponse.h"
#include "ConnectivityManager.h"
#include "UrlManager.h"

namespace Extra
{
    /** 远端连接类 */
    class RemoteManager : public Base::AsyncService
    {
        Q_OBJECT

    public:
        /** 指令 */
        struct Command
        {
            /** uuid */
            QString uuid;

            /** id */
            QString id;

            /** key */
            QString key;

            /** 参数 */
            QJsonObject params;

            /** 是否需要回执 */
            bool needReply = false;
        };

    public:
        RemoteManager(QObject* parent = nullptr);
        virtual ~RemoteManager();

        /** 上报dmp文件 */
        void UploadDumpFile();

        /** 开始心跳 */
        void StartHeartbeat();

    protected:
        virtual bool OnStart() override;

        virtual void OnStop() override;

        virtual void OnAfterStart() override;

        virtual void OnBeforeStop() override;

        Q_INVOKABLE void onUploadDumpFile();

        Q_INVOKABLE void onStartHeartbeat();

    protected Q_SLOTS:
        /** 定时器响应 */
        void onTimeout();

        /** 网络连接响应 */
        void onOnlineServiceConnected();

    private:
        /** 解析指令 */
        Command ParseCommand(const QString& commandStr) const;

        /** 执行指令 */
        void ExecCommand();

        /** 上传日志命令 */
        void OnCommandUploadLog(const Command& command);

        /** 上传dump命令 */
        void OnCommandUploadDump(const Command& command);

        /** 上传数据库命令 */
        void OnCommandUploadDb(const Command& command);

        /** 上传指定文件命令 */
        void OnCommandUploadFile(const Command& command);

        /** 删除指定文件命令 */
        void OnCommandDeleteFile(const Command& command);

        /** 上传文件 */
        void UploadFile(const Command& command, const QString& path, const QString& uploadDir);

        /** 上传文件结果 */
        Q_INVOKABLE void OnUploadResult(const Command& command, QSharedPointer<Http::Request> request, QSharedPointer<Http::Response> response);

        /** 回复指令执行结果 */
        void ReplyCommand(const QJsonObject& payload);

    private:
        // 心跳定时器
        QTimer* m_timer = nullptr;

        // 是否发送设备在线请求
        // 第一次心跳连接需要发送设备在线请求
        bool m_sendDeviceOnline = true;

        // 执行指令集
        QList<Command> m_commands;

        // url管理器
        UrlManager* m_um = nullptr;

        // 连接管理器
        ConnectivityManager* m_cm = nullptr;
    };
}// namespace Extra