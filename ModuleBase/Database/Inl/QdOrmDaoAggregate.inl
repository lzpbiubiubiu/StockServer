#pragma once

#include "ModuleBase/Database/Orm/QdOrmDml.h"
#include "ModuleBase/Database/Orm/QdOrmFactory.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoAggregate
    {
    public:
        static inline QSqlError count(long long& size, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT(database != nullptr && meta != nullptr);

            QSqlQuery query = QdOrmDml::countRecord(database, meta, column.isEmpty() ? "*" : column, condition);
            QSqlError error = query.lastError();
            if(error.isValid())
            {
                size = 0;
                return error;
            }

            while(query.next())
            {
                size = query.value(0).toLongLong();
            }
            return error;
        }

        static inline QSqlError sum(QVariant& sum, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT(database != nullptr && meta != nullptr);

            QSqlQuery query = QdOrmDml::sumRecord(database, meta, column, condition);
            QSqlError error = query.lastError();
            if (error.isValid())
            {
                return error;
            }

            while (query.next())
            {
                sum = query.value(0);
            }
            return error;
        }

        static inline QSqlError avg(QVariant& avg, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT(database != nullptr && meta != nullptr);

            QSqlQuery query = QdOrmDml::avgRecord(database, meta, column, condition);
            QSqlError error = query.lastError();
            if (error.isValid())
            {
                return error;
            }

            while (query.next())
            {
                avg = query.value(0);
            }
            return error;
        }

        static inline QSqlError min(QVariant& min, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT(database != nullptr && meta != nullptr);

            QSqlQuery query = QdOrmDml::minRecord(database, meta, column, condition);
            QSqlError error = query.lastError();
            if (error.isValid())
            {
                return error;
            }

            while (query.next())
            {
                min = query.value(0);
            }
            return error;
        }

        static inline QSqlError max(QVariant& max, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT(database != nullptr && meta != nullptr);

            QSqlQuery query = QdOrmDml::maxRecord(database, meta, column, condition);
            QSqlError error = query.lastError();
            if (error.isValid())
            {
                return error;
            }

            while (query.next())
            {
                max = query.value(0);
            }
            return error;
        }
    };
}