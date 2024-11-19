#include "QueryOrderStockListRouter.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleBase/Log/Log.h"
namespace Http
{
    const QString QueryOrderStockListRouter::GetName() const
    {
        return "/stockServer/sale/queryOrderStockList";
    }

    const QString QueryOrderStockListRouter::GetDescription() const
    {
        return "queryOrderStockList";
    }

    const RoutePolicy QueryOrderStockListRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool QueryOrderStockListRouter::IsCheckHeader() const
    {
        return true;
    }

    bool QueryOrderStockListRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        return true;
    }

    void QueryOrderStockListRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;
        auto uniqueId = object["uniqueId"].toString();
        QJsonArray orderArray;
        auto db = Base::GetService<Extra::DatabaseManager>()->GetTempDatabase();
        Sql::DmlBase<Sql::OrderEntity> dml;
        dml.SetDatabase(db);
        QString sqlError;
        Sql::OrderEntityPtrList orders;

        bool result = dml.ListAll(orders, sqlError);
        if(!result)
        {
            LOG_ERROR("select orders stock list failed sql error: {}", sqlError.toStdString());
            reponse->code = HTTP_SERVER_SQL_ERROR;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
            reponse->msg = "update config wares stock failed sql error:" + sqlError;
            reponse->displayMsg = QStringLiteral("查询备货清单失败Sql执行失败");
            reponse->traceId = header.traceId;
            reponse->data["uniqueId"] = uniqueId;
            return;
        }

        for(auto item : orders)
        {
            QJsonObject order;
            order["orderNo"] = item->GetReceiptNo();
            order["orderUuid"] = item->GetUuid();
            order["orderAmount"] = item->GetOrderAmount();
            order["totalPromotionAmount"] = item->GetTotalPromotionAmount();
            order["deliveryFreeAmount"] = item->GetDeliveryFeesAmount();
            order["extension"] = item->GetExtension();
            order["time"] = item->GetTime().toString("yyyy-MM-dd HH:mm:ss");
            order["wares"] = Base::JsonUtil::ToJsonArray(item->GetWares());
            orderArray.append(order);
        }
        
        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
        reponse->data["uniqueId"] = uniqueId;
        reponse->data["stockingOrders"] = orderArray;
    }
}