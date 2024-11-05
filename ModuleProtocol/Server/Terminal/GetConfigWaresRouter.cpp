#include "GetConfigWaresRouter.h"
#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/GlobalStructData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleBase/Log/Log.h"

namespace Http
{
    const QString GetConfigWaresRouter::GetName() const
    {
        return "/stockServer/terminal/getConfigWares";
    }

    const QString GetConfigWaresRouter::GetDescription() const
    {
        return "getConfigWares";
    }

    const RoutePolicy GetConfigWaresRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool GetConfigWaresRouter::IsCheckHeader() const
    {
        return true;
    }

    bool GetConfigWaresRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        return true;
    }

    void GetConfigWaresRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;

        auto db = Base::GetService<Extra::DatabaseManager>()->GetStockServerDatabase();

        Sql::DmlBase<Sql::ConfigWareEntity> dml;
        dml.SetDatabase(db);
        Sql::ConfigWareEntityPtrList entities;
        QString sqlError;

        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();

        if (!dml.ListAll(entities, sqlError))
        {
            Q_ASSERT_X(0, "", sqlError.toUtf8());
            LOG_ERROR("select config wares failed sql error: {}", sqlError.toStdString());
            reponse->code = HTTP_SERVER_SELECT_CONFIG_WARES_ERROR;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_CONFIG_WARES_ERROR;
            reponse->msg = "select config wares failed";
            reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SELECT_CONFIG_WARES_ERROR];
            reponse->traceId = header.traceId;
            return;
        }

        Core::ConfigWarePtrList configWares;
        configWares = Core::ConvertUtil::FromEntityList(entities);
        QJsonArray configWaresJsonArray;
        for (const auto& item : configWares)
        {
            QJsonObject configWareObj;
            configWareObj["name"] = item->name;
            configWareObj["code"] = item->code;
            configWareObj["retailPrice"] = item->retailPrice;
            configWareObj["wholesalePrice"] = item->wholesalePrice;
            configWareObj["stock"] = item->stock;
            configWareObj["extension"] = item->extension;
            configWaresJsonArray.append(configWareObj);
        }
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
        reponse->data["configWares"] = configWaresJsonArray;
    }
}