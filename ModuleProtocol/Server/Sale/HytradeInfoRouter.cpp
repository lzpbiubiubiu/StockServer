#include "HytradeInfoRouter.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleBase/Log/Log.h"
namespace Http
{
    const QString HytradeInfoRouter::GetName() const
    {
        return "/stockServer/sale/hytradeInfo";
    }

    const QString HytradeInfoRouter::GetDescription() const
    {
        return "hytradeInfo";
    }

    const RoutePolicy HytradeInfoRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool HytradeInfoRouter::IsCheckHeader() const
    {
        return true;
    }

    bool HytradeInfoRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "wares"))
            {
                error = "wares is required";
                break;
            }
            if (!CHECK_REQUIRED_PARAM(object, "receiptNo"))
            {
                error = "receiptNo is required";
                break;
            }
            if (!CHECK_REQUIRED_PARAM(object, "orderAmount"))
            {
                error = "orderAmount is required";
                break;
            }
            if (!CHECK_REQUIRED_PARAM(object, "userAccount"))
            {
                error = "userAccount is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void HytradeInfoRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;
        auto uniqueId = object["uniqueId"].toString();
        auto wareArray = object["wares"].toArray();
        if (wareArray.isEmpty())
        {
            reponse->code = HTTP_SERVER_HAS_TRADE_ERROR_WARE;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_HAS_TRADE_ERROR_WARE;
            reponse->msg = "wares is null";
            reponse->displayMsg = QStringLiteral("入库商品列表为空");
            reponse->traceId = header.traceId;
            reponse->data["uniqueId"] = uniqueId;
            return;
        }

        auto db = Base::GetService<Extra::DatabaseManager>()->GetStockServerDatabase();
        Sql::DmlBase<Sql::ConfigWareEntity> dml;
        dml.SetDatabase(db);
        QString sqlError;

        Core::OrderPtr order = Core::OrderPtr::create();
        order->receiptNo = object["receiptNo"].toString();
        order->orderAmount = object["orderAmount"].toVariant().toLongLong();
        order->userAccount = object["userAccount"].toString();
        order->userName = object["userName"].toString();
        order->deliveryFeesAmount = object["deliveryFeesAmount"].toVariant().toLongLong();
        order->totalPromotionAmount = object["totalPromotionAmount"].toVariant().toLongLong();
        order->uuid = Base::MathUtil::GetUUID();
        order->type = static_cast<Core::Order::OrderType>(object["orderType"].toInt());
        order->time = QDateTime::currentDateTime();
        order->extension = object["extension"].toObject();

        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();

        QMap<QString, Sql::ConfigWareEntityPtr> wareEntityMap;
        QJsonArray tradeErrorWareArr;
        
        for (auto item : wareArray)
        {
            QJsonObject wareObj = item.toObject();
            auto wareCode = wareObj["code"].toString();
            int saleCount = wareObj["count"].toInt();
            Sql::ConfigWareEntityPtr wareEntity = nullptr;
            int wareStock = 0;
            if (wareEntityMap.contains(wareCode))
            {
                wareEntity = wareEntityMap[wareCode];
                wareStock = wareEntity->GetStock();
                if (wareStock < saleCount)
                {
                    LOG_ERROR("select config wares stock failed sql error: {}", sqlError.toStdString());
                    reponse->code = HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR;
                    reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR;
                    reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR];;
                    reponse->displayMsg = QStringLiteral("商品：%1同时存在卖品和赠品，库存不足").arg(wareCode);
                    reponse->traceId = header.traceId;
                    reponse->data["uniqueId"] = uniqueId;
                    return;
                }
            }
            else
            {
                wareEntity = Sql::ConfigWareEntityPtr::create();
                wareEntity->SetCode(wareCode);
                bool result = dml.FindById(wareEntity, sqlError);
                if (!result)
                {
                    LOG_ERROR("select config wares stock failed sql error: {}", sqlError.toStdString());
                    reponse->code = HTTP_SERVER_SQL_ERROR;
                    reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                    reponse->msg = "update config wares stock failed sql error:" + sqlError;
                    reponse->displayMsg = QStringLiteral("查询商品：%1Sql执行失败").arg(wareCode);
                    reponse->traceId = header.traceId;
                    reponse->data["uniqueId"] = uniqueId;
                    return;
                }

                wareStock = wareEntity->GetStock();
                if (wareStock < saleCount)
                {
                    wareObj["stock"] = wareStock;
                    tradeErrorWareArr << wareObj;
                    continue;
                }
            }
            wareEntity->SetStock(wareStock - saleCount);
            wareEntityMap[wareCode] = wareEntity;

            Core::Order::WareItem wareItem;
            wareItem.name = wareObj["name"].toString();
            wareItem.code = wareCode;
            wareItem.count = saleCount;
            wareItem.price = wareObj["price"].toVariant().toLongLong();
            wareItem.discountPrice = wareObj["discountPrice"].toVariant().toLongLong();
            wareItem.promotionAmount = wareObj["promotionAmount"].toVariant().toLongLong();
            wareItem.amount = wareObj["amount"].toVariant().toLongLong();
            wareItem.gifts = wareObj["gifts"].toBool();
            wareItem.extension = wareObj["extension"].toObject();
            order->wareItems << wareItem;

        }
        
        auto promoArray = object["promotions"].toArray();
        for (auto item : promoArray)
        {
            QJsonObject promoObj = item.toObject();
            Core::Order::PromotionItem promoItem;
            promoItem.promotionName = promoObj["promotionName"].toString();
            promoItem.promotionAmount = promoObj["promotionAmount"].toVariant().toLongLong();
            order->promotionItems << promoItem;
        }

        // 不存在异常商品
        if (tradeErrorWareArr.isEmpty())
        {
            Sql::ConfigWareEntityPtrList entities = wareEntityMap.values();
            if (!dml.UpdateList(entities, sqlError))
            {
                LOG_ERROR("update config wares stock failed sql error: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "update config wares stock failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("商品库存扣减Sql执行失败");
                reponse->traceId = header.traceId;
                return;
            }

            auto bussinessDb = Base::GetService<Extra::DatabaseManager>()->GetTempDatabase();
            Sql::DmlBase<Sql::OrderEntity> dml;
            dml.SetDatabase(bussinessDb);

            auto orderEntity = Core::ConvertUtil::ToEntity(order);
            if (!dml.Add(orderEntity, sqlError))
            {
                LOG_ERROR("insert order failed sql error: {}", sqlError.toStdString());
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "insert order failed sql error:" + sqlError;
                reponse->displayMsg = QStringLiteral("订单入库Sql执行失败");
                reponse->traceId = header.traceId;
                return;
            }
            reponse->code = HTTP_SERVER_SUCCESS;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
            reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
            reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
            reponse->traceId = header.traceId;
            reponse->data["uniqueId"] = uniqueId;
        }
        else
        {
            reponse->code = HTTP_SERVER_HAS_TRADE_ERROR_WARE;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_HAS_TRADE_ERROR_WARE;
            reponse->msg = "has trade error ware";
            reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_HAS_TRADE_ERROR_WARE];
            reponse->traceId = header.traceId;
            reponse->data["tradeErrorWares"] = tradeErrorWareArr;
            reponse->data["uniqueId"] = uniqueId;
        }
        
    }
}