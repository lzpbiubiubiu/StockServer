#include "AsyncService.h"

namespace Base
{
    AsyncService::AsyncService(QObject* parent)
        : Service(parent)
    {
        moveToThread(&m_thread);
        connect(&m_thread, &QThread::started, [this]()
        {
            QWriteLocker locker(&m_rwLock);
            if(OnStart())
            {
                m_state = State::RUNNING;
                emit signalStarted(true);
            }
            else
            {
                emit signalStarted(false);
            }
        });

        connect(&m_thread, &QThread::finished, [this]()
        {
            QWriteLocker locker(&m_rwLock);
            if(m_state == State::RUNNING)
            {
                OnStop();
                m_state = State::STOPPED;
            }
            emit signalStopped();
        });
    }

    AsyncService::~AsyncService()
    {}

    bool AsyncService::OnStart()
    {
        return true;
    }

    void AsyncService::OnAfterStart()
    {}

    void AsyncService::OnStop()
    {}

    void AsyncService::OnBeforeStop()
    {}

    Service::State AsyncService::GetState()
    {
        QReadLocker locker(&m_rwLock);
        return m_state;
    }

    bool AsyncService::IsAsync()
    {
        QReadLocker locker(&m_rwLock);
        return true;
    }
}