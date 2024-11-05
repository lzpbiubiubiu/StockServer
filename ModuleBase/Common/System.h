#pragma once
#include <QDateTime>
#include <QString>

/**
* @file         System.h
* @brief        系统相关函数
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "ModuleBase/ModuleBaseApi.h"

namespace Base
{
    /** 系统类，提供系统运行环境相关的接口 */
    class MODULE_BASE_API System
    {
    public:
        /** 获取当前本地时间 */
        static QDateTime GetCurrentDateTime();

        /** 获取当前UTC时间（毫秒）*/
        static qint64 GetCurrentTimeMillis();

        /** 获取物理网卡MAC地址 */
        static QString GetMAC();

        /** 启动PosServer */
        static void StartStockServer();

        /** 获取系统支持OpenGL版本号 */
        static bool GetOpenGLVersion(int& major, int& minor);

        /** 是否开启硬件加速 */
        static bool IsHardwareRenderEnabled();

        /** 获取屏幕缩放比 */
        static double GetScreenScaleFactor();

    protected:
        System() {};

    private:
        // MAC地址缓存
        static QString s_mac;
    };
}// namespace Base