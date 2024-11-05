#include "VersionEntity.h"

ORM_REGISTER_CPP_STOCK_DATABASE(VersionEntity)

namespace Orm
{
    template<>
    void register_class(QdOrmClassDao<VersionEntity>& t)
    {
        ORM_REGISTER_QD_TABLE(VersionEntity, VERSION_TABLE_VERSION_V1)
        OrmMetaMemberPtr member = nullptr;

        // VERSION_TABLE_VERSION_V1
        member = ORM_REGISTER_QD_ID(VersionEntity, Name)
        member = ORM_REGISTER_QD_DATA(VersionEntity, Version, VERSION_TABLE_VERSION_V1)
        member->setIsNotNull(true);
    }
}
