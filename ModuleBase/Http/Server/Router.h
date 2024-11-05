#pragma once

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include "httplib.h"
#include "Consts.h"
#include "ModuleBase/Util/JsonUtil.h"
#include "ModuleBase/ModuleBaseApi.h"

namespace Http
{
    /** HTTP路由策略 */
    enum class RoutePolicy
    {
        /** GET */
        ROUTE_GET,

        /** POST */
        ROUTE_POST,
    };

    struct RouterThreadLocal
    {
        /** 请求唯一标识 */
        QString uuid;

        /** 开始时间 */
        qint64 start;

        /** post数据 */
        QJsonObject object;
    };

    class MODULE_BASE_API Router : public QObject
    {
        Q_OBJECT
    public:
        Router() = default;
        virtual ~Router() = default;

    public:
        /** Http server处理方法 */
        void HttpServerHandler(const httplib::Request& req, httplib::Response& res);

        /** 获取路由方法名称 */
        virtual const QString GetName() const = 0;

        /** 获取路由方法描述 */
        virtual const QString GetDescription() const = 0;

        /** 获取路由策略 */
        virtual const RoutePolicy GetPolicy() const = 0;

        /** 是否校验必须的headers (download不需要校验header，其他都需要 )*/
        virtual const bool IsCheckHeader() const = 0;

        /** 校验POST请求的必须参数 */
        virtual bool CheckRequiredParams(const httplib::Request& req, QString& error) = 0;

        /** HTTP路由处理方法响应 */
        virtual void OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse) = 0;

    protected:
        /** 获取本地线程存储数据 */
        const RouterThreadLocal& GetThreadLocal() const;

    private:
        /** 校验请求头必要参数 */
        bool CheckRequirdHeaders(const httplib::Request& req, QString& error);

        /** 校验请求必要参数失败处理方法 */
        void OnCheckHeadersFailedHandler(const QString& error, HttpServerResponsePtr& response);
        void OnCheckParamsFailedHandler(const QString& error, HttpServerResponsePtr& response);

        /** 分派请求头 */
        void AssignHeader(const httplib::Request& req, HttpServerHeader& header);

        /** 打印请求日志 */
        void PrintRequestLog(const httplib::Request& req);

        /** 打印返回日志 */
        void PrintResponseLog(const httplib::Request& req, httplib::Response& res, const QString& response);

        /** 组装返回数据 */
        QString PackageHttpServerResponse(HttpServerResponsePtr response);

        /** 预处理POST参数 */
        void PreprocessPostBody(const httplib::Request& req);
    };

    using RouterPtr = QSharedPointer<Router>;
}