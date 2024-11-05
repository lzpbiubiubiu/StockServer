#pragma once

#include "ModuleData/ModuleDataApi.h"
#include "ModuleBase/Database/Base/DBBase.h"

namespace Sql
{
    /** 销售数据库 */
    class MODULE_DATA_API RecordDB : public Sql::DBBase
    {
    public:
        virtual bool CreateTable() override;
    };
}