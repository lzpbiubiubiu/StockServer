#pragma once

#include <QMap>
#include <QTimer>
#include <QReadWriteLock>
#include "ModuleBase/Service/AsyncService.h"

namespace Extra
{
    /** 终端管理服务 */
    class TerminalManager : public Base::AsyncService
    {
        Q_OBJECT
        SUPPORT_FROZEN_FLAG()

    public:
        enum TerminalType
        {
            POS = 0,
            PVT = 1
        };

        /** 终端数据结构 */
        struct Terminal
        {
            /** ID */
            QString id;

            /** 银台号 */
            QString deskNo;

            /** 设备编码 */
            QString code;

            /** 是否连接 */
            bool connected = false;

            /** 上次心跳时间戳 */
            qint64 heartTimestamp = 0;

            /** 未上传云端离线任务条数 */
            int notUploadCloudServer = 0;

            /** 未上传in-store-server离线任务条数 */
            int notUpLoadInStoreServer = 0;
        };

        TerminalManager(QObject* parent = nullptr);
        ~TerminalManager();

        /** 获取终端列表 */
        QMap<QString, Terminal> GetTerminals(TerminalType type);

        /** 刷新 */
        void Refresh();

    protected:
        virtual bool OnStart() override;
        virtual void OnStop() override;

        Q_INVOKABLE void onRefresh();

    public Q_SLOTS:
        void onCheckTimout();

        void onLoadDeviceSuccess();

        void onHeartbeat(int type, const QString& id,int cloud, int inStore);

    Q_SIGNALS:
        void signalConnected(TerminalType type, Terminal terminal);
        void signalDisconnected(TerminalType type, Terminal terminal);

        void signalRefreshSuccess(QMap<QString, Terminal> posTerminals, QMap<QString, Terminal> pvtTerminals);
        void signalRefreshError(const QString& message, const QString& code);

    private:
        // 检查定时器
        QTimer* m_checkTimer = nullptr;

        // pos终端列表
        QMap<QString, Terminal> m_posTerminals;

        // pvt终端列表
        QMap<QString, Terminal> m_pvtTerminals;

        // 终端列表锁
        QReadWriteLock m_terminalLock;
    };
}