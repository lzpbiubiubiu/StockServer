#pragma once

#include <QThread>
#include <QReadWriteLock>
#include <QMetaObject>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include "IService.h"

namespace Base
{
    /** 服务基类实现 */
    class MODULE_BASE_API Service : public QObject, public IService
    {
        Q_OBJECT
        friend class ServiceManager;

    public:
        Service(QObject* parent = nullptr) : QObject(parent) {};
        virtual ~Service() {};

    Q_SIGNALS:
        /** 服务启动信号 */
        void signalStarted(bool success);

        /** 服务停止信号 */
        void signalStopped();

    protected:
        QThread m_thread;
        QReadWriteLock m_rwLock;
        State m_state = State::STOPPED;
    };
}