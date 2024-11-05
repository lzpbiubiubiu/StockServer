#pragma once

/**
* @file         Task.h
* @brief        Http客户端普通任务封装
* @details      负责客户端普通Http请求和接收
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include "Task.h"

namespace Http
{
    // Http 客户端普通任务封装类
    class MODULE_BASE_API NormalTask : public Task
    {
        Q_OBJECT

    public:
        NormalTask(QSharedPointer<Request> request, QSharedPointer<Response> response, QObject* parent = nullptr);
        virtual ~NormalTask() = default;

        /** 任务执行 */
        virtual void Run() override;

    private:
        /** Http Get处理 */
        void ProcessGet(QNetworkRequest& request);

        /** Http Post处理 */
        void ProcessPost(QNetworkRequest& request);

        /** 完成处理 */
        void Finished();

    protected Q_SLOTS:
        void onReplyReadReady();
        void onReplyFinished();
        void onReplyError(QNetworkReply::NetworkError code);
        void onReplySslErrors(const QList<QSslError>& errors);
        void onReplyDownloadProgress(qint64 recv, qint64 total);
        void onReplyUploadProgress(qint64 sent, qint64 total);
        void onTimeout();
    };
}