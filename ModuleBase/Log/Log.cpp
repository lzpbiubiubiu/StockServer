#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/msvc_sink.h>
#include "Log.h"

// stock-logger
std::shared_ptr<spdlog::logger> g_stockServerLogger;

namespace Base
{
    void InitLogging(const QString& dirPath)
    {
        if(g_stockServerLogger != nullptr)
            return;

        // queue with max 32k items 1 backing thread.
        spdlog::init_thread_pool(32768, 1);
        spdlog::flush_every(std::chrono::seconds(5));

        // stock-logger 5M * 200
        QString logFile = dirPath + "Main.log";
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile.toLocal8Bit().data(), 5 * 1024 * 1024, 200));
        g_stockServerLogger = std::make_shared<spdlog::async_logger>("stock-server-logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        g_stockServerLogger->set_pattern("[%^%l%$] [stock-server] %Y-%m-%d %H:%M:%S.%e [%s:%#][%!][%t]: %v");
        g_stockServerLogger->flush_on(spdlog::level::err);

#ifdef _DEBUG
        g_stockServerLogger->set_level(spdlog::level::trace);
#else
        g_stockServerLogger->set_level(spdlog::level::info);
#endif
        spdlog::register_logger(g_stockServerLogger);

    }

    void ShutdownLogging()
    {
        spdlog::drop_all();
        spdlog::shutdown();
    }

    std::shared_ptr<spdlog::logger> GetLogger(LogSystem sys)
    {
        if(sys == LogSystem::STOCK_SERVER)
            return g_stockServerLogger;
        return nullptr;
    }
}