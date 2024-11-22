#include <QJsonObject>
#include <QJsonDocument>
#include "HttpServerManager.h"
#include "TerminalManager.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleProtocol/Server/Terminal/HeartbeatRouter.h"
#include "ModuleProtocol/Server/Terminal/GetConfigWaresRouter.h"
#include "ModuleProtocol/Server/Sale/HytradeInfoRouter.h"
#include "ModuleProtocol/Server/Terminal/UserLoginRouter.h"
#include "ModuleProtocol/Server/Storage/ModifyConfigWareRouter.h"
#include "ModuleProtocol/Server/Sale/QueryOrderReportRouter.h"
#include "ModuleProtocol/Server/Sale/QueryOrderStockListRouter.h"
#include "ModuleProtocol/Server/Sale/ModifyStockOrderRouter.h"

namespace Extra
{
    HttpServerManager::HttpServerManager(QObject* parent)
        : Base::AsyncService(parent)
    {
    }

    HttpServerManager::~HttpServerManager()
    {
        m_serverRouters.clear();
    }

    void HttpServerManager::StartServer()
    {
        QMetaObject::invokeMethod(this, "onStartServer");
    }

    bool HttpServerManager::OnStart()
    {
        auto heartbeat = Http::HeartbeatRouterPtr::create();

        // 注册路由
        RegistServerRouter(heartbeat);
        RegistServerRouter(Http::GetConfigWaresRouterPtr::create());
        RegistServerRouter(Http::HytradeInfoRouterPtr::create());
        RegistServerRouter(Http::UserLoginRouterPtr::create());
        RegistServerRouter(Http::ModifyConfigWareRouterPtr::create());
        RegistServerRouter(Http::QueryOrderReportRouterPtr::create());
        RegistServerRouter(Http::QueryOrderStockListRouterPtr::create());
        RegistServerRouter(Http::ModifyStockOrderRouterPtr::create());

        return true;
    }

    void HttpServerManager::OnStop()
    {
    }

    void HttpServerManager::OnAfterStart()
    {
        //QMetaObject::invokeMethod(this, "onStartServer");
    }

    void HttpServerManager::OnBeforeStop()
    {
        if(m_server.is_running())
            m_server.stop();
    }

    void HttpServerManager::RegistServerRouter(Http::RouterPtr router)
    {
        if(!m_serverRouters.contains(router))
            m_serverRouters.append(router);
    }

    void HttpServerManager::onStartServer()
    {
        if(!m_server.is_valid())
        {
            LOG_ERROR("http server has an error,start failed...");
            return;
        }
        // 读取配置文件
        if(!QFile::exists(APP_CACHE_FILE))
            WriteAppCache(APP_CACHE_FILE);
        else
            ReadAppCache(APP_CACHE_FILE);

        // 设置路由方法
        for(const auto& router : m_serverRouters)
        {
            if(router.isNull())
                continue;

            if(router->GetPolicy() == Http::RoutePolicy::ROUTE_GET)
            {
                m_server.Get(router->GetName().toStdString(),
                    static_cast<httplib::Server::Handler>(std::bind(&Http::Router::HttpServerHandler, router, std::placeholders::_1, std::placeholders::_2)));
            }
            else if(router->GetPolicy() == Http::RoutePolicy::ROUTE_POST)
            {
                m_server.Post(router->GetName().toStdString(),
                    static_cast<httplib::Server::Handler>(std::bind(&Http::Router::HttpServerHandler, router, std::placeholders::_1, std::placeholders::_2)));
            }
            else
            {
                LOG_ERROR("http server has an illegal route policy...");
            }
        }

        // 设置错误处理函数
        m_server.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
            const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
            char buf[BUFSIZ] = { 0 };
            snprintf(buf, sizeof(buf), fmt, res.status);
            res.set_content(buf, "text/html");
        });

        // 监听端口
        const auto& app = Core::GlobalData::Get()->GetConfig().app;
        m_server.listen("0.0.0.0", app.port);
    }

    bool HttpServerManager::WriteAppCache(const QString& fileName) const
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            LOG_ERROR(QString("%1 file open failed").arg(fileName).toStdString());
            return false;
        }

        const auto& app = Core::GlobalData::Get()->GetConfig().app;
        QJsonObject root;
        if(app.render == Core::Render::RENDER_SOFTWARE)
            root["render"] = "software";
        else if(app.render == Core::Render::RENDER_HARDWARE)
            root["render"] = "hardware";
        else
            Q_ASSERT_X(0, "WriteApp()", "render type is invalid");
        root["port"] = app.port;

        QJsonDocument doc;
        doc.setObject(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.flush();
        file.close();
        return true;
    }

    bool HttpServerManager::ReadAppCache(const QString& fileName) const
    {
        auto& app = Core::GlobalData::Get()->GetConfig().app;

        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            LOG_ERROR(QString("%1 file open failed").arg(fileName).toStdString());
            return false;
        }
        QByteArray data = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if(parseError.error != QJsonParseError::NoError)
        {
            LOG_ERROR(QString("%1 json parse failed").arg(fileName).toStdString());
            return false;
        }

        QJsonObject root = doc.object();
        if(root["render"].toString().compare("software", Qt::CaseInsensitive) == 0)
            app.render = Core::Render::RENDER_SOFTWARE;
        else if(root["render"].toString().compare("hardware", Qt::CaseInsensitive) == 0)
            app.render = Core::Render::RENDER_HARDWARE;
        else
            Q_ASSERT_X(0, "ReadApp()", "render type is invalid");

        app.port = root["port"].toVariant().toLongLong();

        return true;
    }
}