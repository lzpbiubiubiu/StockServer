// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include <QTimer>
#include "Extras/ConfigManager.h"

namespace UI
{
    /** 状态栏控件 StatusBar */
    class StatusBar : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString version READ GetVersion WRITE SetVersion NOTIFY signalVersionChanged)
        Q_PROPERTY(QString time READ GetTime WRITE SetTime NOTIFY signalTimeChanged)
        Q_PROPERTY(bool netState READ GetNetState WRITE SetNetState NOTIFY signalNetStateChanged)

    public:
        StatusBar(QObject* parent = nullptr);
        ~StatusBar();

        const QString& GetVersion() const;
        void SetVersion(const QString& version);

        const QString& GetTime() const;
        void SetTime(const QString& time);

        bool GetNetState() const;
        void SetNetState(bool netState);

        Q_INVOKABLE void onQuitClicked();

    protected Q_SLOTS:
        void onTimeout();

        void onLoadDeviceSuccess();

    Q_SIGNALS:
        void signalVersionChanged();
        void signalTimeChanged();
        void signalNetStateChanged();
        
    private:

        // 版本号
        QString m_version = "0.0.0.0";

        // 系统时间
        QString m_time;

        // 网络状态
        bool m_netState = false;

        // 系统时间刷新定时器
        QTimer* m_timer = nullptr;

        // 配置管理器
        Extra::ConfigManager* m_configManager = nullptr;
    };
}
