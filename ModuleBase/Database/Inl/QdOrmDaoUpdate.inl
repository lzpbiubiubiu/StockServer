#pragma once

#include <QVariantMap>
#include "ModuleBase/Database/Orm/QdOrmDml.h"
#include "ModuleBase/Database/Orm/QdOrmFactory.h"
#include "ModuleBase/Database/Orm/QdOrmCondition.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoUpdateGeneric
    {
    public:
        static inline QSqlError updateById(T& t, QSqlDatabase* database, const QStringList& columns)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoUpdateGeneric::updateById()"
                , "database or meta is nullptr");

            QVariantMap dataMap;
            if(columns.size() > 0)
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    if(meta->primaryKey() == member->fieldName() || columns.contains(member->fieldName()))
                    {
                        QByteArray array = member->entityName().toLatin1();
                        dataMap.insert(member->fieldName(), t.property(array));
                    }
                }
            }
            else
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    QByteArray array = member->entityName().toLatin1();
                    dataMap.insert(member->fieldName(), t.property(array));
                }
            }

            return QdOrmDml::updateRecord(database, meta, dataMap);
        }

        static inline QSqlError updateByQuery(T& t, QSqlDatabase* database, const QStringList& columns, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoUpdateGeneric::updateByQuery()"
                , "database or meta is nullptr");

            QVariantMap dataMap;
            if (columns.size() > 0)
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    if (columns.contains(member->fieldName()))
                    {
                        QByteArray array = member->entityName().toLatin1();
                        dataMap.insert(member->fieldName(), t.property(array));
                    }
                }
            }
            else
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    QByteArray array = member->entityName().toLatin1();
                    dataMap.insert(member->fieldName(), t.property(array));
                }
            }

            return QdOrmDml::updateRecord(database, meta, dataMap, condition);
        }
    };

    template <class T>
    class QdOrmDaoUpdatePtr
    {
    public:
        static inline QSqlError updateById(T& t, QSqlDatabase* database, const QStringList& columns)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*t)>::type>::type;
            return (t ? QdOrmDaoUpdateGeneric<OriginalType>::updateById(*t, database, columns) : QSqlError());
        }

        static inline QSqlError updateByQuery(T& t, QSqlDatabase* database, const QStringList& columns, const QdOrmWhere& condition)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*t)>::type>::type;
            return (t ? QdOrmDaoUpdateGeneric<OriginalType>::updateByQuery(*t, database, columns, condition) : QSqlError());
        }
    };

    template <class T>
    class QdOrmDaoUpdateContainerGeneric
    {
    public:
        static inline QSqlError updateById(T& t, QSqlDatabase* database, const QStringList& columns)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoUpdateContainerGeneric::updateById()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(t[0])>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoUpdateContainerGeneric::updateById()"
                , "database or meta is nullptr");

            QMap<QString, QVariantList> dataMap;
            if (columns.size() > 0)
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    if (meta->primaryKey() == member->fieldName() || columns.contains(member->fieldName()))
                    {
                        QVariantList dataList;
                        QByteArray array = member->entityName().toLatin1();
                        for (typename T::iterator it = t.begin(); it != t.end(); ++it)
                        {
                            dataList << it->property(array);
                        }
                        dataMap.insert(member->fieldName(), dataList);
                    }
                }
            }
            else
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    QVariantList dataList;
                    QByteArray array = member->entityName().toLatin1();
                    for (typename T::iterator it = t.begin(); it != t.end(); ++it)
                    {
                        dataList << it->property(array);
                    }
                    dataMap.insert(member->fieldName(), dataList);
                }
            }

            return QdOrmDml::updateRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoUpdateContainerPtr
    {
    public:
        static inline QSqlError updateById(T& t, QSqlDatabase* database, const QStringList& columns)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoUpdateContainerPtr::updateById()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*(t[0]))>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoUpdateContainerPtr::updateById()"
                , "database or meta is nullptr");

            QMap<QString, QVariantList> dataMap;
            if (columns.size() > 0)
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    if (meta->primaryKey() == member->fieldName() || columns.contains(member->fieldName()))
                    {
                        QVariantList dataList;
                        QByteArray array = member->entityName().toLatin1();
                        for (typename T::iterator it = t.begin(); it != t.end(); ++it)
                        {
                            dataList << (*it)->property(array);
                        }
                        dataMap.insert(member->fieldName(), dataList);
                    }
                }
            }
            else
            {
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    QVariantList dataList;
                    QByteArray array = member->entityName().toLatin1();
                    for (typename T::iterator it = t.begin(); it != t.end(); ++it)
                    {
                        dataList << (*it)->property(array);
                    }
                    dataMap.insert(member->fieldName(), dataList);
                }
            }

            return QdOrmDml::updateRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoUpdateContainer
    {
    public:
        static inline QSqlError updateById(T& t, QSqlDatabase* database, const QStringList& columns)
        {
            typedef typename std::conditional <std::is_pointer <T::value_type>::value, QdOrmDaoUpdateContainerPtr<T>, QdOrmDaoUpdateContainerGeneric<T >> ::type type_1;
            typedef typename std::conditional<is_smart_ptr<T::value_type>::value, QdOrmDaoUpdateContainerPtr<T>, type_1>::type type_2;
            return type_2::updateById(t, database, columns);
        }
    };

    template <class T>
    class QdOrmDaoUpdate
    {
    public:
        static inline QSqlError updateById(T& t, QSqlDatabase* database, const QStringList& columns)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoUpdatePtr<T>, QdOrmDaoUpdateGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoUpdatePtr<T>, type_1>::type type_2;
            typedef typename std::conditional<is_container<T>::value, QdOrmDaoUpdateContainer<T>, type_2>::type type_3;
            return type_3::updateById(t, database, columns);
        }

        static inline QSqlError updateByQuery(T& t, QSqlDatabase* database, const QStringList& columns, const QdOrmWhere& condition)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoUpdatePtr<T>, QdOrmDaoUpdateGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoUpdatePtr<T>, type_1>::type type_2;
            return type_2::updateByQuery(t, database, columns, condition);
        }
    };
}