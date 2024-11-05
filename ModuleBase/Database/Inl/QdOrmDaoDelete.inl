#pragma once

#include <QVariantMap>
#include "ModuleBase/Database/Orm/QdOrmDml.h"
#include "ModuleBase/Database/Orm/QdOrmFactory.h"
#include "ModuleBase/Database/Orm/QdOrmCondition.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoDeleteGeneric
    {
    public:
        static inline QSqlError deleteById(T& t, QSqlDatabase* database)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoDeleteGeneric::deleteById()"
                , "database or meta is nullptr");

            QVariantMap dataMap;
            foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
            {
                if(member->isPrimaryKey())
                {
                    QByteArray array = member->entityName().toLatin1();
                    dataMap.insert(member->fieldName(), t.property(array));
                    break;
                }
            }
            return QdOrmDml::deleteRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoDeletePtr
    {
    public:
        static inline QSqlError deleteById(T& t, QSqlDatabase* database)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*t)>::type>::type;
            return (t ? QdOrmDaoDeleteGeneric<OriginalType>::deleteById(*t, database) : QSqlError());
        }
    };

    template <class T>
    class QdOrmDaoDeleteContainerGeneric
    {
    public:
        static inline QSqlError deleteById(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoDeleteContainerGeneric::deleteById()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(t[0])>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoDeleteContainerGeneric::deleteById()"
                , "database or meta is nullptr");

            QMap<QString, QVariantList> dataMap;
            foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
            {
                if (member->isPrimaryKey())
                {
                    QVariantList dataList;
                    QByteArray array = member->entityName().toLatin1();
                    for (typename T::iterator it = t.begin(); it != t.end(); ++it)
                    {
                        dataList << it->property(array);
                    }
                    dataMap.insert(member->fieldName(), dataList);
                    break;
                }
            }

            return QdOrmDml::deleteRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoDeleteContainerPtr
    {
    public:
        static inline QSqlError deleteById(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoDeleteContainerPtr::deleteById()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*(t[0]))>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoDeleteContainerPtr::deleteById()"
                , "database or meta is nullptr");

            QMap<QString, QVariantList> dataMap;
            foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
            {
                if (member->isPrimaryKey())
                {
                    QVariantList dataList;
                    QByteArray array = member->entityName().toLatin1();
                    for (typename T::iterator it = t.begin(); it != t.end(); ++it)
                    {
                        dataList << (*it)->property(array);
                    }
                    dataMap.insert(member->fieldName(), dataList);
                    break;
                }
            }

            return QdOrmDml::deleteRecord(database, meta, dataMap);
        }
    };

    template <class T>
    class QdOrmDaoDeleteContainer
    {
    public:
        static inline QSqlError deleteById(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T::value_type>::value, QdOrmDaoDeleteContainerPtr<T>, QdOrmDaoDeleteContainerGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T::value_type> ::value, QdOrmDaoDeleteContainerPtr<T>, type_1> ::type type_2;
            return type_2::deleteById(t, database);
        }
    };

    template <class T>
    class QdOrmDaoDelete
    {
    public:
        static inline QSqlError deleteById(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoDeletePtr<T>, QdOrmDaoDeleteGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoDeletePtr<T>, type_1>::type type_2;
            typedef typename std::conditional<is_container<T>::value, QdOrmDaoDeleteContainer<T>, type_2>::type type_3;
            return type_3::deleteById(t, database);
        }

        static inline QSqlError deleteByQuery(QSqlDatabase* database, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoDelete::deleteByQuery()"
                , "database or meta is nullptr");
            return QdOrmDml::deleteRecord(database, meta, condition);
        }
    };
}