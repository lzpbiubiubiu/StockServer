#include "UserLoginRouter.h"
#include "ModuleData/Core/LoginData.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Entity/UserLoginEntity.h"
#include "ModuleBase/Log/Log.h"

namespace Http
{
    const QString UserLoginRouter::GetName() const
    {
        return "/stockServer/terminal/userLogin";
    }

    const QString UserLoginRouter::GetDescription() const
    {
        return "userLogin";
    }

    const RoutePolicy UserLoginRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool UserLoginRouter::IsCheckHeader() const
    {
        return true;
    }

    bool UserLoginRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "account"))
            {
                error = "account is required";
                break;
            }
            if(!CHECK_REQUIRED_PARAM(object, "password"))
            {
                error = "password is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void UserLoginRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;
        QString account = object["account"].toString();
        QString password = object["password"].toString();

        auto db = Base::GetService<Extra::DatabaseManager>()->GetStockServerDatabase();
        Sql::DmlBase<Sql::UserLoginEntity> dml;
        dml.SetDatabase(db);
        QString sqlError;

        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();

        Sql::UserLoginEntityPtr entity = Sql::UserLoginEntityPtr::create();
        entity->SetAccount(account);
        bool result = dml.FindById(entity, sqlError);
        if (!result)
        {
            LOG_ERROR("select user login failed sql error: {}", sqlError.toStdString());
            reponse->code = HTTP_SERVER_SELECT_USER_LOGIN_ERROR;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_USER_LOGIN_ERROR;
            reponse->msg = "select user login failed";
            reponse->displayMsg = QStringLiteral("没有此用户");
            reponse->traceId = header.traceId;
            return;
        }

        if (entity->GetPassword() != password)
        {
            reponse->code = HTTP_SERVER_SELECT_USER_LOGIN_ERROR;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SELECT_USER_LOGIN_ERROR;
            reponse->msg = "password error";
            reponse->displayMsg = QStringLiteral("密码错误");
            reponse->traceId = header.traceId;
            return;
        }

        QJsonObject loginObject;
        loginObject["account"] = account;
        loginObject["name"] = entity->GetName();
        loginObject["password"] = password;
        loginObject["tel"] = entity->GetTel();
        loginObject["status"] = entity->GetStatus();

        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
        reponse->data["userInfo"] = loginObject;
    }

   
}