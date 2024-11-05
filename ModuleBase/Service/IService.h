#pragma once

#include "ModuleBase/ModuleBaseApi.h"

namespace Base
{
    /** 抽象服务接口 */
    class MODULE_BASE_API IService
    {
    public:
        /** 服务状态 */
        enum State
        {
            /** 运行中 */
            RUNNING = 0,

            /** 已停止 */
            STOPPED
        };

        IService() = default;
        virtual ~IService() = default;

        /** 获取服务状态 */
        virtual State GetState() = 0;

    protected:
        /** 服务启动前回调 */
        virtual bool OnStart() = 0;

        /** 服务启动成功后回调 */
        virtual void OnAfterStart() = 0;

        /** 服务停止后回调 */
        virtual void OnStop() = 0;

        /** 服务停止前回调 */
        virtual void OnBeforeStop() = 0;

        /** 是否是异步服务 */
        virtual bool IsAsync() = 0;
    };
}