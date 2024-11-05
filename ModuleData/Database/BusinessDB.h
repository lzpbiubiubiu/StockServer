#pragma once

#include "ModuleData/ModuleDataApi.h"
#include "ModuleBase/Database/Base/DBBase.h"

namespace Sql
{
    /** 业务数据库 */
    class MODULE_DATA_API BusinessDB : public Sql::DBBase
    {
    public:
        virtual bool CreateTable() override;
    };
}
