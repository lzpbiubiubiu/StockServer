#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleBase/Log/Log.h"
#include "ModifyConfigWareRouter.h"

namespace Http
{
    const QString ModifyConfigWareRouter::GetName() const
    {
        return "/stockServer/storage/modifyConfigWare";
    }

    const QString ModifyConfigWareRouter::GetDescription() const
    {
        return "modifyConfigWare";
    }

    const RoutePolicy ModifyConfigWareRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool ModifyConfigWareRouter::IsCheckHeader() const
    {
        return true;
    }

    bool ModifyConfigWareRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "operate"))
            {
                error = "operate is required";
                break;
            }
            if (!CHECK_REQUIRED_PARAM(object, "wareCode"))
            {
                error = "operate is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void ModifyConfigWareRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;
        auto db = Base::GetService<Extra::DatabaseManager>()->GetStockServerDatabase();
        Core::ConfigWarePtr configWare = Core::ConfigWarePtr::create();
        Sql::DmlBase<Sql::ConfigWareEntity> dml;
        dml.SetDatabase(db);
        Sql::ConfigWareEntityPtr entitiy = Sql::ConfigWareEntityPtr::create();
        QString sqlError;

        auto uniqueId = object["uniqueId"].toString();
        auto operate = object["operate"].toInt();
        auto code = object["wareCode"].toString();
        auto name = object["wareName"].toString();
        auto retailPrice = object["retailPrice"].toVariant().toLongLong();
        auto wholeSalePrice = object["wholeSalePrice"].toVariant().toLongLong();

        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();

        // 操作类型operate, 1:新增 2:更新
        if (1 == operate)
        {
            auto stock = object["stock"].toVariant().toLongLong();
            entitiy->SetCode(code);
            entitiy->SetName(name);
            entitiy->SetRetailPrice(retailPrice);
            entitiy->SetWholesalePrice(wholeSalePrice);
            entitiy->SetStock(stock);
            if (!dml.Add(entitiy, sqlError))
            {
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "insert config ware failed sql error" + sqlError;
                reponse->displayMsg = QStringLiteral("插入商品Sql执行失败");
                reponse->traceId = header.traceId;
                return;
            }
        }
        else if (2 == operate)
        {
            entitiy->SetCode(code);
            if (!dml.FindById(entitiy, sqlError))
            {
                reponse->code = HTTP_SERVER_SELECT_CONFIG_WARE_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_CONFIG_WARE_ERROR;
                reponse->msg = "select config ware failed sql error" + sqlError;
                reponse->displayMsg = QStringLiteral("没有此商品，无法更新");
                reponse->traceId = header.traceId;
                return;
            }
            entitiy->SetName(name);
            entitiy->SetRetailPrice(retailPrice);
            entitiy->SetWholesalePrice(wholeSalePrice);
            if (!dml.UpdateById(entitiy, QStringList(), sqlError))
            {
                reponse->code = HTTP_SERVER_SQL_ERROR;
                reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SQL_ERROR;
                reponse->msg = "update config ware failed sql error" + sqlError;
                reponse->displayMsg = QStringLiteral("更新商品Sql执行失败");
                reponse->traceId = header.traceId;
                return;
            }
        }

        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
    }
}