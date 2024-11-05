#pragma once

/**
* @file         UploadTask.h
* @brief        Http客户端上传任务封装
* @details      负责客户端上传文件
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QFile>

#include "Task.h"

namespace Http
{
    // Http 客户端上传任务封装类
    class MODULE_BASE_API UploadTask : public Task
    {
        Q_OBJECT

    public:
        UploadTask(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent = nullptr);
        virtual ~UploadTask();

        /** 任务执行 */
        virtual void Run() override;

    protected:
        /** 打印请求日志 */
        void PrintRequestLog() override;

        /** 打印响应日志 */
        void PrintResponseLog() override;

        /** 完成处理 */
        void Finished();

    protected Q_SLOTS:
        void onReplyReadReady();
        void onReplyFinished();
        void onReplyError(QNetworkReply::NetworkError code);
        void onReplySslErrors(const QList<QSslError>& errors);
        void onReplyUploadProgress(qint64 sent, qint64 total);
        void onTimeout();

    private:
        // 上传文件对象
        QFile* m_file = nullptr;
    };
}