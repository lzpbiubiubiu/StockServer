#pragma once
#include "ModuleBase/Database/Base/EntityBase.h"
#include "ModuleData/ModuleDataApi.h"

#define WARE_ITEM_TABLE_VERSION_V1 (1)
//#define WARE_ITEM_TABLE_VERSION_V2 (2)
//#define WARE_ITEM_TABLE_VERSION_V3 (3)

namespace Sql
{
    /** [tb_config_ware] 商品表  */
    class MODULE_DATA_API ConfigWareEntity : public Sql::EntityBase
    {
        Q_OBJECT

        ORM_REGISTER_FRIEND_CLASS(ConfigWareEntity)

        ORM_REGISTER_TABLE("tb_config_ware")

        // 商品编码
        ORM_REGISTER_COLUMN(QString, Code, "code")

        // 名称
        ORM_REGISTER_COLUMN(QString, Name, "name")

        // 商品剩余库存
        ORM_REGISTER_COLUMN(qint64, Stock, "stock")

        // 商品零售价格
        ORM_REGISTER_COLUMN(qint64, RetailPrice, "retail_price")

        // 商品批发价格
        ORM_REGISTER_COLUMN(qint64, WholesalePrice, "wholesale_price")

        // 扩展字段（通常是JSON格式字符串）
        ORM_REGISTER_COLUMN(QString, Extension, "extension")
    };

    using ConfigWareEntityPtr = QSharedPointer<ConfigWareEntity>;
    using ConfigWareEntityPtrList = QList<QSharedPointer<ConfigWareEntity>>;
}

using namespace Sql;
ORM_REGISTER_HPP_STOCK_DATABASE(ConfigWareEntity)
