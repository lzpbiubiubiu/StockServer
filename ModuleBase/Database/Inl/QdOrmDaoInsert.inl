#pragma once

#include <QVariantMap>
#include "ModuleBase/Database/Orm/QdOrmDml.h"
#include "ModuleBase/Database/Orm/QdOrmFactory.h"
#include "ModuleBase/Database/Orm/QdOrmCondition.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoInsertGeneric
    {
    public:
        static inline QSqlError insert(T& t, QSqlDatabase* database)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoInsertGeneric::insert()"
                , "database or meta is nullptr");

            QVariantMap dataMap;
            foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
            {
                QByteArray array = member->entityName().toLatin1();
                dataMap.insert(member->fieldName(), t.property(array));
            }
            return QdOrmDml::insertRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoInsertPtr
    {
    public:
        static inline QSqlError insert(T& t, QSqlDatabase* database)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*t)>::type>::type;
            return (t ? QdOrmDaoInsertGeneric<OriginalType>::insert(*t, database) : QSqlError());
        }
    };

    template <class T>
    class QdOrmDaoInsertContainerGeneric
    {
    public:
        static inline QSqlError insert(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoInsertContainerGeneric::insert()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(t[0])>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoInsertContainerGeneric::insert()"
                , "database or meta is nullptr");

            QMap<QString, QVariantList> dataMap;
            foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
            {
                QVariantList dataList;
                QByteArray array = member->entityName().toLatin1();
                for(typename T::iterator it = t.begin(); it != t.end(); ++it)
                {
                    dataList << it->property(array);
                }
                dataMap.insert(member->fieldName(), dataList);
            }

            return QdOrmDml::insertRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoInsertContainerPtr
    {
    public:
        static inline QSqlError insert(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoInsertContainerPtr::insert()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*(t[0]))>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoInsertContainerPtr::insert()"
                , "database or meta is nullptr");

            QMap<QString, QVariantList> dataMap;
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

            return QdOrmDml::insertRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoInsertContainer
    {
    public:
        static inline QSqlError insert(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T::value_type>::value, QdOrmDaoInsertContainerPtr<T>, QdOrmDaoInsertContainerGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T::value_type>::value, QdOrmDaoInsertContainerPtr<T>, type_1>::type type_2;
            return type_2::insert(t, database);
        }
    };

    template <class T>
    class QdOrmDaoInsert
    {
    public:
        static inline QSqlError insert(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoInsertPtr<T>, QdOrmDaoInsertGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoInsertPtr<T>, type_1>::type type_2;
            typedef typename std::conditional<is_container<T>::value, QdOrmDaoInsertContainer<T>, type_2>::type type_3;
            return type_3::insert(t, database);
        }
    };
}