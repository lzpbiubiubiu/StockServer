#include "SyncService.h"

namespace Base
{
    SyncService::SyncService(QObject* parent)
        : Service(parent)
    {
        Q_UNUSED(m_thread);
        Q_UNUSED(m_rwLock);
    }

    SyncService::~SyncService()
    {}

    bool SyncService::OnStart()
    {
        return true;
    }

    void SyncService::OnAfterStart()
    {}

    void SyncService::OnStop()
    {}

    void SyncService::OnBeforeStop()
    {}

    Service::State SyncService::GetState()
    {
        return m_state;
    }

    bool SyncService::IsAsync()
    {
        return false;
    }
}