#pragma once

#undef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QString>
#include <spdlog/spdlog.h>
#include "ModuleBase/ModuleBaseApi.h"

/** 日志子系统 */
enum LogSystem
{
    /** STOCK_SERVER */
    STOCK_SERVER = 0,
};

#define LOG_TRACE(...)                    SPDLOG_LOGGER_TRACE(Base::GetLogger(STOCK_SERVER), __VA_ARGS__)
#define LOG_DEBUG(...)                    SPDLOG_LOGGER_DEBUG(Base::GetLogger(STOCK_SERVER), __VA_ARGS__)
#define LOG_INFO(...)                     SPDLOG_LOGGER_INFO(Base::GetLogger(STOCK_SERVER), __VA_ARGS__)
#define LOG_WARN(...)                     SPDLOG_LOGGER_WARN(Base::GetLogger(STOCK_SERVER), __VA_ARGS__)
#define LOG_ERROR(...)                    SPDLOG_LOGGER_ERROR(Base::GetLogger(STOCK_SERVER), __VA_ARGS__)
#define LOG_CRITICAL(...)                 SPDLOG_LOGGER_CRITICAL(Base::GetLogger(STOCK_SERVER), __VA_ARGS__)

namespace Base
{
    /** 开启日志 */
    void MODULE_BASE_API InitLogging(const QString& dirPath);

    /** 关闭日志 */
    void MODULE_BASE_API ShutdownLogging();

    /** 返回日志模块 */
    std::shared_ptr<spdlog::logger> MODULE_BASE_API GetLogger(LogSystem sys);
}