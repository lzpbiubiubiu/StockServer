// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "ModuleData/ModuleDataApi.h"
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleData/Entity/OrderEntity.h"

namespace Core
{
    /** 数据类型转换工具类 */
    class MODULE_DATA_API ConvertUtil
    {
    public:
        // Sql::ConfigWareEntity <=> Core::ConfigWare
        static Sql::ConfigWareEntityPtr ToEntity(Core::ConfigWarePtr order);
        static Sql::ConfigWareEntityPtrList ToEntityList(const Core::ConfigWarePtrList& orders);
        static Core::ConfigWarePtr FromEntity(Sql::ConfigWareEntityPtr entity);
        static Core::ConfigWarePtrList FromEntityList(const Sql::ConfigWareEntityPtrList& entities);

        // Core::Order <=> Sql::OrderEntity
        static Sql::OrderEntityPtr ToEntity(Core::OrderPtr order);
        static Sql::OrderEntityPtrList ToEntityList(const Core::OrderPtrList& orders);
        static Core::OrderPtr FromEntity(Sql::OrderEntityPtr entity);
        static Core::OrderPtrList FromEntityList(const Sql::OrderEntityPtrList& entities);
    };
}