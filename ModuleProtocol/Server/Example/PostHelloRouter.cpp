#include "PostHelloRouter.h"

namespace Http
{
    PostHelloRouter::PostHelloRouter()
    {
    }

    PostHelloRouter::~PostHelloRouter()
    {
    }

    const QString PostHelloRouter::GetName() const
    {
        return "/instore/example/hello";
    }

    const QString PostHelloRouter::GetDescription() const
    {
        return "hello";
    }

    const RoutePolicy PostHelloRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool PostHelloRouter::IsCheckHeader() const
    {
        return true;
    }

    bool PostHelloRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        return true;
    }

    void PostHelloRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
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