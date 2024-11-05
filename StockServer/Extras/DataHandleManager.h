#pragma once
/**
* @file         DataHandleManager.h
* @brief        数据管理类
* @details      负责数据的导入和导出
* @copyright    Copyright (c) 2024 Dmall Pos Team
*/

#include <QObject>
#include <QHash>
#include <QString>

#include "ModuleBase/Service/AsyncService.h"
#include "ModuleData/Dml/OfflineDataDml.h"
#include "ModuleData/Dml/RecordDataDml.h"

namespace Extras
{
    class DataHandleManager : public Base::AsyncService
    {
        Q_OBJECT
        SUPPORT_FROZEN_FLAG()
    public:

        enum ExportType
        {
            /** 离线任务 */
            OFFLINE_TASK = 1,

            /** 记录 */
            RECORD_TASK = 2
        };
        Q_ENUM(ExportType)

        DataHandleManager(QObject* parent = nullptr);
        virtual ~DataHandleManager();

        /** 初始化 */
        void Init();

        /** 导出交易数据 */
        void ExportTradeData(const QString& dateTime, const QString& path);

        /** 导入交易数据 */
        void ImportTradeData(const QString& path);

    protected:
        /** 导出数据 */
        Q_INVOKABLE void onExportTradeData(const QString& dateTime, const QString& path);

        /** 导出数据(分片导出) */
        Q_INVOKABLE void onExportTradeDataNew(const QString& dateTime, const QString& path);

        /** 导入数据 */
        Q_INVOKABLE void onImportTradeData(const QString& path);

    protected:
        virtual bool OnStart() override;
        virtual void OnStop() override;

    private Q_SLOTS:
        /** 自动对账检查定时器 */
        void CheckTimeout();

    Q_SIGNALS:
        /** 数据导出信号 */
        void signalExportOfflineDataSuccess(bool exportState,const QString& msg = QString());

        /** 数据导入信号 */
        void signalImportOfflineDataSuccess(bool exportState);

    private:
        /** 读自动对账接口请求 */
        void OfflineCheckAccounts(const QJsonArray& data);

        /** 读取交易数据 */
        bool ReadCsvData(QString filePath, QList<QStringList>& data);

        /** 写交易数据 */
        bool WriteCsv(QString filePath, QList<QStringList>& data);

        /** 写交易数据 */
        QList<QStringList> JoinData(Core::OfflineDataPtrList& offlineDataList, Core::RecordDataPtrList& recordDataList, const QString& dateTime, bool header = false);

        /** 时间处理 */
        bool IsTimePoint();

        /** 获取对账数据 */
        QJsonObject GetCheckAccountsData(const QDate& date);

        /** 文件拷贝 */
        bool CopyFileToDirectory(const QString& sourceFile, const QString& destDirectory);

        /** 文件压缩 */
        bool CompressFilesToDirectory(const QList<QString>& fileList,const QString& sourceFile, const QString& destDirectory=QString());

        /** 文件压缩 */
        QList<QString> WriteData(const QString& fileName, const QDate& dateTime);
    private:
        // 数据库操作对象
        Sql::PushDml m_pushDml;

        // 数据库操作对象
        Sql::RecordDataDml m_recordDml;

        // 自动对账定时器
        QTimer* m_checkTimer = nullptr;
    };
}
