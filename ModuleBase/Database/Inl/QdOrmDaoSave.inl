#pragma once

#include <QVariantMap>
#include "ModuleBase/Database/Orm/QdOrmDml.h"
#include "ModuleBase/Database/Orm/QdOrmFactory.h"
#include "ModuleBase/Database/Orm/QdOrmCondition.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoSaveGeneric
    {
    public:
        static inline QSqlError save(T& t, QSqlDatabase* database)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoSaveGeneric::save()"
                , "database or meta is nullptr");

            QVariantMap dataMap;
            foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
            {
                QByteArray array = member->entityName().toLatin1();
                dataMap.insert(member->fieldName(), t.property(array));
            }
            return QdOrmDml::saveRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoSavePtr
    {
    public:
        static inline QSqlError save(T& t, QSqlDatabase* database)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*t)>::type>::type;
            return (t ? QdOrmDaoSaveGeneric<OriginalType>::save(*t, database) : QSqlError());
        }
    };

    template <class T>
    class QdOrmDaoSaveContainerGeneric
    {
    public:
        static inline QSqlError save(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoSaveContainerGeneric::save()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(t[0])>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoSaveContainerGeneric::save()"
                , "database or meta is nullptr");

            QMap<QString, QVariantList> dataMap;
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

            return QdOrmDml::saveRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoSaveContainerPtr
    {
    public:
        static inline QSqlError save(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoSaveContainerPtr::save()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*(t[0]))>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoSaveContainerPtr::save()"
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

            return QdOrmDml::saveRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoSaveContainer
    {
    public:
        static inline QSqlError save(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T::value_type>::value, QdOrmDaoSaveContainerPtr<T>, QdOrmDaoSaveContainerGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T::value_type>::value, QdOrmDaoSaveContainerPtr<T>, type_1>::type type_2;
            return type_2::save(t, database);
        }
    };

    template <class T>
    class QdOrmDaoSave
    {
    public:
        static inline QSqlError save(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoSavePtr<T>, QdOrmDaoSaveGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoSavePtr<T>, type_1>::type type_2;
            typedef typename std::conditional<is_container<T>::value, QdOrmDaoSaveContainer<T>, type_2>::type type_3;
            return type_3::save(t, database);
        }
    };
}