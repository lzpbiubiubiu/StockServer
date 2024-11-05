#pragma once
#include "Service.h"

namespace Base
{
    /** 同步服务基类实现 */
    class MODULE_BASE_API SyncService : public Service
    {
        Q_OBJECT

    public:
        SyncService(QObject* parent = nullptr);
        virtual ~SyncService();

        virtual State GetState() override;

    protected:
        virtual bool OnStart() override;

        virtual void OnAfterStart() override;

        virtual void OnStop() override;

        virtual void OnBeforeStop() override;

        virtual bool IsAsync() override;
    };
}