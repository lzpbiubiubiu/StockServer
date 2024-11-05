#pragma once

#include <QMap>
#include <QString>
#include <QObject>
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleData/Core/GlobalStructData.h"

/** 程序配置文件 */
#define APP_CACHE_FILE (Base::PathUtil::GetDataDir() + "App.json")

/** 版本号文件 */
#define VERSION_FILE (Base::PathUtil::GetApplicationDir() + "Version.json")

/** 数据库名称 */
#define BUSINESS_DATABASE_NAME ("Bussiness")

/** 配置数据库名称 */
#define STOCK_SERVER_DATABASE_NAME ("StockServer")

namespace Core
{
    /** 渲染器 */
    enum Render
    {
        /** 软件渲染 */
        RENDER_SOFTWARE = 0,

        /** 硬件渲染 */
        RENDER_HARDWARE
    };

    struct CommonConfig
    {
        /** 版本号 */
        struct Version
        {
            /** 版本号 */
            QString string = "1.0.0.0";

            /** 版本代码 */
            QString code = "1000";

            /** 版本发布时间 */
            QString date;

            /** 默认发布环境 [DEV,UAT,PRO] */
            QString env = "DEV";
        } version;

        /** 程序配置 */
        struct App
        {
            /** 渲染器 */
            Render render = Render::RENDER_HARDWARE;

            /** 主屏语言 */
            QString lang;

            /** ip */
            QString ip;

            /** 端口 */
            int port = 9090;
        } app;
    
        /** 退出触发行为 */
        struct Exit
        {
            /** 重启 */
            bool restart = false;
        } exit;

        /** 商品配置 */
        Core::ConfigWarePtrList configWares;
    };
}
Q_DECLARE_METATYPE(Core::CommonConfig)