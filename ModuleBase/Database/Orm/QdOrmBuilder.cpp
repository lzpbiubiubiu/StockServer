#include <QSqlDriver>
#include "QdOrmBuilder.h"
#include "ModuleBase/Database/Orm/QdOrmDefine.h"

namespace Orm
{
    QString QdOrmBuilder::createTable(QSqlDatabase* database, const QdOrmClass* meta)
    {
        QStringList fieldList;
        for(const auto& member : meta->ormMetaMembers())
        {
            QString temp = member->fieldName() + " " + member->fieldSqlForm();
            if(member->isNotNull())
                temp += " NOT NULL";
            if(member->isPrimaryKey())
                temp += " NOT NULL PRIMARY KEY";
            if(member->isUnique())
                temp += " UNIQUE";
            fieldList << temp;
        }

        QString sql = create_table_sql
            .arg(meta->tableName())
            .arg(fieldList.join(QLatin1String(", ")));
        return sql;
    }

    QString QdOrmBuilder::dropTable(QSqlDatabase* database, const QdOrmClass* meta)
    {
        QString sql = drop_table_sql.arg(meta->tableName());
        return sql;
    }

    QSqlQuery QdOrmBuilder::exist(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QStringList whereColumns;
        foreach(const QString & name, dataMap.keys())
            whereColumns << name + QString(" = :" + name);

        QString sql = exist_sql.arg(meta->tableName())
            .arg(whereColumns.join(QLatin1String(" AND ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const QString & name, dataMap.keys())
            query.bindValue(QString(":" + name), dataMap.value(name));
        return query;
    }

    QSqlQuery QdOrmBuilder::insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QStringList fieldColumns;
        QStringList fieldHolders;
        foreach(const QString & name, dataMap.keys())
        {
            fieldColumns << name;
            fieldHolders << QString(":" + name);
        }

        QString sql = insert_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(", "))).arg(fieldHolders.join(QLatin1String(", ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const QString & name, dataMap.keys())
            query.bindValue(QString(":" + name), dataMap.value(name));

        return query;
    }

    QSqlQuery QdOrmBuilder::insertRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        QStringList fieldColumns;
        QStringList fieldHolders;
        foreach(const QString & name, dataMap.keys())
        {
            fieldColumns << name;
            fieldHolders << QString(":" + name);
        }

        QString sql = insert_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(", "))).arg(fieldHolders.join(QLatin1String(", ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const QString & name, dataMap.keys())
            query.bindValue(QString(":" + name), dataMap.value(name));

        return query;
    }

    QSqlQuery QdOrmBuilder::saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QStringList fieldColumns;
        QStringList fieldHolders;
        foreach(const QString & name, dataMap.keys())
        {
            fieldColumns << name;
            fieldHolders << QString(":" + name);
        }

        QString sql = save_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(", "))).arg(fieldHolders.join(QLatin1String(", ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const QString & name, dataMap.keys())
            query.bindValue(QString(":" + name), dataMap.value(name));

        return query;
    }

    QSqlQuery QdOrmBuilder::saveRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        QStringList fieldColumns;
        QStringList fieldHolders;
        foreach(const QString & name, dataMap.keys())
        {
            fieldColumns << name;
            fieldHolders << QString(":" + name);
        }

        QString sql = save_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(", "))).arg(fieldHolders.join(QLatin1String(", ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const QString & name, dataMap.keys())
            query.bindValue(QString(":" + name), dataMap.value(name));

        return query;
    }

    QSqlQuery QdOrmBuilder::deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QStringList fieldColumns;
        foreach(const QString & name, dataMap.keys())
        {
            fieldColumns << name + QString(" = :" + name);
        }

        QString sql = delete_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(" AND ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const QString & name, dataMap.keys())
            query.bindValue(QString(":" + name), dataMap.value(name));

        return query;
    }

    QSqlQuery QdOrmBuilder::deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        QStringList fieldColumns;
        foreach(const QString & name, dataMap.keys())
        {
            fieldColumns << name + QString(" = :" + name);
        }

        QString sql = delete_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(" AND ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const QString & name, dataMap.keys())
            query.bindValue(QString(":" + name), dataMap.value(name));

        return query;
    }

    QSqlQuery QdOrmBuilder::deleteRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition)
    {
        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        QString sql = delete_all_sql.arg(meta->tableName())
            .arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QStringList fieldColumns;
        QStringList whereColumns;
        QVariantList fieldValues;
        QVariantList whereValues;
        foreach(const QString & name, dataMap.keys())
        {
            if(meta->primaryKey() != name)
            {
                fieldColumns << name + QString(" = :" + name);
                fieldValues << dataMap.value(name);
            }
            else
            {
                whereColumns << name + QString(" = :" + name);
                whereValues << dataMap.value(name);
            }
        }

        QString sql = update_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(", "))).arg(whereColumns.join(QLatin1String(" AND ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto& value, fieldValues)
        {
            query.addBindValue(value);
        }

        foreach(const auto& value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QMap<QString, QVariantList>& dataMap)
    {
        QStringList fieldColumns;
        QStringList whereColumns;
        QList<QVariantList> fieldValues;
        QList<QVariantList> whereValues;
        foreach(const QString & name, dataMap.keys())
        {
            if (meta->primaryKey() != name)
            {
                fieldColumns << name + QString(" = :" + name);
                fieldValues << dataMap.value(name);
            }
            else
            {
                whereColumns << name + QString(" = :" + name);
                whereValues << dataMap.value(name);
            }
        }

        QString sql = update_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(", "))).arg(whereColumns.join(QLatin1String(" AND ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, fieldValues)
        {
            query.addBindValue(value);
        }

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::updateRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap, const QdOrmWhere& condition)
    {
        QStringList fieldColumns;
        QVariantList fieldValues;
        foreach(const QString & name, dataMap.keys())
        {
            fieldColumns << name + QString(" = :" + name);
            fieldValues << dataMap.value(name);
        }

        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        QString sql = update_all_sql.arg(meta->tableName())
            .arg(fieldColumns.join(QLatin1String(", "))).arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, fieldValues)
        {
            query.addBindValue(value);
        }

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QVariantMap& dataMap)
    {
        QStringList fieldColumns;
        QStringList whereColumns;
        QVariantList whereValues;
        foreach(const QString & name, dataMap.keys())
        {
            if(meta->primaryKey() == name)
            {
                whereColumns << name + QString(" = :" + name);
                whereValues << dataMap.value(name);
            }
            fieldColumns << name;
        }

        QString sql = fetch_sql.arg(fieldColumns.join(QLatin1String(", ")))
            .arg(meta->tableName())
            .arg(whereColumns.join(QLatin1String(" AND ")));

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::fetchRecord(QSqlDatabase* database, const QdOrmClass* meta, const QdOrmWhere& condition)
    {
        // 数据列
        QStringList fieldColumns;
        foreach(const auto& members, meta->ormMetaMembers())
        {
            fieldColumns << members->fieldName();
        }

        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        // order by
        QStringList orderByColumns;
        for(const auto& item : condition.getOrmOrderByItems())
        {
            if(item->flag == QdOrmOrderByItem::ASC)
                orderByColumns << (item->cloumns.join(QLatin1String(", ")) + " ASC");
            else
                orderByColumns << (item->cloumns.join(QLatin1String(", ")) + " DESC");
        }

        if(orderByColumns.size() > 0)
            conditionSql += " ORDER BY " + orderByColumns.join(QLatin1String(", "));

        // limit
        const QVariantList& limitItems = condition.getLimitItems();
        if(2 == limitItems.size())
        {
            conditionSql += QString(" LIMIT %1 OFFSET %2")
                .arg(limitItems.at(0).toInt()).arg(limitItems.at(1).toInt());
        }

        QString sql = fetch_all_sql.arg(fieldColumns.join(QLatin1String(", ")))
            .arg(meta->tableName())
            .arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto& value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::countRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        QString sql = count_all_sql.arg(column).arg(meta->tableName())
            .arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::sumRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        QString sql = sum_all_sql.arg(column).arg(meta->tableName())
            .arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::avgRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        QString sql = avg_all_sql.arg(column).arg(meta->tableName())
            .arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::maxRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        QString sql = max_all_sql.arg(column).arg(meta->tableName())
            .arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QSqlQuery QdOrmBuilder::minRecord(QSqlDatabase* database, const QdOrmClass* meta, const QString& column, const QdOrmWhere& condition)
    {
        // 查询条件sql语句
        QString conditionSql;

        // where
        QStringList whereColumns;
        QVariantList whereValues;
        for(const auto& item : condition.getOrmWhereItems())
        {
            QString temp = operateSql(item);
            temp += item->cloumn;
            temp += compareSql(item);
            whereColumns << temp;

            if(0 != item->value.size())
                whereValues << item->value;
        }

        if(whereColumns.size() > 0)
            conditionSql += whereColumns.join(QLatin1String(""));

        QString sql = min_all_sql.arg(column).arg(meta->tableName())
            .arg(conditionSql);

        QSqlQuery query(*database);
        query.prepare(sql);

        foreach(const auto & value, whereValues)
        {
            query.addBindValue(value);
        }

        return query;
    }

    QString QdOrmBuilder::operateSql(const QdOrmWhereItemPtr& item)
    {
        QString sql;
        if(item->oMode == QdOrmWhereItem::ORM_OM_WHERE)
            sql += " WHERE ";
        else if(item->oMode == QdOrmWhereItem::ORM_OM_AND)
            sql += " AND ";
        else if(item->oMode == QdOrmWhereItem::ORM_OM_AND_OPEN_PARENTHESIS)
            sql += " AND (";
        else if(item->oMode == QdOrmWhereItem::ORM_OM_OR)
            sql += " OR ";
        else if(item->oMode == QdOrmWhereItem::ORM_OM_OR_OPEN_PARENTHESIS)
            sql += " OR (";
        else if(item->oMode == QdOrmWhereItem::ORM_OM_OPEN_PARENTHESIS)
            sql += "(";
        else if(item->oMode == QdOrmWhereItem::ORM_OM_CLOSE_PARENTHESIS)
            sql += ")";
        else
            sql += "";
        return sql;
    }

    QString QdOrmBuilder::compareSql(const QdOrmWhereItemPtr& item)
    {
        QString sql;
        if(item->cMode == QdOrmWhereItem::ORM_CM_EQUAL)
            sql += " = ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_NOT_EQUAL)
            sql += " != ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_GREATER)
            sql += " > ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_GREATER_OR_EQUAL)
            sql += " >= ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_LESS)
            sql += " < ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_LESS_OR_EQUAL)
            sql += " <= ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_NULL)
            sql += " IS NULL";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_NOT_NULL)
            sql += " IS NOT NULL";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_BETWEEN)
            sql += " BETWEEN ? AND ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_NOT_BETWEEN)
            sql += " NOT BETWEEN ? AND ?";
        else if(item->cMode == QdOrmWhereItem::ORM_CM_IN)
        {
            QStringList bits;
            for(int i = 0; i < item->value.size(); i++)
                bits << QLatin1String("?");
            sql += QString::fromLatin1(" IN (%1)").arg(bits.join(QLatin1String(", ")));
        }
        else if(item->cMode == QdOrmWhereItem::ORM_CM_NOT_IN)
        {
            QStringList bits;
            for(int i = 0; i < item->value.size(); i++)
                bits << QLatin1String("?");
            sql += QString::fromLatin1(" NOT IN (%1)").arg(bits.join(QLatin1String(", ")));
        }
        else
            sql += "";
        return sql;
    }
}