#include "KvEntity.h"

ORM_REGISTER_CPP_STOCK_DATABASE(KvEntity)

namespace Orm
{
    template<>
    void register_class(QdOrmClassDao<KvEntity>& t)
    {
        ORM_REGISTER_QD_TABLE(KvEntity, KV_TABLE_VERSION_V1)
        OrmMetaMemberPtr member = nullptr;

        // KV_TABLE_VERSION_V1
        member = ORM_REGISTER_QD_ID(KvEntity, Key)
        member = ORM_REGISTER_QD_DATA(KvEntity, Value, KV_TABLE_VERSION_V1)
    }
}
