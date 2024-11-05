#include "HeartBeatRouter.h"

namespace Http
{
    const QString HeartbeatRouter::GetName() const
    {
        return "/instore/terminal/heartbeat";
    }

    const QString HeartbeatRouter::GetDescription() const
    {
        return "heartbeat";
    }

    const RoutePolicy HeartbeatRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool HeartbeatRouter::IsCheckHeader() const
    {
        return true;
    }

    bool HeartbeatRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "timestamp"))
            {
                error = "timestamp is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void HeartbeatRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;

        qint64 timestamp = 0;
        int notUploadCloud = 0;
        int notUploadInStore = 0;
        ASSIGN_POST_PARAM_TO_INT64(object, "timestamp", timestamp);
        ASSIGN_POST_PARAM_TO_INT32(object, "cloudServerNum", notUploadCloud);
        ASSIGN_POST_PARAM_TO_INT32(object, "inStoreServerNum", notUploadInStore);

        // 赋值返回数据
        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
    }
}