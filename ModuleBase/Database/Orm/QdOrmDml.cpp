#include <QDebug>
#include <QSqlQuery>
#include "QdOrmDml.h"
#include "Log/Log.h"
#include "ModuleBase/Database/Orm/QdOrmBuilder.h"

#define SQL_RECORD(success, ...) \
    if(success) { \
        SQL_RECORD_INFO(__VA_ARGS__); \
    }else { \
        SQL_RECORD_ERROR(__VA_ARGS__); \
    }

namespace Orm
{
    SQLRecordHelper::SQLRecordHelper(QSqlDatabase* database, QSqlQuery& qurey)
        : m_query(qurey)
        , m_database(database)
    {
    }

    SQLRecordHelper::~SQLRecordHelper()
    {
        /*QStringList boundValues;
        const auto& values = m_query.boundValues();
        for(auto itr = values.constBegin(); itr != values.constEnd(); ++itr)
        {
            boundValues.append(QStringLiteral("(%1, %2)").arg(itr.key()).arg(itr.value().toString()));
        }*/
      
        /*SQL_RECORD(m_success, "\n======>[FilePath]:{}\n======>[SQLRecord]:{}\n======>[BoundValues(key,value)]:{}",
            Base::CryptoUtil::ToBase64Encode(m_database->databaseName().toUtf8()),
            Base::CryptoUtil::ToBase64Encode(m_query.lastQuery().toUtf8()),
            Base::CryptoUtil::ToBase64Encode(boundValues.join(",").toUtf8()));*/
    }

    void SQLRecordHelper::SetStatus(bool success)
    {
        m_success = success;
    }

    QSqlError QdOrmDml::createTable(QSqlDatabase* database, const QdOrmClass* meta)
    {
        QString sql = QdOrmBuilder::createTable(database, meta);
        QSqlQuery query(*database);
        SQLRecordHelper helper(database, query);

        if(!query.exec(sql))
        {
            helper.SetStatus(false);
            return query.lastError();
        }

        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::dropTable(QSqlDatabase* database, const QdOrmClass* meta)
    {
        QString sql = QdOrmBuilder::dropTable(database, meta);
        QSqlQuery query(*database);
        SQLRecordHelper helper(database, query);

        if(!query.exec(sql))
        {
            helper.SetStatus(false);
            return query.lastError();
        }

        helper.SetStatus(true);
        return QSqlError();
    }

    bool QdOrmDml::exist(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QSqlQuery query = QdOrmBuilder::exist(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
            return false;
        }

        helper.SetStatus(true);
        return query.next();
    }

    QSqlError QdOrmDml::insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
       QSqlQuery query = QdOrmBuilder::insertRecord(database, meta, dataMap);
       SQLRecordHelper helper(database, query);
       if(!query.exec())
       {
           helper.SetStatus(false);
           return query.lastError();
       }

       helper.SetStatus(true);
       return QSqlError();
    }

    QSqlError QdOrmDml::insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        database->transaction();
        QSqlQuery query = QdOrmBuilder::insertRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if (!query.execBatch())
        {
            database->rollback();
            helper.SetStatus(false);
            return query.lastError();
        }

        database->commit();
        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QSqlQuery query = QdOrmBuilder::saveRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
            return query.lastError();
        }

        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        database->transaction();
        QSqlQuery query = QdOrmBuilder::saveRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if (!query.execBatch())
        {
            database->rollback();
            helper.SetStatus(false);
            return query.lastError();
        }

        database->commit();
        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QSqlQuery query = QdOrmBuilder::deleteRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
            return query.lastError();
        }

        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        database->transaction();
        QSqlQuery query = QdOrmBuilder::deleteRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if (!query.execBatch())
        {
            database->rollback();
            helper.SetStatus(false);
            return query.lastError();
        }

        database->commit();
        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::deleteRecord(database, meta, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
            return query.lastError();
        }

        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QSqlQuery query = QdOrmBuilder::updateRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
            return query.lastError();
        }

        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        database->transaction();
        QSqlQuery query = QdOrmBuilder::updateRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if (!query.execBatch())
        {
            database->rollback();
            helper.SetStatus(false);
            return query.lastError();
        }

        database->commit();
        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlError QdOrmDml::updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::updateRecord(database, meta, dataMap, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
            return query.lastError();
        }

        helper.SetStatus(true);
        return QSqlError();
    }

    QSqlQuery QdOrmDml::fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QSqlQuery query = QdOrmBuilder::fetchRecord(database, meta, dataMap);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
        }

        helper.SetStatus(true);
        return query;
    }

    QSqlQuery QdOrmDml::fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::fetchRecord(database, meta, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
        }

        helper.SetStatus(true);
        return query;
    }

    QSqlQuery QdOrmDml::countRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::countRecord(database, meta, column, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
        }

        helper.SetStatus(true);
        return query;
    }

    QSqlQuery QdOrmDml::sumRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::sumRecord(database, meta, column, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
        }

        helper.SetStatus(true);
        return query;
    }

    QSqlQuery QdOrmDml::avgRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::avgRecord(database, meta, column, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
        }

        helper.SetStatus(true);
        return query;
    }

    QSqlQuery QdOrmDml::minRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::minRecord(database, meta, column, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
        }

        helper.SetStatus(true);
        return query;
    }

    QSqlQuery QdOrmDml::maxRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        QSqlQuery query = QdOrmBuilder::maxRecord(database, meta, column, condition);
        SQLRecordHelper helper(database, query);
        if(!query.exec())
        {
            helper.SetStatus(false);
        }

        helper.SetStatus(true);
        return query;
    }
}