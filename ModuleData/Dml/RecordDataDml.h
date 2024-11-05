// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleData/Core/RecordData.h"
#include "ModuleData/Entity/RecordDataEntity.h"
#include "ModuleData/ModuleDataApi.h"

namespace Sql
{
    /** POS、PVT报表数据持久化辅助类 */
    class MODULE_DATA_API RecordDataDml : public QObject
    {
        Q_OBJECT
    public:
        RecordDataDml(QObject* parent = nullptr);
        ~RecordDataDml();

        void SetDatabase(QSharedPointer<DBBase> database);

        /** 添加记录 */
        bool AddRecord(Core::RecordDataPtr record, QString& sqlError);

        /** 删除（主键匹配） */
        bool DeleteRecord(Core::RecordDataPtr record, QString& sqlError);

        /** 清空所有任务 */
        bool DeleteAllRecords(QString& sqlError);

        /** 获取所有任务并按生成时间升序排列 */
        bool GetAllRecords(Core::RecordDataPtrList& reocrds, QString& sqlError);

        /** 指定主键数据是否已经存在 */
        bool IsRecordExist(Core::RecordDataPtr record, QString& sqlError);

        /** 获取指定数量的数据 */
        bool GetFixQuantityRecords(Core::RecordDataPtrList& jobGroups, QString& sqlError, int quantity = 1, int startRow = 0);

        /** 获取数据库数据条数 */
        int GetRecordsQuantity();
    private:
        Sql::DmlBase<Sql::RecordDataEntity> m_dmlRecord;
    };
}