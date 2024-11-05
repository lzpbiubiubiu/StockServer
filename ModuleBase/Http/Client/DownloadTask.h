#pragma once

/**
* @file         DownloadTask.h
* @brief        Http客户端下载任务封装
* @details      负责客户端下载文件
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/
#include <QFile>

#include "Task.h"

namespace Http
{
    // Http 客户端下载任务封装类
    class MODULE_BASE_API DownloadTask : public Task
    {
        Q_OBJECT

    public:
        DownloadTask(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent = nullptr);
        virtual ~DownloadTask();

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
        void onReplyDownloadProgress(qint64 recv, qint64 total);
        void onTimeout();

    private:
        // 缓存文件对象
        QFile* m_file = nullptr;
    };
}