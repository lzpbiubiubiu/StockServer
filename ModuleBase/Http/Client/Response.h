#pragma once

/**
* @file         Response.h
* @brief        Http Response封装
* @copyright    Copyright (c) 2023 Dmall Pos Team
*/

#include <QObject>

#include "Constants.h"
#include "ModuleBase/ModuleBaseApi.h"

namespace Http
{
    // Http Response封装类
    class MODULE_BASE_API Response : public QObject
    {
        Q_OBJECT

    public:
        Response(QObject* parent = nullptr);
        virtual ~Response() = default;

        /** 响应数据相关 */
        void AppendData(const QByteArray& ba);
        const QByteArray& GetData() const;
        void ClearData();

        /** 错误信息相关 */
        void SetError(const QString& error);
        void SetErrorMessage(const QString& errorMessage);
        virtual QString GetError() const;
        virtual QString GetErrorMessage() const;

        /** 响应耗时相关 */
        inline void SetElapsed(qint64 elapsed) { m_elapsed = elapsed; }
        inline qint64 GetElapsed() const { return m_elapsed; }

        /** HTTP状态码相关 */
        inline void SetStatusCode(int statusCode) { m_statusCode = statusCode; }
        inline int GetStatusCode() const { return m_statusCode; }

        /** 日志相关 */
        inline void SetLogEnabled(bool enabled) { m_logEnabled = enabled; }
        inline bool IsLogEnabled() const { return m_logEnabled; }
        inline void SetLogFormat(LogFormat format) { m_logFormat = format; }
        inline LogFormat GetLogFormat() const { return m_logFormat; }

        /** 下载文件相关 */
        inline void SetDownloadSize(qint64 size) { m_downloadSize = size; }
        inline qint64 GetDownloadSize() const { return m_downloadSize; }

        /** 重置数据 */
        void Reset();

        /** 请求是否成功 */
        virtual bool IsSuccess() const;

        /** 反序列化响应数据 */
        virtual void Deserialize(const QByteArray& ba);

    protected:
        // 响应数据
        QByteArray m_data;

        // 网络错误码
        QString m_error;

        // 网络错误描述
        QString m_errorMessage;

        // 响应耗时（毫秒）
        qint64 m_elapsed = 0;

        // HTTP状态码
        int m_statusCode = 0;

        // 是否打印返回日志
        bool m_logEnabled = true;

        // 返回日志格式
        LogFormat m_logFormat = LogFormat::DETAIL;

        // 下载文件大小
        qint64 m_downloadSize = 0;
    };
}

//Q_DECLARE_METATYPE(Http::Response)