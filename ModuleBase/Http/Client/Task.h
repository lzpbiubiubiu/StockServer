#pragma once

/**
* @file         Task.h
* @brief        Http客户端任务基类封装
* @details      负责客户端请求和接收
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QTimer>

#include "Constants.h"
#include "ModuleBase/ModuleBaseApi.h"
#include "Request.h"
#include "Response.h"

namespace Http
{
    // Http 客户端任务封装基类
    class MODULE_BASE_API Task : public QObject
    {
        Q_OBJECT

    public:
        Task(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent = nullptr);
        virtual ~Task();

        /** 设置网络管理器 */
        void SetNetworkAccessManager(QNetworkAccessManager* networkAccessManager);

        /** 任务执行 */
        virtual void Run() = 0;

    protected:

        /** 设置请求头部 */
        void SetRawHeader(QNetworkRequest& request);

        /** 检查任务是否超时 */
        bool IsExpired(qint64 timestamp) const;

        /** 打印请求日志 */
        virtual void PrintRequestLog();

        /** 打印响应日志 */
        virtual void PrintResponseLog();

    Q_SIGNALS:
        // 任务完成信号
        void signalFinished(HttpResultPtr result);

    protected:
        // 任务唯一标识ID
        QString m_uuid;

        // 请求对象
        QSharedPointer<Request> m_request;

        // 响应对象
        QSharedPointer<Response> m_response;

        // 请求时间计算定时器
        QTimer* m_timer = nullptr;

        // 网络访问管理器
        QNetworkAccessManager* m_networkAccessManager = nullptr;

        // 网络响应对象
        QNetworkReply* m_networkReply = nullptr;

        // 请求开始时间记录(ms)
        qint64 m_timestamp = 0;

        // 请求耗时(ms)
        qint64 m_elapsed = 0;
    };
}