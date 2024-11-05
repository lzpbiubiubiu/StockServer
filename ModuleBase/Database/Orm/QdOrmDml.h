#pragma once

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include "ModuleBase/ModuleBaseApi.h"
#include "ModuleBase/Database/Orm/QdOrmClass.h"
#include "ModuleBase/Database/Orm/QdOrmWhere.h"

namespace Orm
{
    class SQLRecordHelper
    {
    public:
        SQLRecordHelper(QSqlDatabase* database, QSqlQuery& qurey);
        ~SQLRecordHelper();

        void SetStatus(bool success);

    private:
        QSqlQuery& m_query;
        QSqlDatabase* m_database = nullptr;
        bool m_success = true;
    };

    class MODULE_BASE_API QdOrmDml
    {
    public:
        /** 创建数据库 */
        static QSqlError createTable(QSqlDatabase* database, const QdOrmClass* meta);

        /** 删除数据库 */
        static QSqlError dropTable(QSqlDatabase* database, const QdOrmClass* meta);

        /** 主键数据是否存在 */
        static bool exist(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);

        /** 插入数据 */
        static QSqlError insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlError insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);

        /** 保存数据 */
        static QSqlError saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlError saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);

        /** 删除数据 */
        static QSqlError deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlError deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);
        static QSqlError deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition);

        /** 更新数据 */
        static QSqlError updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlError updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap);
        static QSqlError updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap, const QdOrmWhere& condition);

        /** 获取数据 */
        static QSqlQuery fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap);
        static QSqlQuery fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition);

        /** 聚合函数 */
        static QSqlQuery countRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery sumRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery avgRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery minRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
        static QSqlQuery maxRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition);
    };
}