#pragma once
#include <algorithm>
#define CPPHTTPLIB_THREAD_POOL_COUNT (std::max(2 * std::thread::hardware_concurrency(), 30u))
#define CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH (20* 1024 *1024)
#include "httplib.h"
#include "ModuleBase/Http/Server/Router.h"
#include "ModuleBase/Service/AsyncService.h"

namespace Extra
{
    class HttpServerManager : public Base::AsyncService
    {
        Q_OBJECT

    public:
        explicit HttpServerManager(QObject* parent = nullptr);
        virtual ~HttpServerManager();

        void StartServer();

    protected:
        Q_INVOKABLE void onStartServer();

        virtual bool OnStart() override;

        virtual void OnStop() override;

        virtual void OnAfterStart() override;

        virtual void OnBeforeStop() override;

    private:
        void RegistServerRouter(Http::RouterPtr router);

        bool WriteAppCache(const QString& fileName) const;

        bool ReadAppCache(const QString& fileName) const;
    private:
        httplib::Server m_server;

        // 路由资源集合
        QList<Http::RouterPtr> m_serverRouters;
    };
}