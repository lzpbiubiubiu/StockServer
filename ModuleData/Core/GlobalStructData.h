#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QSharedPointer>

namespace Core
{
    /** 配置商品 */
    struct ConfigWare
    {

        /** 名称 */
        QString name;

        /** 商品编码 */
        QString code;

        /** 库存 */
        qint64 stock = 0;

        /** 单品零售价格 */
        qint64 retailPrice = 0;

        /** 单品批发价格 */
        qint64 wholesalePrice = 0;

        /** 扩展字段 */
        QJsonObject extension;

        Q_GADGET
    };
    using ConfigWarePtr = QSharedPointer<ConfigWare>;
    using ConfigWarePtrList = QList<QSharedPointer<ConfigWare>>;

    /** 订单*/
    struct Order
    {
        /** 配置商品 */
        struct WareItem
        {
            /** 名称 */
            QString name;

            /** 商品编码 */
            QString code;

            /** 商品数量 */
            int count = 0;

            /** 商品原价价格 */
            qint64 price = 0;

            /** 商品促销价格 */
            qint64 discountPrice = 0;

            /** 商品促销金额 */
            qint64 promotionAmount = 0;

            /** 商品单品小计金额 */
            qint64 amount = 0;

            /** 赠品 */
            bool gifts = false;

            /** 扩展字段 */
            QJsonObject extension;
        };

        /** 配置商品 */
        struct PromotionItem
        {
            /** 名称 */
            QString promotionName;

            /** 促销金额 */
            qint64 promotionAmount = 0;
        };

        enum OrderType
        {
            /** 零售 */
            RETAIL = 1,

            /** 批发 */
            WHOLESALE = 2
        };
        Q_ENUM(OrderType)


        /** UUID */
        QString uuid;

        /** 商品列表 */
        QList<WareItem> wareItems;

        /** 促销列表 */
        QList<PromotionItem> promotionItems;

        /** 订单金额*/
        qint64 orderAmount = 0;

        /** 订单促销总金额*/
        qint64 totalPromotionAmount = 0;

        /** 小票号 */
        QString receiptNo;

        /** 订单类型 */
        OrderType type = RETAIL;

        /** 订单完成时间 */
        QDateTime time;

        /** 用户账号*/
        QString userAccount;

        /** 用户名称*/
        QString userName;

        /** 订单自主配送费*/
        qint64 deliveryFeesAmount = 0;

        /** 扩展字段 */
        QJsonObject extension;

        Q_GADGET
    };
    using OrderPtr = QSharedPointer<Order>;
    using OrderPtrList = QList<QSharedPointer<Order>>;

}
Q_DECLARE_METATYPE(Core::ConfigWare)
Q_DECLARE_METATYPE(Core::ConfigWarePtr)
Q_DECLARE_METATYPE(Core::ConfigWarePtrList)
Q_DECLARE_METATYPE(Core::Order)
Q_DECLARE_METATYPE(Core::OrderPtr)
Q_DECLARE_METATYPE(Core::OrderPtrList)
