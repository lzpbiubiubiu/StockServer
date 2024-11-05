#include "OrderEntity.h"

ORM_REGISTER_CPP_STOCK_DATABASE(OrderEntity)

namespace Orm
{
    template<>
    void register_class(QdOrmClassDao<OrderEntity>& t)
    {
        ORM_REGISTER_QD_TABLE(OrderEntity, ORDER_TABLE_VERSION_V1)
        OrmMetaMemberPtr member = nullptr;

        // ORDER_TABLE_VERSION_V1
        member = ORM_REGISTER_QD_ID(OrderEntity, ReceiptNo)
        member = ORM_REGISTER_QD_DATA(OrderEntity, Uuid, ORDER_TABLE_VERSION_V1)
        member->setIsIndex(true);
        member = ORM_REGISTER_QD_DATA(OrderEntity, OrderAmount, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, TotalPromotionAmount, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, DeliveryFeesAmount, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, Type, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, UserAccount, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, UserName, ORDER_TABLE_VERSION_V1)
        member->setIsIndex(true);
        member = ORM_REGISTER_QD_DATA(OrderEntity, Extension, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, Wares, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, Promotions, ORDER_TABLE_VERSION_V1)
        member = ORM_REGISTER_QD_DATA(OrderEntity, Time, ORDER_TABLE_VERSION_V1)
    }
}
