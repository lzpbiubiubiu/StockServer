#pragma once

#include "ModuleData/ModuleDataApi.h"
#include "ModuleBase/Database/Base/DBBase.h"

namespace Sql
{
    /** StockServer配置数据库 */
    class MODULE_DATA_API StockServerDB : public Sql::DBBase
    {
    public:
        virtual bool CreateTable() override;
    };
}