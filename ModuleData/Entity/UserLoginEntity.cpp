#include "UserLoginEntity.h"

ORM_REGISTER_CPP_STOCK_DATABASE(UserLoginEntity)

namespace Orm
{
    template<>
    void register_class(QdOrmClassDao<UserLoginEntity>& t)
    {
        ORM_REGISTER_QD_TABLE(UserLoginEntity, USER_TABLE_VERSION_V1)
        OrmMetaMemberPtr member = nullptr;

        // USER_TABLE_VERSION_V1
        member = ORM_REGISTER_QD_ID(UserLoginEntity, Account)
        member = ORM_REGISTER_QD_DATA(UserLoginEntity, Password, USER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(UserLoginEntity, Tel, USER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(UserLoginEntity, Name, USER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(UserLoginEntity, Status, USER_TABLE_VERSION_V1)
    }
}