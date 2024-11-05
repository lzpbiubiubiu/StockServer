#pragma once

#include "ModuleBase/Database/Orm/QdOrmDml.h"
#include "ModuleBase/Database/Orm/QdOrmFactory.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoTable
    {
    public:
        static inline QSqlError createTable(QSqlDatabase* database)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoTable::createTable()"
                , "database or meta is nullptr");
            return QdOrmDml::createTable(database, meta);
        }

        static inline QSqlError dropTable(QSqlDatabase* database)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoTable::dropTable()"
                , "database or meta is nullptr");
            return QdOrmDml::dropTable(database, meta);
        }
    };
}