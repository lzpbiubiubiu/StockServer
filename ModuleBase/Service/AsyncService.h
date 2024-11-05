#pragma once
#include "Service.h"

// 冻结标识，提供单一操作互斥宏
#define SUPPORT_FROZEN_FLAG()\
    protected:\
        QMutex m_frozenLock;\
        bool m_isFrozen = false;

#define CHECK_FROZEN_FLAG()         do { QMutexLocker locker(&m_frozenLock); if(m_isFrozen) return; } while(0)
#define RESET_FROZEN_FLAG()         do { QMutexLocker locker(&m_frozenLock); m_isFrozen = false; } while(0)
#define CHECK_AND_SET_FROZEN_FLAG() do { QMutexLocker locker(&m_frozenLock); if(m_isFrozen) return; m_isFrozen = true; } while(0)

namespace Base
{
    /** 异步服务基类实现 */
    class MODULE_BASE_API AsyncService : public Service
    {
        Q_OBJECT

    public:
        AsyncService(QObject* parent = nullptr);
        virtual ~AsyncService();

        virtual State GetState() override;

    protected:
        virtual bool OnStart() override;

        virtual void OnAfterStart() override;

        virtual void OnStop() override;

        virtual void OnBeforeStop() override;

        virtual bool IsAsync() override;
    };
}