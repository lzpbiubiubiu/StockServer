#include "ConfigWareEntity.h"

ORM_REGISTER_CPP_STOCK_DATABASE(ConfigWareEntity)

namespace Orm
{
    template<>
    void register_class(QdOrmClassDao<ConfigWareEntity>& t)
    {
        ORM_REGISTER_QD_TABLE(ConfigWareEntity, WARE_ITEM_TABLE_VERSION_V1)
        OrmMetaMemberPtr member = nullptr;

        // WARE_ITEM_TABLE_VERSION_V1
        member = ORM_REGISTER_QD_ID(ConfigWareEntity, Code, WARE_ITEM_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(ConfigWareEntity, Name, WARE_ITEM_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(ConfigWareEntity, Stock, WARE_ITEM_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(ConfigWareEntity, RetailPrice, WARE_ITEM_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(ConfigWareEntity, WholesalePrice, WARE_ITEM_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(ConfigWareEntity, ImageFileName, WARE_ITEM_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(ConfigWareEntity, ImageMd5, WARE_ITEM_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(ConfigWareEntity, Extension, WARE_ITEM_TABLE_VERSION_V1)
    }
}
