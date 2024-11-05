#pragma once

#include <QSqlQuery>
#include <QVariantMap>
#include <QSqlDatabase>
#include "ModuleBase/Database/Orm/QdOrmClass.h"
#include "ModuleBase/Database/Orm/QdOrmWhere.h"

namespace Orm
{
    class QdOrmBuilder
    {
    public:
        static QString createTable(QSqlDatabase* database, const QdOrmClass* meta);

        static QString dropTable(QSqlDatabase* database, const QdOrmClass* meta);

        static QSqlQuery exist(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);

        static QSqlQuery insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlQuery insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);

        static QSqlQuery saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlQuery saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);

        static QSqlQuery deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlQuery deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);
        static QSqlQuery deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition);

        static QSqlQuery updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlQuery updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);
        static QSqlQuery updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap, const QdOrmWhere& condition);

        static QSqlQuery fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlQuery fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition);

        static QSqlQuery countRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery sumRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery avgRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery minRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery maxRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);

    private:
        static QString operateSql(const QdOrmWhereItemPtr& item);

        static QString compareSql(const QdOrmWhereItemPtr& item);
    };
}