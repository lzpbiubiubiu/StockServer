#pragma once

#include <QSqlDatabase>
#include "ModuleBase/Database/Orm/QdOrmWhere.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoTable;

    template <class T>
    class QdOrmDaoInsertGeneric;

    template <class T>
    class QdOrmDaoInsertPtr;

    template <class T>
    class QdOrmDaoInsertContainerGeneric;

    template <class T>
    class QdOrmDaoInsertContainerPtr;

    template <class T>
    class QdOrmDaoInsertContainer;

    template <class T>
    class QdOrmDaoInsert;

    template <class T>
    class QdOrmDaoSaveGeneric;

    template <class T>
    class QdOrmDaoSavePtr;

    template <class T>
    class QdOrmDaoSaveContainerGeneric;

    template <class T>
    class QdOrmDaoSaveContainerPtr;

    template <class T>
    class QdOrmDaoSaveContainer;

    template <class T>
    class QdOrmDaoSave;

    template <class T>
    class QdOrmDaoFetchGeneric;

    template <class T>
    class QdOrmDaoFetchPtr;

    template <class T>
    class QdOrmDaoFetchContainerGeneric;

    template <class T>
    class QdOrmDaoFetchContainerPtr;

    template <class T>
    class QdOrmDaoFetchContainerSmartPtr;

    template <class T>
    class QdOrmDaoFetchContainer;

    template <class T>
    class QdOrmDaoFetch;

    template <class T>
    class QdOrmDaoDeleteGeneric;

    template <class T>
    class QdOrmDaoDeletePtr;

    template <class T>
    class QdOrmDaoDeleteContainerGeneric;

    template <class T>
    class QdOrmDaoDeleteContainerPtr;

    template <class T>
    class QdOrmDaoDeleteContainer;

    template <class T>
    class QdOrmDaoDelete;

    template <class T>
    class QdOrmDaoUpdateGeneric;

    template <class T>
    class QdOrmDaoUpdatePtr;

    template <class T>
    class QdOrmDaoUpdateContainerGeneric;

    template <class T>
    class QdOrmDaoUpdateContainerPtr;

    template <class T>
    class QdOrmDaoUpdateContainer;

    template <class T>
    class QdOrmDaoUpdate;

    template <class T>
    class QdOrmDaoAggregate;
}

namespace Orm
{
    /** 创建数据库表 */
    template <class T>
    inline QSqlError create_table(QSqlDatabase* database)
    {
        return Orm::QdOrmDaoTable<T>::createTable(database);
    }

    /** 删除数据库表 */
    template <class T>
    inline QSqlError drop_table(QSqlDatabase* database)
    {
        return Orm::QdOrmDaoTable<T>::dropTable(database);
    }

    /** 插入数据 */
    template <class T>
    inline QSqlError insert(T& t, QSqlDatabase* database)
    {
        return Orm::QdOrmDaoInsert<T>::insert(t, database);
    }

    /** 保存数据 */
    template <class T>
    inline QSqlError save(T& t, QSqlDatabase* database)
    {
        return Orm::QdOrmDaoSave<T>::save(t, database);
    }

    /** 删除数据（主键匹配） */
    template <class T>
    inline QSqlError delete_by_id(T& t, QSqlDatabase* database)
    {
        return Orm::QdOrmDaoDelete<T>::deleteById(t, database);
    }

    /** 删除全部数据(可添加查询条件) */
    template <class T>
    inline QSqlError delete_by_query(QSqlDatabase* database, const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoDelete<T>::deleteByQuery(database, condition);
    }

    /** 更新数据（主键匹配） */
    template <class T>
    inline QSqlError update_by_id(T& t, QSqlDatabase* database, const QStringList& columns = QStringList())
    {
        return Orm::QdOrmDaoUpdate<T>::updateById(t, database, columns);
    }

    /** 更新数据（可添加查询条件） */
    template <class T>
    inline QSqlError update_by_query(T& t, QSqlDatabase* database, const QStringList& columns = QStringList(), const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoUpdate<T>::updateByQuery(t, database, columns, condition);
    }

    /** 查找（主键匹配） */
    template <class T>
    inline bool exist(T& t, QSqlDatabase* database)
    {
        return Orm::QdOrmDaoFetch<T>::exist(t, database);
    }

    /** 获取数据（主键匹配） */
    template <class T>
    inline QSqlError fetch_by_id(T& t, QSqlDatabase* database)
    {
        return Orm::QdOrmDaoFetch<T>::fetchById(t, database);
    }

    /** 获取全部数据(可添加查询条件) */
    template <class T>
    inline QSqlError fetch_by_query(T& t, QSqlDatabase* database, const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoFetch<T>::fetchByQuery(t, database, condition);
    }

    /** 统计(可添加查询条件) */
    template <class T>
    inline QSqlError count(long long& size, QSqlDatabase* database, const QString& column = QString(), const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoAggregate<T>::count(size, database, column, condition);
    }

    /** 求和(可添加查询条件) */
    template <class T>
    inline QSqlError sum(QVariant& sum, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoAggregate<T>::sum(sum, database, column, condition);
    }

    /** 平均值(可添加查询条件) */
    template <class T>
    inline QSqlError avg(QVariant& avg, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoAggregate<T>::avg(avg, database, column, condition);
    }

    /** 最大值(可添加查询条件) */
    template <class T>
    inline QSqlError max(QVariant& max, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoAggregate<T>::max(max, database, column, condition);
    }

    /** 最小值(可添加查询条件) */
    template <class T>
    inline QSqlError min(QVariant& min, QSqlDatabase* database, const QString& column, const QdOrmWhere& condition = QdOrmWhere())
    {
        return Orm::QdOrmDaoAggregate<T>::min(min, database, column, condition);
    }
}

#include "ModuleBase/Database/Inl/QdOrmDaoTable.inl"
#include "ModuleBase/Database/Inl/QdOrmDaoInsert.inl"
#include "ModuleBase/Database/Inl/QdOrmDaoSave.inl"
#include "ModuleBase/Database/Inl/QdOrmDaoFetch.inl"
#include "ModuleBase/Database/Inl/QdOrmDaoDelete.inl"
#include "ModuleBase/Database/Inl/QdOrmDaoUpdate.inl"
#include "ModuleBase/Database/Inl/QdOrmDaoAggregate.inl"