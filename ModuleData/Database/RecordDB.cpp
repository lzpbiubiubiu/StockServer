#include "RecordDB.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleData/Entity/VersionEntity.h"
#include "ModuleData/Entity/RecordDataEntity.h"
#include "ModuleData/Entity/OfflineDataEntity.h"
#include "ModuleBase/Database/Orm/QdOrmDao.h"

namespace Sql
{
    bool RecordDB::CreateTable()
    {
        QWriteLocker locker(&GetLock());
        auto sqlDatabase = GetSqlDatabase();
        if(!sqlDatabase.isOpen())
            return false;

        auto tables = sqlDatabase.tables();

        // 创建tb_version表
        if(!tables.contains(VersionEntity::GetTableName()))
        {
            QSqlError error = Orm::create_table<VersionEntity>(&sqlDatabase);
            if(error.isValid())
            {
                LOG_ERROR("sql error: {}", error.databaseText().toStdString());
                return false;
            }
        }

        // 创建tb_record_data表
        if(!tables.contains(RecordDataEntity::GetTableName()))
        {
            QSqlError error = Orm::create_table<RecordDataEntity>(&sqlDatabase);
            if(error.isValid())
            {
                LOG_ERROR("sql error: {}", error.databaseText().toStdString());
                return false;
            }
        }

        // 创建tb_offline_data表
        if(!tables.contains(OfflineDataEntity::GetTableName()))
        {
            QSqlError error = Orm::create_table<OfflineDataEntity>(&sqlDatabase);
            if(error.isValid())
            {
                LOG_ERROR("sql error: {}", error.databaseText().toStdString());
                return false;
            }
        }

        return true;
    }
}