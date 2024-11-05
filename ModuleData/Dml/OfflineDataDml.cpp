#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "OfflineDataDml.h"
//#include "ModuleBase/Common/ServiceManager.h"
//#include "ModuleData/Core/DatabaseManager.h"

namespace Sql
{
    PushDml::PushDml(QObject* parent)
        : QObject(parent)
    {
    }

    PushDml::~PushDml()
    {}

    void PushDml::SetDatabase(QSharedPointer<DBBase> database)
    {
        m_dmlPush.SetDatabase(database);
    }

    bool PushDml::AddJobGroup(Core::OfflineDataPtr jobGroup, QString& sqlError)
    {
        auto entity = Core::ConvertUtil::ToEntity(jobGroup);
        return m_dmlPush.Add(entity, sqlError);
    }

    bool PushDml::DeleteJobGroup(Core::OfflineDataPtr jobGroup, QString& sqlError)
    {
        auto entity = Core::ConvertUtil::ToEntity(jobGroup);
        return m_dmlPush.DeleteById(entity, sqlError);
    }

    bool PushDml::DeleteAllJobGroups(QString& sqlError)
    {
        return m_dmlPush.DeleteAll(sqlError);
    }

    bool PushDml::GetAllJobGroups(Core::OfflineDataPtrList& jobGroups, QString& sqlError)
    {
        Sql::OfflineDataEntityPtrList entities;
        auto orgId = Core::GlobalData::Get()->GetConfig().device.storeId;
        auto query = Orm::QdOrmWhere();

        query.where(OfflineDataEntity::ColumnOrgId()).isEqualTo(orgId);

        bool result = m_dmlPush.FindByQuery(entities, query, sqlError);
        if(result && !entities.isEmpty())
            jobGroups = Core::ConvertUtil::FromEntityList(entities);
        return result;
    }

    bool PushDml::IsJobGroupExist(Core::OfflineDataPtr jobGroup, QString& sqlError)
    {
        auto entity = Core::ConvertUtil::ToEntity(jobGroup);
        return m_dmlPush.Exist(entity, sqlError);
    }
    bool PushDml::GetAllJobGroups(const QString& orgId, const QString& cashierNo, int deviceType,
                             int saleType, const QDateTime& handStartTime, const QDateTime& handEndTime,
                             bool asc, Core::OfflineDataPtrList& jobGroups, QString& sqlError)
    {
        Sql::OfflineDataEntityPtrList entities;
        auto query = Orm::QdOrmWhere();
        query.where(OfflineDataEntity::ColumnOrgId()).isEqualTo(orgId)
            .and_(OfflineDataEntity::ColumnDeviceType()).isEqualTo(deviceType)
            .and_(OfflineDataEntity::ColumnCashierNo()).isEqualTo(cashierNo)
            .and_(OfflineDataEntity::ColumnSaleType()).isEqualTo(saleType)
            .and_(OfflineDataEntity::ColumnCreateTime()).isBetween(handStartTime, handEndTime);
        qint64 handTime1 = handStartTime.toMSecsSinceEpoch();
        qint64 handTime2 = handEndTime.toMSecsSinceEpoch();
        if(asc)
            query.orderAsc(QStringList() << "create_time");
        else
            query.orderDesc(QStringList() << "create_time");
        bool result = m_dmlPush.FindByQuery(entities, query, sqlError);
        if(result && !entities.isEmpty())
            jobGroups = Core::ConvertUtil::FromEntityList(entities);
        return result;
    }

    bool PushDml::GetFixQuantityJobGroups(Core::OfflineDataPtrList& jobGroups, QString& sqlError, int quantity, int startRow)
    {
        Sql::OfflineDataEntityPtrList entities;
        auto query = Orm::QdOrmWhere();
        query.limit(quantity, startRow);
        bool result = m_dmlPush.FindByQuery(entities, query, sqlError);
        if(result && !entities.isEmpty())
            jobGroups = Core::ConvertUtil::FromEntityList(entities);
        return result;
    }

    int PushDml::GetJobGroupsQuantity()
    {
        qint64 quantity = 0;
        QString col;
        QString sqlError;
        m_dmlPush.CountAll(quantity,col, sqlError);
        return quantity;
    }

    qint64 PushDml::GetJobGroupsQuantity(const QVariantList& saleType, QString& sqlError)
    {
        QString col;
        long long quantity = 0;

        auto query = Orm::QdOrmWhere();
        query.where(OfflineDataEntity::ColumnSaleType()).in(saleType);

        m_dmlPush.CountByQuery(quantity, query, OfflineDataEntity::ColumnUuid(), sqlError);
        return quantity;
    }

    qint64 PushDml::GetJobGroupsAmount(const QVariantList& saleType, QString& sqlError)
    {
        QVariant totalAmount;
        auto query = Orm::QdOrmWhere();
        query.where(OfflineDataEntity::ColumnSaleType()).in(saleType);

        m_dmlPush.SumByQuery(totalAmount, query, OfflineDataEntity::ColumnAmount(), sqlError);
        return totalAmount.toLongLong();
    }
}