#pragma once

/**
* @file         Request.h
* @brief        Http Request封装
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <functional>

#include <QHttpMultiPart>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QUrl>

#include "Constants.h"
#include "ModuleBase/ModuleBaseApi.h"

namespace Http
{
    class Request;
    class Response;

    using CompleteCallback
        = std::function<void(QSharedPointer<Request> request, QSharedPointer<Response> response)>;

    using ProgressCallback
        = std::function<void(QSharedPointer<Request> request, QSharedPointer<Response> response, qint64 current, qint64 total, bool upload)>;

    /** Http Request封装类 */
    class MODULE_BASE_API Request : public QObject
    {
        Q_OBJECT

    public:
        Request(QObject* parent = nullptr);
        virtual ~Request() = default;

        /** 请求地址相关 */
        void SetUrl(const QString& url);
        QString GetUrl();
        QUrl GetQUrl();
        QString GetHost();
        QString GetPath();
        int GetPort();

        /** SSL支持 */
        inline void SetSslEnabled(bool enabled) { m_sslEnabled = enabled; }
        inline bool IsSslEnabled() const { return m_sslEnabled; }

        /** header相关 */
        void SetHeader(const QString& key, const QString& value);
        const QMap<QString, QString>& GetHeader() const;
        QJsonObject GetHeaderJson() const;

        /** 请求方法相关 */
        inline void SetVerb(Verb verb) { m_verb = verb; }
        inline Verb GetVerb() const { return m_verb; }

        /** 打印日志相关 */
        inline void SetLogEnabled(bool enabled) { m_logEnabled = enabled; }
        inline bool IsLogEnabled() const { return m_logEnabled; }
        inline void SetLogFormat(LogFormat format) { m_logFormat = format; }
        inline LogFormat GetLogFormat() const { return m_logFormat; }

        /** MultiPart数据相关 */
        void SetMultiPart(QSharedPointer<QHttpMultiPart> multiPart);
        QSharedPointer<QHttpMultiPart> GetMultiPart() const;

        /** 请求数据 */
        void SetData(const QByteArray& data);
        const QByteArray& GetData() const;

        /** 重定向相关 */
        inline void SetRedirectionEnabled(bool enabled) { m_redirectionEnabled = enabled; }
        inline bool IsRedirectionEnabled() const { return m_redirectionEnabled; }

        /** 超时相关 */
        inline void SetTimeoutRetryCount(int count) { m_timeoutRetryCount = count; }
        inline int GetTimeoutRetryCount() const { return m_timeoutRetryCount; }
        inline void SetTimeout(int timeout) { m_timeout = timeout; }
        inline int GetTimeout() const { return m_timeout; }

        /** 文件下载相关 */
        inline void SetDownloadFilePath(const QString& path) { m_downloadFilePath = path; }
        inline QString GetDownloadFilePath() const { return m_downloadFilePath; }
        inline QString GetDownloadFileName() { return m_downloadFilePath.split("/").last(); }
        inline void SetMd5(const QString& md5) { m_md5 = md5; }
        inline QString GetMd5() const { return m_md5; }

        /** 文件上传相关 */
        inline void SetUploadFilePath(const QString& path) { m_uploadFilePath = path; }
        inline QString GetUploadFilePath() const { return m_uploadFilePath; }
        inline void SetUploadDir(const QString& dir) { m_uploadDir = dir; }
        inline QString GetUploadDir() const { return m_uploadDir; }

        /** 异步回调相关 */
        void SetCompleteCallback(CompleteCallback callback);
        CompleteCallback GetCompleteCallback() const;
        void SetProgressCallback(ProgressCallback callback);
        ProgressCallback GetProgressCallback() const;

        /** 序列化POST请求数据 */
        virtual void SerializeForPost(QByteArray& ba);

    private:
        // 请求url
        QUrl m_url;

        // 是否启用ssl
        bool m_sslEnabled = true;

        // 请求头部
        QMap<QString, QString> m_header;

        // 请求方法
        Verb m_verb = GET;

        // 是否打印请求日志
        bool m_logEnabled = true;

        // 打印日志格式
        LogFormat m_logFormat = LogFormat::DETAIL;

        // MultiPart数据
        QSharedPointer<QHttpMultiPart> m_multiPart;

        // 请求数据
        QByteArray m_data;

        // 是否允许重定向
        bool m_redirectionEnabled = true;

        // 超时自动重试次数
        int m_timeoutRetryCount = 0;

        // 超时时间（毫秒）
        int m_timeout = -1;

        // 下载文件路径
        QString m_downloadFilePath;

        // 下载文件md5
        QString m_md5;

        // 上传文件路径
        QString m_uploadFilePath;

        // 上传到服务器目录
        QString m_uploadDir;

        // 完成回调函数（成功+错误）
        CompleteCallback m_completeCallback = nullptr;

        // 进度回调函数（下载+上传）
        ProgressCallback m_progressCallback = nullptr;
    };
}