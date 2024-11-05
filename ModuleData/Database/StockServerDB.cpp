#include "StockServerDB.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleData/Entity/VersionEntity.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "ModuleData/Entity/UserLoginEntity.h"
#include "ModuleBase/Database/Orm/QdOrmDao.h"

namespace Sql
{
    bool StockServerDB::CreateTable()
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

        // 创建tb_config_ware表
        if (!tables.contains(ConfigWareEntity::GetTableName()))
        {
            QSqlError error = Orm::create_table<ConfigWareEntity>(&sqlDatabase);
            if (error.isValid())
            {
                LOG_ERROR("sql error: {}", error.databaseText().toStdString());
                return false;
            }
        }

        // 创建tb_user表
        if (!tables.contains(UserLoginEntity::GetTableName()))
        {
            QSqlError error = Orm::create_table<UserLoginEntity>(&sqlDatabase);
            if (error.isValid())
            {
                LOG_ERROR("sql error: {}", error.databaseText().toStdString());
                return false;
            }
        }

        return true;
    }
}