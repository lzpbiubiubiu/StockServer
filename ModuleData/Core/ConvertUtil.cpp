#include "ModuleBase/Util/JsonUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "ConvertUtil.h"
#include "ModuleBase/Common/System.h"
#include <QLocale>

namespace Core
{
    Sql::ConfigWareEntityPtr ConvertUtil::ToEntity(Core::ConfigWarePtr ware)
    {
        auto entity = Sql::ConfigWareEntityPtr::create();
        entity->SetCode(ware->code);
        entity->SetName(ware->name);
        entity->SetRetailPrice(ware->retailPrice);
        entity->SetWholesalePrice(ware->wholesalePrice);
        entity->SetStock(ware->stock);
        entity->SetImageFileName(ware->imageFileName);
        entity->SetImageMd5(ware->imageMd5);
        entity->SetExtension(QString(QJsonDocument(ware->extension).toJson(QJsonDocument::Compact)));
        return entity;
    }
    Sql::ConfigWareEntityPtrList ConvertUtil::ToEntityList(const Core::ConfigWarePtrList& orders)
    {
        Sql::ConfigWareEntityPtrList list;
        for (auto i : orders)
            list << ToEntity(i);
        return list;
    }
    Core::ConfigWarePtr ConvertUtil::FromEntity(Sql::ConfigWareEntityPtr entity)
    {
        auto item = Core::ConfigWarePtr::create();
        item->code = entity->GetCode();
        item->name = entity->GetName();
        item->retailPrice = entity->GetRetailPrice();
        item->wholesalePrice = entity->GetWholesalePrice();
        item->stock = entity->GetStock();
        item->imageFileName = entity->GetImageFileName();
        item->imageMd5 = entity->GetImageMd5();
        item->extension = Base::JsonUtil::ToJsonObject(entity->GetExtension());
        return item;
    }

    Core::ConfigWarePtrList ConvertUtil::FromEntityList(const Sql::ConfigWareEntityPtrList& entities)
    {
        Core::ConfigWarePtrList list;
        for (auto i : entities)
            list << FromEntity(i);
        return list;
    }

    Sql::OrderEntityPtr ConvertUtil::ToEntity(Core::OrderPtr order)
    {
        auto entity = Sql::OrderEntityPtr::create();
        entity->SetUuid(order->uuid);
        entity->SetReceiptNo(order->receiptNo);
        entity->SetOrderAmount(order->orderAmount);
        entity->SetTotalPromotionAmount(order->totalPromotionAmount);
        entity->SetDeliveryFeesAmount(order->deliveryFeesAmount);
        entity->SetUserAccount(order->userAccount);
        entity->SetUserName(order->userName);
        entity->SetTime(order->time);
        QMetaEnum metaEnum = QMetaEnum::fromType<Core::Order::OrderType>();
        QString type = metaEnum.valueToKey(order->type);
        Q_ASSERT(!type.isEmpty());
        entity->SetType(type);

        QJsonArray wareArr;
        for (const auto& item : order->wareItems)
        {
            QJsonObject wareObj;
            wareObj["name"] = item.name;
            wareObj["code"] = item.code;
            wareObj["count"] = item.count;
            wareObj["price"] = item.price;
            wareObj["discountPrice"] = item.discountPrice;
            wareObj["promotionAmount"] = item.promotionAmount;
            wareObj["amount"] = item.amount;
            wareObj["gifts"] = item.gifts;
            wareObj["extension"] = item.extension;
            wareArr << wareObj;
        }
        entity->SetWares(QString(QJsonDocument(wareArr).toJson(QJsonDocument::Compact)));

        QJsonArray promoArr;
        for (const auto& item : order->promotionItems)
        {
            QJsonObject promoObj;
            promoObj["promotionName"] = item.promotionName;
            promoObj["promotionAmount"] = item.promotionAmount;
            promoArr << promoObj;
        } 
        entity->SetPromotions(QString(QJsonDocument(promoArr).toJson(QJsonDocument::Compact)));

        entity->SetExtension(QString(QJsonDocument(order->extension).toJson(QJsonDocument::Compact)));
        return entity;
    }

    Sql::OrderEntityPtrList ConvertUtil::ToEntityList(const Core::OrderPtrList& orders)
    {
        Sql::OrderEntityPtrList list;
        for (auto i : orders)
            list << ToEntity(i);
        return list;
    }

    Core::OrderPtr ConvertUtil::FromEntity(Sql::OrderEntityPtr entity)
    {
        auto order = Core::OrderPtr::create();
        order->uuid = entity->GetUuid();
        order->receiptNo = entity->GetReceiptNo();
        order->orderAmount = entity->GetOrderAmount();
        order->totalPromotionAmount = entity->GetTotalPromotionAmount();
        order->deliveryFeesAmount = entity->GetDeliveryFeesAmount();
        order->userAccount = entity->GetUserAccount();
        order->userName = entity->GetUserName();
        order->time = entity->GetTime();

        QMetaEnum metaEnum = QMetaEnum::fromType<Core::Order::OrderType>();
        int type = metaEnum.keyToValue(entity->GetType().toUtf8());
        Q_ASSERT(type != -1);
        order->type = static_cast<Core::Order::OrderType>(type);

        QJsonParseError error;
        QJsonDocument extension = QJsonDocument::fromJson(entity->GetExtension().toUtf8(), &error);
        if (error.error == QJsonParseError::NoError)
        {
            if (extension.isObject())
            {
                order->extension = extension.object();
            }
        }

        QJsonDocument promotions = QJsonDocument::fromJson(entity->GetPromotions().toUtf8(), &error);
        if (error.error == QJsonParseError::NoError)
        {
            if (promotions.isArray())
            {
                order->promotionItems.clear();
                for (auto item : promotions.array())
                {
                    Core::Order::PromotionItem promoItem;
                    QJsonObject promoObj = item.toObject();
                    promoItem.promotionName = promoObj["promotionName"].toString();
                    promoItem.promotionAmount = promoObj["promotionAmount"].toVariant().toLongLong();
                    order->promotionItems << promoItem;
                }
            }
        }

        QJsonDocument wares = QJsonDocument::fromJson(entity->GetWares().toUtf8(), &error);
        if (error.error == QJsonParseError::NoError)
        {
            if (wares.isArray())
            {
                order->wareItems.clear();
                for (auto item : wares.array())
                {
                    Core::Order::WareItem wareItem;
                    QJsonObject wareObj = item.toObject();
                    wareItem.name = wareObj["name"].toString();
                    wareItem.code = wareObj["code"].toString();
                    wareItem.count = wareObj["count"].toInt();
                    wareItem.price = wareObj["price"].toVariant().toLongLong();
                    wareItem.discountPrice = wareObj["discountPrice"].toVariant().toLongLong();
                    wareItem.promotionAmount = wareObj["promotionAmount"].toVariant().toLongLong();
                    wareItem.amount = wareObj["amount"].toVariant().toLongLong();
                    QJsonParseError error;
                    QJsonDocument wareExtension = QJsonDocument::fromJson(wareObj["extension"].toVariant().toByteArray(), &error);
                    if (error.error == QJsonParseError::NoError)
                    {
                        if (wareExtension.isObject())
                        {
                            wareItem.extension = wareExtension.object();
                        }
                    }
                    order->wareItems << wareItem;
                }
            }
        }
        return order;
    }

    Core::OrderPtrList ConvertUtil::FromEntityList(const Sql::OrderEntityPtrList& entities)
    {
        Core::OrderPtrList list;
        for (auto i : entities)
            list << FromEntity(i);
        return list;
    }
}