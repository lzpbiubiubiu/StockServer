#pragma once

#include <QTimer>
#include "ModuleBase/Http/Client/Client.h"
#include "ModuleBase/Service/AsyncService.h"
#include "UpdateStruct.h"

namespace Extra
{
    /** 包管理服务，负责程序组件更新 */
    class PackageManager : public Base::AsyncService
    {
        Q_OBJECT
        SUPPORT_FROZEN_FLAG()

    public:
        /** 错误码 */
        enum PatchError
        {
            /** 成功 */
            PATCH_ERR_SUCCESS = 0,

            /** 校验失败 */
            PATCH_ERR_CHECKSUM = -1,

            /** 解压失败 */
            PATCH_ERR_DECOMPRESSING = -2,

            /** 解析文件失败 */
            PATCH_ERR_PARSE_INFO = -3,

            /** 释放文件失败 */
            PATCH_ERR_APPLY_TO_FILE = -4,

            /** 拷贝文件失败 */
            PATCH_ERR_COPY_FILE = -5,

            /** 创建文件夹失败 */
            PATCH_ERR_CREATE_DIRECTORY = -6,

            /** 文件不存在 */
            PATCH_ERR_FILE_NOT_EXIST = -7,

            /** 其他错误 */
            PATCH_ERR_KNOWN = -8,
        };
        Q_ENUM(PatchError)

        /** 补丁信息 */
        struct PatchInfo
        {
            /** 版本名 */
            QString version;

            /** 下载地址 */
            QString url;

            /** 文件MD5 */
            QString md5;

            /** 是否差量包 */
            bool isPatched = false;

            /** 是否强制更新 */
            bool isForced = false;
        };

        PackageManager(QObject* parent = nullptr);
        virtual ~PackageManager();

        /** 检查更新 */
        void CheckUpdate();

        /** 下载补丁包 */
        void DownloadPackage(const PatchInfo& info);

        /** 更新程序 */
        void UpdatePackage(const PatchInfo& info, const QString& fileName);

        /** 上报当前版本号 */
        void ReportVersion();

    protected:
        virtual bool OnStart() override;
        virtual void OnStop() override;

        /** 查询版本更新 */
        bool QueryVersion(bool& found, PatchInfo& info, QString& message);

        /** 应用补丁 */
        PatchError ApplyPatch(const PatchInfo& info, const QString& patchFile, QString& message);

        /** 准备等待升级的文件，成功返回目录路径，否则返回空 */
        QString PrepareFiles(const PatchInfo& info) const;

        /** 查找目录下所有文件 */
        void FindFiles(const QString& root, const QString& parent, QMap<QString, FileInfo>& infoList) const;

        /** 更新文件 */
        PatchError PatchFiles(
            const QString& oldDir,
            const QString& patchDir,
            const QMap<QString, FileInfo>& oldFiles,
            const QMap<QString, UpdateFileInfo>& patchFiles, QString& message) const;

        /** 更新程序设置 */
        void UpdateSettings(const PatchInfo& info) const;

        Q_INVOKABLE void onCheckUpdate();

        Q_INVOKABLE void onDownloadPackage(const PatchInfo& info);

        Q_INVOKABLE void OnDownloadResult(const PatchInfo& info, QSharedPointer<Http::Request> request, QSharedPointer<Http::Response> response);

        Q_INVOKABLE void OnDownloadProgress(const PatchInfo& info, qint64 bytesReceived, qint64 bytesTotal);

        Q_INVOKABLE void onUpdatePackage(const PatchInfo& info, const QString& fileName);

        Q_INVOKABLE void onReportVersion();

    protected Q_SLOTS:
        void onTimeout();

    Q_SIGNALS:
        /** 检查更新信号 */
        void signalCheckUpdateSuccess(bool found, const PatchInfo& info);
        void signalCheckUpdateError(const QString& message);

        /** 下载补丁包信号 */
        void signalDownloadPackageSuccess(const PatchInfo& info, const QString& filePath);
        void signalDownloadPackageError(const PatchInfo& info, const QString& message);
        void signalDownloadPackageProgress(const PatchInfo& info, qint64 bytesReceived, qint64 bytesTotal);

        /** 更新程序信号 */
        void signalUpdatePackageSuccess(const PatchInfo& info);
        void signalUpdatePackageError(const PatchInfo& info, PatchError code, const QString& message);
        void signalUpdatePackageProgress(const PatchInfo& info, const QString& message);

    private:
        // 检查更新定时器
        QTimer* m_timer = nullptr;
    };
}