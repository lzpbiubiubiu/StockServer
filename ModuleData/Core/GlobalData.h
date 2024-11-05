#pragma once
#include "ModuleBase/Common/Singleton.h"
#include "ModuleData/ModuleDataApi.h"
#include "Config.h"

namespace Core
{
    /** 全局共享数据对象 */
    class MODULE_DATA_API GlobalData
    {
        TO_BE_SINGLETON(GlobalData)

    public:
        /** 获取配置信息 */
        CommonConfig& GetConfig();

    private:
        GlobalData();
        ~GlobalData();

        // 配置数据
        CommonConfig m_config;
    };
}