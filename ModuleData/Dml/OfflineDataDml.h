// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleData/Core/OfflineData.h"
#include "ModuleData/Entity/OfflineDataEntity.h"
#include "ModuleData/ModuleDataApi.h"

namespace Sql
{
    /** 离线数据推送数据持久化辅助类 */
    class MODULE_DATA_API PushDml : public QObject
    {
        Q_OBJECT
    public:
        PushDml(QObject* parent = nullptr);
        ~PushDml();

        void SetDatabase(QSharedPointer<DBBase> database);

        /** 添加任务组 */
        bool AddJobGroup(Core::OfflineDataPtr jobGroup, QString& sqlError);

        /** 删除任务组（主键匹配） */
        bool DeleteJobGroup(Core::OfflineDataPtr jobGroup, QString& sqlError);

        /** 清空所有任务 */
        bool DeleteAllJobGroups(QString& sqlError);

        /** 获取所有任务并按生成时间升序排列 */
        bool GetAllJobGroups(Core::OfflineDataPtrList& jobGroups, QString& sqlError);

        /** 指定主键数据是否已经存在 */
        bool IsJobGroupExist(Core::OfflineDataPtr jobGroup, QString& sqlError);

        /** 根据类型获取开始时间到结束时间内的所有任务并按生成时间升序排列 
        *  orgId: 门店ID
        *  cashierNo: 收银员编号
        *  deviceType: 设备类型 0-默认值 1-POS 2-PVT
        *  saleType: 0-默认值 1-销售 2-售后 3-押金收费 4-存取大钞 5-COD回款  
        *  handStartTime: 交班开始时间
        *  handEndTime: 交班结束时间
        */
        bool GetAllJobGroups(const QString& orgId, const QString& cashierNo, int deviceType,
                             int saleType, const QDateTime& handStartTime, const QDateTime& handEndTime,
                             bool asc, Core::OfflineDataPtrList& jobGroups, QString& sqlError);
        /** 获取指定数量的数据 */
        bool GetFixQuantityJobGroups(Core::OfflineDataPtrList& jobGroups, QString& sqlError,int quantity = 1,int startRow = 0);

        /** 获取数据库数据条数 */
        int GetJobGroupsQuantity();

        /** 获取指定条件的数据条数 */
        qint64 GetJobGroupsQuantity(const QVariantList& saleType, QString& sqlError);

        /** 获取指定条件的数据总金额 */
        qint64 GetJobGroupsAmount(const QVariantList& saleType, QString& sqlError);

    private:
        Sql::DmlBase<Sql::OfflineDataEntity> m_dmlPush;
    };
}