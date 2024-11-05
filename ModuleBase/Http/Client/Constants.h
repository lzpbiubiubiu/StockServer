#pragma once

/**
* @file         Constants.h
* @brief        Http 客户端常量定义
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>

/** HTTP 网络错误码 */
#define ERR_NETWORK_SUCCESS                             "ERR_NETWORK_SUCCESS"
#define ERR_NETWORK_CONNECTION_REFUSED                  "ERR_NETWORK_CONNECTION_REFUSED"
#define ERR_NETWORK_REMOTE_HOST_CLOSED                  "ERR_NETWORK_REMOTE_HOST_CLOSED"
#define ERR_NETWORK_HOST_NOT_FOUND                      "ERR_NETWORK_HOST_NOT_FOUND"
#define ERR_NETWORK_TIMEOUT                             "ERR_NETWORK_TIMEOUT"
#define ERR_NETWORK_OPERATION_CANCELED                  "ERR_NETWORK_OPERATION_CANCELED"
#define ERR_NETWORK_SSL_HANDSHAKE_FAILED                "ERR_NETWORK_SSL_HANDSHAKE_FAILED"
#define ERR_NETWORK_TEMPORARY_NETWORK_FAILURE           "ERR_NETWORK_TEMPORARY_NETWORK_FAILURE"
#define ERR_NETWORK_SESSION_FAILED                      "ERR_NETWORK_SESSION_FAILED"
#define ERR_NETWORK_BACKGROUND_REQUEST_NOT_ALLOWED      "ERR_NETWORK_BACKGROUND_REQUEST_NOT_ALLOWED"
#define ERR_NETWORK_TOO_MANY_REDIRECTS                  "ERR_NETWORK_TOO_MANY_REDIRECTS"
#define ERR_NETWORK_INSECURED_REDIRECT                  "ERR_NETWORK_INSECURED_REDIRECT"
#define ERR_NETWORK_UNKNOWN_ERROR                       "ERR_NETWORK_UNKNOWN_ERROR"
#define ERR_NETWORK_PROXY_CONNECTION_REFUSED            "ERR_NETWORK_PROXY_CONNECTION_REFUSED"
#define ERR_NETWORK_PROXY_CONNECTION_CLOSED             "ERR_NETWORK_PROXY_CONNECTION_CLOSED"
#define ERR_NETWORK_PROXY_NOT_FOUND                     "ERR_NETWORK_PROXY_NOT_FOUND"
#define ERR_NETWORK_PROXY_TIMEOUT                       "ERR_NETWORK_PROXY_TIMEOUT"
#define ERR_NETWORK_PROXY_AUTHENTICATION_REQUIRED       "ERR_NETWORK_PROXY_AUTHENTICATION_REQUIRED"
#define ERR_NETWORK_UNKNOWN_PROXY                       "ERR_NETWORK_UNKNOWN_PROXY"
#define ERR_NETWORK_CONTENT_ACCESS_DENIED               "ERR_NETWORK_CONTENT_ACCESS_DENIED"
#define ERR_NETWORK_OPERATION_NOT_PERMITTED             "ERR_NETWORK_OPERATION_NOT_PERMITTED"
#define ERR_NETWORK_CONTENT_NOT_FOUND                   "ERR_NETWORK_CONTENT_NOT_FOUND"
#define ERR_NETWORK_AUTHENTICATION_REQUIRED             "ERR_NETWORK_AUTHENTICATION_REQUIRED"
#define ERR_NETWORK_CONTENT_RESEND                      "ERR_NETWORK_CONTENT_RESEND"
#define ERR_NETWORK_CONTENT_CONFLICT                    "ERR_NETWORK_CONTENT_CONFLICT"
#define ERR_NETWORK_CONTENT_GONE                        "ERR_NETWORK_CONTENT_GONE"
#define ERR_NETWORK_UNKNOWN_CONTENT                     "ERR_NETWORK_UNKNOWN_CONTENT"
#define ERR_NETWORK_PROTOCOL_UNKNOWN                    "ERR_NETWORK_PROTOCOL_UNKNOWN"
#define ERR_NETWORK_PROTOCOL_INVALID_OPERATION          "ERR_NETWORK_PROTOCOL_INVALID_OPERATION"
#define ERR_NETWORK_PROTOCOL_FAILURE                    "ERR_NETWORK_PROTOCOL_FAILURE"
#define ERR_NETWORK_INTERNAL_SERVER                     "ERR_NETWORK_INTERNAL_SERVER"
#define ERR_NETWORK_OPERATION_NOT_IMPLEMENTED           "ERR_NETWORK_OPERATION_NOT_IMPLEMENTED"
#define ERR_NETWORK_SERVICE_UNAVAILABLE                 "ERR_NETWORK_SERVICE_UNAVAILABLE"
#define ERR_NETWORK_UNKNOWN_SERVER                      "ERR_NETWORK_UNKNOWN_SERVER"

/** HTTP 用户扩展错误码 */
#define ERR_NETWORK_DISCONNECTED                        "ERR_NETWORK_DISCONNECTED"

#define ERR_NETWORK_CREATE_FILE_FAILED                  "ERR_NETWORK_CREATE_FILE_FAILED"

#define ERR_NETWORK_SSL_ERROR                           "ERR_NETWORK_SSL_ERROR"

/** Json数据填入宏 */
#define ASSIGN_FROM_INT32(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(int));object[name]=var; } while(0)

#define ASSIGN_FROM_INT32_IF_NOT_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(int));if(var!=0)object[name]=var; } while(0)

#define ASSIGN_FROM_INT32_IF_GREATER_THAN_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(int));if(var>0)object[name]=var; } while(0)

#define ASSIGN_FROM_INT32_IF_LESS_THAN_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(int));if(var<0)object[name]=var; } while(0)

#define ASSIGN_FROM_INT64(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(qint64));object[name]=var; } while(0)

#define ASSIGN_FROM_INT64_IF_NOT_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(qint64));if(var!=0)object[name]=var; } while(0)

#define ASSIGN_FROM_INT64_IF_GREATER_THAN_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(qint64));if(var>0)object[name]=var; } while(0)

#define ASSIGN_FROM_INT64_IF_LESS_THAN_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(qint64));if(var<0)object[name]=var; } while(0)

#define ASSIGN_FROM_BOOL(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(bool));object[name]=var; } while(0)

#define ASSIGN_FROM_BOOL_IF_TRUE(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(bool));if(var)object[name]=var; } while(0)

#define ASSIGN_FROM_BOOL_IF_FALSE(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(bool));if(!var)object[name]=var; } while(0)

#define ASSIGN_FROM_STRING(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(QString));object[name]=var; } while(0)

#define ASSIGN_FROM_STRING_IF_NOT_EMPTY(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(QString));if(!var.isEmpty())object[name]=var; } while(0)

#define ASSIGN_FROM_DOUBLE(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(double));object[name]=var; } while(0)

#define ASSIGN_FROM_DOUBLE_IF_GREATER_THAN_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(double));if(var>0)object[name]=var; } while(0)

#define ASSIGN_FROM_DOUBLE_IF_LESS_THAN_ZERO(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(double));if(var<0)object[name]=var; } while(0)

#define ASSIGN_FROM_OBJECT(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(QJsonObject));object[name]=var; } while(0)

#define ASSIGN_FROM_ARRAY(object, name, var)\
    do { Q_ASSERT(typeid(var)==typeid(QJsonArray));object[name]=var; } while(0)

/** Json数据解析宏 */
#define ASSIGN_TO_INT32(object, name, var)                  \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(int));               \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toInt();                     \
    } while(0)

#define ASSIGN_TO_INT64(object, name, var)                  \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(qint64));            \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toVariant().toLongLong();    \
    } while(0)

#define ASSIGN_TO_STRING(object, name, var)                 \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(QString));           \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toString();                  \
    } while(0)

#define ASSIGN_TO_LSTRING(object, name, defVal, var)        \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(Base::LString)); \
        var = Base::LStringUtil::FromLanMap(object, name, defVal); \
    } while(0)                                              \

#define ASSIGN_TO_BOOL(object, name, var)                   \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(bool));              \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toBool();                    \
    } while(0)

#define ASSIGN_TO_DOUBLE(object, name, var)                 \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(double));            \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toDouble();                  \
    } while(0)

#define ASSIGN_TO_DATETIME(object, name, var)               \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(QDateTime));         \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toVariant().toDateTime();    \
    } while(0)

#define ASSIGN_TO_OBJECT(object, name, var)                 \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(QJsonObject));       \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toObject();                  \
    } while(0)

#define ASSIGN_TO_ARRAY(object, name, var)                  \
    do                                                      \
    {                                                       \
        Q_ASSERT(typeid(var) == typeid(QJsonArray));        \
        if(object.contains(name) && !object[name].isNull()) \
            var = object[name].toArray();                   \
    } while(0)


namespace Http
{
    /** HTTP请求模式 */
    enum Verb
    {
        /** Get */
        GET,

        /** Post */
        POST,
    };

    /** HTTP日志格式 */
    enum LogFormat
    {
        /** 详细信息 */
        DETAIL,

        /** 数据大小（KB） */
        SIZE,
    };

    /** HTTP请求结果 */
    class Result
    {
    public:
        /** UUID */
        QString trace;

        /** URL地址 */
        QString url;

        /** 请求方法 */
        QString method;

        /** 请求结果 */
        bool ok = false;

        /** 状态码 */
        int status = 0;

        /** 错误码 */
        QString error;

        /** 错误描述 */
        QString message;

        /** 耗时（毫秒) */
        int elapsed = 0;

        /** 已发送字节数 */
        qint64 sent = 0;

        /** 已接收字节数*/
        qint64 recv = 0;

        QString ToString() const
        {
            QString str =
                "\n==========> HTTP RESULT\n"
                "|UUID: " + trace + "\n"
                "|URL: " + url + "\n"
                "|Method: " + method + "\n"
                "|Status: " + QString::number(status) + "\n"
                "|Error: " + error + "\n"
                "|Message: " + message + "\n"
                "|Time: " + QString::number(elapsed) + " ms \n"
                "|Sent: " + QString::number(sent / 1000.0, 'f', 2) + " KB \n"
                "|Recv: " + QString::number(recv / 1000.0, 'f', 2) + " KB \n";
            return str;
        }
    };
    using HttpResultPtr = QSharedPointer<Http::Result>;

    // 网络错误码定义
    static const QHash<int, const char*> NETWORK_ERROR_CODE_MAP = {
        { QNetworkReply::NoError, ERR_NETWORK_SUCCESS },
        { QNetworkReply::ConnectionRefusedError, ERR_NETWORK_CONNECTION_REFUSED },
        { QNetworkReply::RemoteHostClosedError, ERR_NETWORK_REMOTE_HOST_CLOSED },
        { QNetworkReply::HostNotFoundError, ERR_NETWORK_HOST_NOT_FOUND },
        { QNetworkReply::TimeoutError, ERR_NETWORK_TIMEOUT },
        { QNetworkReply::OperationCanceledError, ERR_NETWORK_OPERATION_CANCELED },
        { QNetworkReply::SslHandshakeFailedError, ERR_NETWORK_SSL_HANDSHAKE_FAILED },
        { QNetworkReply::TemporaryNetworkFailureError, ERR_NETWORK_TEMPORARY_NETWORK_FAILURE },
        { QNetworkReply::NetworkSessionFailedError, ERR_NETWORK_SESSION_FAILED },
        { QNetworkReply::BackgroundRequestNotAllowedError, ERR_NETWORK_BACKGROUND_REQUEST_NOT_ALLOWED },
        { QNetworkReply::TooManyRedirectsError, ERR_NETWORK_TOO_MANY_REDIRECTS },
        { QNetworkReply::InsecureRedirectError, ERR_NETWORK_INSECURED_REDIRECT },
        { QNetworkReply::UnknownNetworkError, ERR_NETWORK_UNKNOWN_ERROR },
        { QNetworkReply::ProxyConnectionRefusedError, ERR_NETWORK_PROXY_CONNECTION_REFUSED },
        { QNetworkReply::ProxyConnectionClosedError, ERR_NETWORK_PROXY_CONNECTION_CLOSED },
        { QNetworkReply::ProxyNotFoundError, ERR_NETWORK_PROXY_NOT_FOUND },
        { QNetworkReply::ProxyTimeoutError, ERR_NETWORK_PROXY_TIMEOUT },
        { QNetworkReply::ProxyAuthenticationRequiredError, ERR_NETWORK_PROXY_AUTHENTICATION_REQUIRED },
        { QNetworkReply::UnknownProxyError, ERR_NETWORK_UNKNOWN_PROXY },
        { QNetworkReply::ContentAccessDenied, ERR_NETWORK_CONTENT_ACCESS_DENIED },
        { QNetworkReply::ContentOperationNotPermittedError, ERR_NETWORK_OPERATION_NOT_PERMITTED },
        { QNetworkReply::ContentNotFoundError, ERR_NETWORK_CONTENT_NOT_FOUND },
        { QNetworkReply::AuthenticationRequiredError, ERR_NETWORK_AUTHENTICATION_REQUIRED },
        { QNetworkReply::ContentReSendError, ERR_NETWORK_CONTENT_RESEND },
        { QNetworkReply::ContentConflictError, ERR_NETWORK_CONTENT_CONFLICT },
        { QNetworkReply::ContentGoneError, ERR_NETWORK_CONTENT_GONE },
        { QNetworkReply::UnknownContentError, ERR_NETWORK_UNKNOWN_CONTENT },
        { QNetworkReply::ProtocolUnknownError, ERR_NETWORK_PROTOCOL_UNKNOWN },
        { QNetworkReply::ProtocolInvalidOperationError, ERR_NETWORK_PROTOCOL_INVALID_OPERATION },
        { QNetworkReply::ProtocolFailure, ERR_NETWORK_PROTOCOL_FAILURE },
        { QNetworkReply::InternalServerError, ERR_NETWORK_INTERNAL_SERVER },
        { QNetworkReply::OperationNotImplementedError, ERR_NETWORK_OPERATION_NOT_IMPLEMENTED },
        { QNetworkReply::ServiceUnavailableError, ERR_NETWORK_SERVICE_UNAVAILABLE },
        { QNetworkReply::UnknownServerError, ERR_NETWORK_UNKNOWN_SERVER } };

    // 网络错误描述定义
    static const QHash<int, QString> NETWORK_ERROR_MESSAGE_MAP = {
        { QNetworkReply::NoError, "Request succeeded"},
        { QNetworkReply::ConnectionRefusedError, "The remote server refused the connection (the server is not accepting requests)"},
        { QNetworkReply::RemoteHostClosedError, "The remote server closed the connection prematurely, before the entire reply was received and processed"},
        { QNetworkReply::HostNotFoundError, "The remote host name was not found (invalid hostname)"},
        { QNetworkReply::TimeoutError, "Network times out. Please contact IT Help or try again."},
        { QNetworkReply::OperationCanceledError, "The operation was canceled via calls to abort() or close() before it was finished."},
        { QNetworkReply::SslHandshakeFailedError, "The SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted."},
        { QNetworkReply::TemporaryNetworkFailureError, "The connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established."},
        { QNetworkReply::NetworkSessionFailedError, "The connection was broken due to disconnection from the network or failure to start the network."},
        { QNetworkReply::BackgroundRequestNotAllowedError, "The background request is not currently allowed due to platform policy."},
        { QNetworkReply::TooManyRedirectsError, "While following redirects, the maximum limit was reached. The limit is by default set to 50 or as set by QNetworkRequest::setMaxRedirectsAllowed(). (This value was introduced in 5.6.)"},
        { QNetworkReply::InsecureRedirectError, "While following redirects, the network access API detected a redirect from a encrypted protocol (https) to an unencrypted one (http). (This value was introduced in 5.6.)"},
        { QNetworkReply::UnknownNetworkError, "Unknown network error"},
        { QNetworkReply::ProxyConnectionRefusedError, "The connection to proxy server was refused (proxy server does not accept requests)."},
        { QNetworkReply::ProxyConnectionClosedError, "The proxy server closed the connection prematurely, before the entire reply was received and processed."},
        { QNetworkReply::ProxyNotFoundError, "The proxy host name was not found (invalid proxy hostname)."},
        { QNetworkReply::ProxyTimeoutError, "The connection to the proxy timed out or the proxy did not reply in time to the request sent."},
        { QNetworkReply::ProxyAuthenticationRequiredError, "The proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)."},
        { QNetworkReply::UnknownProxyError, "An unknown proxy-related error was detected."},
        { QNetworkReply::ContentAccessDenied, "The access to the remote content was denied (similar to HTTP error 403)"},
        { QNetworkReply::ContentOperationNotPermittedError, "The operation requested on the remote content is not permitted."},
        { QNetworkReply::ContentNotFoundError, "The content was not found at the server (similar to HTTP error 404)."},
        { QNetworkReply::AuthenticationRequiredError,"The remote server requires authentication to serve the content but the credentials provided were not accepted (if any)."},
        { QNetworkReply::ContentReSendError, "The request needed to be sent again, but this failed for example because the upload data could not be read a second time."},
        { QNetworkReply::ContentConflictError, "The request could not be completed due to a conflict with the current state of the resource."},
        { QNetworkReply::ContentGoneError, "The requested resource is no longer available at the server."},
        { QNetworkReply::UnknownContentError, "Unknown error detected related to remote content."},
        { QNetworkReply::ProtocolUnknownError, "The Network Access API cannot honor the request because the protocol is not known."},
        { QNetworkReply::ProtocolInvalidOperationError, "The requested operation is invalid for this protocol."},
        { QNetworkReply::ProtocolFailure, "A breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)."},
        { QNetworkReply::InternalServerError, "The server encountered an unexpected condition which prevented it from fulfilling the request."},
        { QNetworkReply::OperationNotImplementedError, "The server does not support the functionality required to fulfill the request."},
        { QNetworkReply::ServiceUnavailableError, "The server is unable to handle the request at this time."},
        { QNetworkReply::UnknownServerError, "An unknown error related to the server response was detected."}
    };
}