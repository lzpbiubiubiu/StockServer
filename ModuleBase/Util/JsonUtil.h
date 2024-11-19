#pragma once

/**
* @file         JsonUtil.h
* @brief        Json封装方法
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QJsonObject>
#include <QJsonArray>

#include "ModuleBase/ModuleBaseApi.h"

namespace Base
{
    /**
     * @brief The JsonUtil class Json操作便捷类
     */
    class MODULE_BASE_API JsonUtil
    {
    public:
        /** QByteArray转QJsonObject */
        static QJsonObject ToJsonObject(const QByteArray& byteData);

        /** 字符串转QJsonObject */
        static QJsonObject ToJsonObject(const QString& str);

        /** QByteArray转QJsonArray */
        static QJsonArray ToJsonArray(const QByteArray& byteData);

        /** 字符串转QJsonArray */
        static QJsonArray ToJsonArray(const QString& str);

        /** QJsonObject转字符串 */
        static QString ToString(const QJsonObject& jsonObj, bool indent = false);

    protected:
        JsonUtil() {};
        ~JsonUtil() {};
    };
} // namespace Base