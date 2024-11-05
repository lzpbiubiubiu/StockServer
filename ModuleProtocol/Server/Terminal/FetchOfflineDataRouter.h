﻿#pragma once

#include "ModuleBase/Http/Server/Router.h"
#include "ModuleProtocol/ModuleProtocolApi.h"

namespace Http
{
    /** 获取离线数据 */
    class MODULE_PROTOCOL_API FetchOfflineDataRouter : public Router
    {
    public:
        FetchOfflineDataRouter() = default;
        virtual ~FetchOfflineDataRouter() = default;

    public:
        /** 获取路由方法名称 */
        virtual const QString GetName() const override;

        /** 获取路由方法描述 */
        virtual const QString GetDescription() const override;

        /** 获取路由策略 */
        virtual const RoutePolicy GetPolicy() const override;

        /** 是否校验必须的headers */
        virtual const bool IsCheckHeader() const override;

        /** 校验POST请求的必须参数 */
        virtual bool CheckRequiredParams(const httplib::Request& req, QString& error) override;

        /** 路由处理方法 */
        virtual void OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse) override;
    };

    using FetchOfflineDataRouterPtr = QSharedPointer<FetchOfflineDataRouter>;
}