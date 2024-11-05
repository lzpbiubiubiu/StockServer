#include <QJsonObject>
#include <QDateTime>
#include "Router.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleBase/Util/JsonUtil.h"

namespace Http
{
    thread_local RouterThreadLocal g_threadLocal;

    void Router::HttpServerHandler(const httplib::Request& req, httplib::Response& res)
    {
        // 请求日志
        QString traceId = QString::fromStdString(req.get_header_value("traceId"));
        g_threadLocal.uuid = traceId.isEmpty() ? Base::MathUtil::GetUUID() : traceId;
        PrintRequestLog(req);

        g_threadLocal.start = QDateTime::currentMSecsSinceEpoch();

        // 检查必须的请求头与路由方法处理
        HttpServerResponsePtr response = nullptr;
        do
        {
            QString error;
            // 校验请求头
            if(IsCheckHeader() && !CheckRequirdHeaders(req, error))
            {
                OnCheckHeadersFailedHandler(error, response);
                break;
            }

            // 解析请求头
            HttpServerHeader header;
            AssignHeader(req, header);

            if(req.method == "POST")
                PreprocessPostBody(req);

            // 校验请求参数
            if(!CheckRequiredParams(req, error))
            {
                OnCheckParamsFailedHandler(error, response);
                break;
            }

            // 路由处理
            OnRouteHandler(req, res, header, response);
        } while(0);

        // 组装response数据
        QString responseData = PackageHttpServerResponse(response);

        // 设置响应
        if(!responseData.isEmpty())
        {
            res.status = 200;
            res.set_content(responseData.toStdString(), "application/json");
        }

        // 打印日志
        PrintResponseLog(req, res, responseData);
    }

    void Router::PreprocessPostBody(const httplib::Request& req)
    {
        // post参数,去除前缀"param=",url解码,转为json对象
        QByteArray ba = QByteArray::fromPercentEncoding(req.body.substr(6).c_str());
        g_threadLocal.object = Base::JsonUtil::ToJsonObject(ba);
    }

    const RouterThreadLocal& Router::GetThreadLocal() const
    {
        return g_threadLocal;
    }

    bool Router::CheckRequirdHeaders(const httplib::Request& req, QString& error)
    {
        // 校验header
        auto CheckHeader = [&](const QString& name) {
            return req.has_header(name.toStdString());
        };

        do
        {
            if(!CheckHeader("traceId"))
            {
                error = "traceId is required";
                break;
            }
            if(!CheckHeader("deviceType") && "Stock App" == req.get_header_value("deviceType"))
            {
                error = "deviceType is required";
                break;
            }
            if(!CheckHeader("version"))
            {
                error = "version is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void Router::OnCheckHeadersFailedHandler(const QString& error, HttpServerResponsePtr& response)
    {
        response = HttpServerResponsePtr::create();
        response->code = HTTP_SERVER_REQUIRD_HEADER_ERROR;
        response->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_REQUIRD_HEADER_ERROR;
        response->msg = error;
        response->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_REQUIRD_HEADER_ERROR];
        response->traceId = g_threadLocal.uuid;
        response->success = false;
    }

    void Router::OnCheckParamsFailedHandler(const QString& error, HttpServerResponsePtr& response)
    {
        response = HttpServerResponsePtr::create();
        response->code = HTTP_SERVER_REQUIRD_PARAM_ERROR;
        response->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_REQUIRD_PARAM_ERROR;
        response->msg = error;
        response->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_REQUIRD_PARAM_ERROR];
        response->traceId = g_threadLocal.uuid;
        response->success = false;
    }

    void Router::AssignHeader(const httplib::Request& req, HttpServerHeader& header)
    {
        auto GetHeader = [&](const QString& name) {
            return QString::fromStdString(req.get_header_value(name.toStdString()));
            };

        header.traceId = GetHeader("traceId");
        header.deviceType = GetHeader("deviceType");
    }

    void Router::PrintRequestLog(const httplib::Request& req)
    {
        // Headers
        QJsonObject root;
        for(const auto& header : req.headers)
            root[QString::fromStdString(header.first)] = QString::fromStdString(header.second);

        if(req.method == "GET")
        {
            std::ostringstream oss;
            for(const auto& param : req.params)
            {
                if(!oss.str().empty())
                    oss << "&";
                oss << param.first << "=" << param.second;
            }

            LOG_INFO("HTTP-SERVER-{} [{}] Request: http://{}:{}{}{} Headers: {}",
                req.method, g_threadLocal.uuid.toStdString(), req.local_addr, req.local_port, req.path,
                oss.str().empty() ? "" : "?" + oss.str(), Base::JsonUtil::ToString(root).toStdString());
        }
        else if(req.method == "POST")
        {
            LOG_INFO("HTTP-SERVER-{} [{}] Request: http://{}:{}{}?{} Headers: {}",
                req.method, g_threadLocal.uuid.toStdString(), req.local_addr, req.local_port, req.path,
                QByteArray::fromPercentEncoding(req.body.c_str()).toStdString(), Base::JsonUtil::ToString(root).toStdString());
        }
    }

    void Router::PrintResponseLog(const httplib::Request& req, httplib::Response& res, const QString& response)
    {
        auto duration = QDateTime::currentMSecsSinceEpoch() - g_threadLocal.start;
        LOG_INFO("HTTP-SERVER-{} [{}] Response: Time:{} ms Status:{} Content: {}", req.method, g_threadLocal.uuid.toStdString(), std::to_string(duration), res.status, res.body);
    }

    QString Router::PackageHttpServerResponse(HttpServerResponsePtr response)
    {
        if(response == nullptr)
        {
            LOG_INFO("no custom response data");
            return QString();
        }

        QJsonObject obj;
        obj["code"] = response->code;
        obj["displayCode"] = response->displayCode;
        obj["msg"] = response->msg;
        obj["displayMsg"] = response->displayMsg;
        obj["traceId"] = response->traceId;
        obj["data"] = response->data;
        obj["success"] = response->success;

        return Base::JsonUtil::ToString(obj);
    }
}