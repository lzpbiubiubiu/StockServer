#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "RecordDataDml.h"
//#include "ModuleData/Core/DatabaseManager.h"
//#include "ModuleBase/Common/ServiceManager.h"

namespace Sql
{
    RecordDataDml::RecordDataDml(QObject* parent)
        : QObject(parent)
    {
    }

    RecordDataDml::~RecordDataDml()
    {}

    void RecordDataDml::SetDatabase(QSharedPointer<DBBase> database)
    {
        m_dmlRecord.SetDatabase(database);
    }

    bool RecordDataDml::AddRecord(Core::RecordDataPtr record, QString& sqlError)
    {
        auto entity = Core::ConvertUtil::ToEntity(record);
        return m_dmlRecord.Add(entity, sqlError);
    }

    bool RecordDataDml::DeleteRecord(Core::RecordDataPtr record, QString& sqlError)
    {
        auto entity = Core::ConvertUtil::ToEntity(record);
        return m_dmlRecord.DeleteById(entity, sqlError);
    }

    bool RecordDataDml::DeleteAllRecords(QString& sqlError)
    {
        return m_dmlRecord.DeleteAll(sqlError);
    }

    bool RecordDataDml::GetAllRecords(Core::RecordDataPtrList& records, QString& sqlError)
    {
        Sql::RecordDataEntityPtrList entities;
        bool result = m_dmlRecord.ListAll(entities, sqlError);
        if(result && !entities.isEmpty())
            records = Core::ConvertUtil::FromEntityList(entities);
        return result;
    }

    bool RecordDataDml::IsRecordExist(Core::RecordDataPtr record, QString& sqlError)
    {
        auto entity = Core::ConvertUtil::ToEntity(record);
        return m_dmlRecord.Exist(entity, sqlError);
    }

    bool RecordDataDml::GetFixQuantityRecords(Core::RecordDataPtrList& jobGroups, QString& sqlError, int quantity, int startRow)
    {
        Sql::RecordDataEntityPtrList entities;
        auto query = Orm::QdOrmWhere();
        query.limit(quantity, startRow);
        bool result = m_dmlRecord.FindByQuery(entities, query, sqlError);
        if(result && !entities.isEmpty())
            jobGroups = Core::ConvertUtil::FromEntityList(entities);
        return result;
    }

    int RecordDataDml::GetRecordsQuantity()
    {
        qint64 quantity = 0;
        QString col;
        QString sqlError;
        m_dmlRecord.CountAll(quantity,col,sqlError);
        return quantity;
    }
}