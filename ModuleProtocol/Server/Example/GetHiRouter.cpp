#include "GetHiRouter.h"

namespace Http
{
    GetHiRouter::GetHiRouter()
    {
    }

    GetHiRouter::~GetHiRouter()
    {
    }

    const QString GetHiRouter::GetName() const
    {
        return "/instore/example/hi";
    }

    const QString GetHiRouter::GetDescription() const
    {
        return "hi";
    }

    const RoutePolicy GetHiRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_GET;
    }

    const bool GetHiRouter::IsCheckHeader() const
    {
        return false;
    }

    bool GetHiRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        return true;
    }

    void GetHiRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
    }
}