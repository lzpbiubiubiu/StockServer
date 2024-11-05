#include "QueryOrderReportRouter.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleBase/Log/Log.h"
namespace Http
{
    const QString QueryOrderReportRouter::GetName() const
    {
        return "/stockServer/sale/queryOrderReport";
    }

    const QString QueryOrderReportRouter::GetDescription() const
    {
        return "queryOrderReport";
    }

    const RoutePolicy QueryOrderReportRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool QueryOrderReportRouter::IsCheckHeader() const
    {
        return true;
    }

    bool QueryOrderReportRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "date"))
            {
                error = "date is required";
                break;
            }
            if (!CHECK_REQUIRED_PARAM(object, "queryRange"))
            {
                error = "queryRange is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void QueryOrderReportRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;
        auto uniqueId = object["uniqueId"].toString();
        auto dateTime = object["date"].toVariant().toLongLong();
        auto queryRange = object["queryRange"].toInt();
        auto queryDate = QDateTime::fromMSecsSinceEpoch(dateTime).date();
        qint64 dateRange = 29;

        QJsonArray orderArray;
        for (int i = dateRange; i >= 0; i--)
        {
            auto date = queryDate.addDays(-i);
            auto db = Base::GetService<Extra::DatabaseManager>()->GetBusinessDatabase(date);
            Sql::DmlBase<Sql::OrderEntity> dml;
            dml.SetDatabase(db);
            QString sqlError;
            Sql::OrderEntityPtrList orders;
            qint64 totalAmount = 0;
            if (dml.ListAll(orders, sqlError))
            {
                for (auto item : orders)
                {
                    totalAmount += item->GetOrderAmount();
                }
                QJsonObject obj;
                obj["date"] = date.toString("yyyy-MM-dd");
                obj["totalAmount"] = totalAmount;
                orderArray.append(obj);
            }
            else
            {
                LOG_ERROR("QueryOrderReportRouter", "query order error: %s", sqlError.toStdString().c_str());
            }
        }
        if (!orderArray.isEmpty())
        {
            // 赋值返回数据
            reponse = HttpServerResponsePtr::create();
            reponse->code = HTTP_SERVER_SUCCESS;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
            reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
            reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
            reponse->traceId = header.traceId;
            reponse->data["uniqueId"] = uniqueId;
            reponse->data["orderReports"] = orderArray;
        }
        else
        {
            reponse = HttpServerResponsePtr::create();
            reponse->code = HTTP_SERVER_SELECT_ORDER_REPORT_ERROR;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_ORDER_REPORT_ERROR;
            reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SELECT_ORDER_REPORT_ERROR];
            reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SELECT_ORDER_REPORT_ERROR];
            reponse->traceId = header.traceId;
            reponse->data["uniqueId"] = uniqueId;
        }
    }
}