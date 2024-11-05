#pragma once

/**
* @file         TaskAdapter.h
* @brief        Http客户端任务适配器封装
* @details      适配QThreadPool与Task实现兼容问题，让Task对象属于QThreadPool线程，而不是只有Run函数属于QThreadPool线程
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QRunnable>
#include <QSharedPointer>

#include "Constants.h"
#include "ModuleBase/ModuleBaseApi.h"
#include "Request.h"
#include "Response.h"

namespace Http
{
    enum TaskType
    {
        // 普通Http请求
        NORMAL = 0,

        // 文件下载
        DOWNLOAD,

        // 文件上传
        UPLOAD
    };

    // Http 客户端任务适配器封装类
    class MODULE_BASE_API TaskAdapter : public QObject, public QRunnable
    {
        Q_OBJECT

    public:
        TaskAdapter(TaskType type, QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent = nullptr);
        virtual ~TaskAdapter() = default;

        /** 线程入口函数 */
        virtual void run() override;

    Q_SIGNALS:
        // 任务完成信号
        void signalFinished(HttpResultPtr result);

    private:
        TaskType m_type = NORMAL;

        // 请求对象
        QSharedPointer<Request> m_request;

        // 响应对象
        QSharedPointer<Response> m_response;
    };
}