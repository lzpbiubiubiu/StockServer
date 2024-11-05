#include "BusinessDB.h"
#include "ModuleData/Entity/VersionEntity.h"
#include "ModuleData/Entity/OrderEntity.h"
#include "ModuleBase/Database/Orm/QdOrmDao.h"
#include "ModuleBase/Log/Log.h"

namespace Sql
{
    bool BusinessDB::CreateTable()
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

        // 创建tb_order表
        if (!tables.contains(OrderEntity::GetTableName()))
        {
            QSqlError error = Orm::create_table<OrderEntity>(&sqlDatabase);
            if (error.isValid())
            {
                LOG_ERROR("sql error: {}", error.databaseText().toStdString());
                return false;
            }
        }
        return true;
    }
}
