#pragma once
#include <QMap>
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>

#define CHECK_REQUIRED_PARAM(object, name)                             \
    (object.contains(name) && !object[name].isNull())                  \

#define ASSIGN_POST_PARAM_TO_STRING(object, name, var)                 \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(QString));                      \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toString();                             \
    } while(0)

#define ASSIGN_POST_PARAM_TO_INT32(object, name, var)                  \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(int));                          \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toInt();                                \
    } while(0)

#define ASSIGN_POST_PARAM_TO_INT64(object, name, var)                  \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(qint64));                       \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toVariant().toLongLong();               \
    } while(0)

#define ASSIGN_POST_PARAM_TO_BOOL(object, name, var)                   \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(bool));                         \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toBool();                               \
    } while(0)

#define ASSIGN_POST_PARAM_TO_DOUBLE(object, name, var)                 \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(double));                       \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toDouble();                             \
    } while(0)

#define ASSIGN_POST_PARAM_TO_DATETIME(object, name, var)               \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(QDateTime));                    \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toVariant().toDateTime();               \
    } while(0)

#define ASSIGN_POST_PARAM_TO_OBJECT(object, name, var)                 \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(QJsonObject));                  \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toObject();                             \
    } while(0)

#define ASSIGN_POST_PARAM_TO_ARRAY(object, name, var)                  \
    do                                                                 \
    {                                                                  \
        Q_ASSERT(typeid(var) == typeid(QJsonArray));                   \
        if(object.contains(name) && !object[name].isNull())            \
            var = object[name].toArray();                              \
    } while(0)

// 错误码定义
const QString HTTP_SERVER_ERROR_CODE_PREFIX = "STOCK-SERVER-";
const QString HTTP_SERVER_SUCCESS = "0000";
const QString HTTP_SERVER_SQL_ERROR = "1000";
const QString HTTP_SERVER_REQUIRD_HEADER_ERROR = "1001";
const QString HTTP_SERVER_REQUIRD_PARAM_ERROR = "1002";
const QString HTTP_SERVER_SELECT_CONFIG_WARES_ERROR = "1003";
const QString HTTP_SERVER_HAS_TRADE_ERROR_WARE = "1004";
const QString HTTP_SERVER_SELECT_USER_LOGIN_ERROR = "1005";
const QString HTTP_SERVER_SELECT_CONFIG_WARE_ERROR = "1006";
const QString HTTP_SERVER_SELECT_ORDER_REPORT_ERROR = "1007";
const QString HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR = "1008";


namespace Http
{
    // 错误码displayMsg定义
    static const QHash<QString, QString> HTTP_SERVER_ERROR_MESSAGES =
    {
        { HTTP_SERVER_SUCCESS, "success" },
        { HTTP_SERVER_SQL_ERROR, "database sql error" },
        { HTTP_SERVER_REQUIRD_HEADER_ERROR, "required header error" },
        { HTTP_SERVER_REQUIRD_PARAM_ERROR, "required param error" },
        { HTTP_SERVER_SELECT_CONFIG_WARES_ERROR, "select config wares error" },
        { HTTP_SERVER_HAS_TRADE_ERROR_WARE, "has trade error ware" },
        { HTTP_SERVER_SELECT_USER_LOGIN_ERROR, "select user login failed" },
        { HTTP_SERVER_SELECT_CONFIG_WARE_ERROR, "select config ware failed" },
        { HTTP_SERVER_SELECT_ORDER_REPORT_ERROR, "query order report failed" },
        { HTTP_SERVER_SELECT_INSUFFICIENT_STOCK_ERROR, "ware insufficient stock" },
    };

    // http server请求头
    struct HttpServerHeader
    {
        /** 追溯uuid */
        QString traceId;

        /** 设备类型 Stock App */
        QString deviceType;
    };

    // http server返回数据
    struct HttpServerResponse
    {
        /** 内部错误码 */
        QString code;

        /** 展示错误码 */
        QString displayCode;

        /** 内部错误信息 */
        QString msg;

        /** 展示错误信息 */
        QString displayMsg;

        /** 追溯uuid */
        QString traceId;

        /** 数据 */
        QJsonObject data;

        /** 是否成功 */
        bool success = true;
    };
    using HttpServerResponsePtr = QSharedPointer<HttpServerResponse>;
}