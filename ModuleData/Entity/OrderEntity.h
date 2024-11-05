#pragma once

#include "ModuleBase/Database/Base/EntityBase.h"
#include "ModuleData/ModuleDataApi.h"

#define ORDER_TABLE_VERSION_V1 (1)
//#define ORDER_TABLE_VERSION_V2 (2)
//#define ORDER_TABLE_VERSION_V3 (3)

namespace Sql
{
    /** [tb_order] 订单表  */
    class MODULE_DATA_API OrderEntity : public Sql::EntityBase
    {
        Q_OBJECT

        ORM_REGISTER_FRIEND_CLASS(OrderEntity)

        ORM_REGISTER_TABLE("tb_order")

        // 订单UUID
        ORM_REGISTER_COLUMN(QString, Uuid, "uuid")

        // 订单小票号
        ORM_REGISTER_COLUMN(QString, ReceiptNo, "receipt_no")

        // 订单金额
        ORM_REGISTER_COLUMN(qint64, OrderAmount, "order_amount")

        // 订单促销总金额
        ORM_REGISTER_COLUMN(qint64, TotalPromotionAmount, "total_promotion_amount")  

        // 打车费用
        ORM_REGISTER_COLUMN(qint64, DeliveryFeesAmount, "delivery_fees_amount")

        // 订单类型
        ORM_REGISTER_COLUMN(QString, Type, "type")

        // 订单创建用户账号
        ORM_REGISTER_COLUMN(QString, UserAccount, "user_account")

        // 订单创建用户名称
        ORM_REGISTER_COLUMN(QString, UserName, "user_name")

        // 扩展字段
        ORM_REGISTER_COLUMN(QString, Extension, "extension")

        // 商品列表
        ORM_REGISTER_COLUMN(QString, Wares, "wares")

        // 促销列表
        ORM_REGISTER_COLUMN(QString, Promotions, "promotions")

        // 订单完成时间
        ORM_REGISTER_COLUMN(QDateTime, Time, "time")
    };

    using OrderEntityPtr = QSharedPointer<OrderEntity>;
    using OrderEntityPtrList = QList<QSharedPointer<OrderEntity>>;
}

using namespace Sql;
