#pragma once

#include <QVariantMap>
#include "ModuleBase/Database/Orm/QdOrmDml.h"
#include "ModuleBase/Database/Orm/QdOrmFactory.h"
#include "ModuleBase/Database/Orm/QdOrmCondition.h"

namespace Orm
{
    template <class T>
    class QdOrmDaoFetchGeneric
    {
    public:
        static inline bool exist(T& t, QSqlDatabase* database)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT(database != nullptr && meta != nullptr);

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
            return QdOrmDml::exist(database, meta, dataMap);
        }

        static inline QSqlError fetchById(T& t, QSqlDatabase* database)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoFetchGeneric::fetchById()"
                , "database or meta is nullptr");

            QVariantMap dataMap;
            foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
            {
                QByteArray array = member->entityName().toLatin1();
                dataMap.insert(member->fieldName(), t.property(array));
            }

            QSqlQuery query = QdOrmDml::fetchRecord(database, meta, dataMap);
            QSqlError error = query.lastError();
            if(error.isValid())
            {
                return error;
            }

            if(!query.next())
            {
                return QSqlError("[Orm] internal error", "[Orm] sql query returns no data", QSqlError::UnknownError);
            }

            foreach(const OrmMetaMemberPtr& member, meta->ormMetaMembers())
            {
                QByteArray array = member->entityName().toLatin1();
                t.setProperty(array, query.value(member->fieldName()));
            }

            return error;
        }

        static inline QSqlError fetchByQuery(T& t, QSqlDatabase* database, const QdOrmWhere& condition)
        {
            Q_ASSERT_X(1 != 1, "QdOrmDaoFetchGeneric::fetchByQuery()"
                , "not support template function");
            return QSqlError();
        }
    };

    template <class T>
    class QdOrmDaoFetchPtr
    {
    public:
        static inline bool exist(T& t, QSqlDatabase* database)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*t)>::type>::type;
            return (t ? QdOrmDaoFetchGeneric<OriginalType>::exist(*t, database) : false);
        }

        static inline QSqlError fetchById(T& t, QSqlDatabase* database)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(*t)>::type>::type;
            return (t ? QdOrmDaoFetchGeneric<OriginalType>::fetchById(*t, database) : QSqlError());
        }

        static inline QSqlError fetchByQuery(T& t, QSqlDatabase* database, const QdOrmWhere& condition)
        {
            Q_ASSERT_X(1 != 1, "QdOrmDaoFetchPtr::fetchByQuery()"
                , "not support template function");
            return QSqlError();
        }
    };

    template <class T>
    class QdOrmDaoFetchContainerGeneric
    {
    public:
        static inline bool exist(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoFetchContainerGeneric::exist()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(t[0])>::type>::type;
            for (typename T::iterator it = t.begin(); it != t.end(); ++it)
            {
                bool exist = QdOrmDaoFetchGeneric<OriginalType>::exist((*it), database);
                if (!exist)
                {
                    return false;
                }
            }
            return true;
        }

        static inline QSqlError fetchById(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoFetchContainerGeneric::fetchById()", "t is empty");
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<decltype(t[0])>::type>::type;
            for (typename T::iterator it = t.begin(); it != t.end(); ++it)
            {
                QSqlError error = QdOrmDaoFetchGeneric<OriginalType>::fetchById((*it), database);
                if (error.isValid())
                {
                    return error;
                }
            }
            return QSqlError();
        }

        static inline QSqlError fetchByQuery(T& t, QSqlDatabase* database, const QdOrmWhere& condition)
        {
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(T::value_type::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoFetchContainerGeneric::fetchByQuery()"
                , "database or meta is nullptr");
            t.clear();

            QSqlQuery query = QdOrmDml::fetchRecord(database, meta, condition);
            QSqlError error = query.lastError();
            if (error.isValid())
            {
                return error;
            }

            while(query.next())
            {
                typename T::value_type row;
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    QByteArray array = member->entityName().toLatin1();
                    row.setProperty(array, query.value(member->fieldName()));
                }
                t << row;
            }
            return error;
        }
    };

    template <class T>
    class QdOrmDaoFetchContainerSmartPtr
    {
    public:
        static inline QSqlError fetchByQuery(T& t, QSqlDatabase* database, const QdOrmWhere& condition)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<T::value_type::value_type>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoFetchContainerPtr::fetchByQuery()"
                , "database or meta is nullptr");
            t.clear();

            QSqlQuery query = QdOrmDml::fetchRecord(database, meta, condition);
            QSqlError error = query.lastError();
            if (error.isValid())
            {
                return error;
            }

            while (query.next())
            {
                typename T::value_type row = T::value_type::create();
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    QByteArray array = member->entityName().toLatin1();
                    row->setProperty(array, query.value(member->fieldName()));
                }
                t << row;
            }

            return QSqlError();
        }
    };

    template <class T>
    class QdOrmDaoFetchContainerPtr
    {
    public:
        static inline bool exist(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoFetchContainerPtr::exist()", "t is empty");
            using OriginalType = typename std::remove_reference<decltype(t[0])>::type;
            for (typename T::iterator it = t.begin(); it != t.end(); ++it)
            {
                bool exist = QdOrmDaoFetchPtr<OriginalType>::exist((*it), database);
                if (!exist)
                {
                    return false;
                }
            }
            return true;
        }

        static inline QSqlError fetchById(T& t, QSqlDatabase* database)
        {
            Q_ASSERT_X(!t.isEmpty(), "QdOrmDaoFetchContainerPtr::fetchById()", "t is empty");
            using OriginalType = typename std::remove_reference<decltype(t[0])>::type;
            for (typename T::iterator it = t.begin(); it != t.end(); ++it)
            {
                QSqlError error = QdOrmDaoFetchPtr<OriginalType>::fetchById((*it), database);
                if (error.isValid())
                {
                    return error;
                }
            }
            return QSqlError();
        }

        static inline QSqlError fetchByQuery(T& t, QSqlDatabase* database, const QdOrmWhere& condition)
        {
            using OriginalType = typename std::remove_reference<typename std::remove_pointer<T::value_type>::type>::type;
            const QdOrmClass* meta = QdOrmFactory::Get()->getAppointClass(OriginalType::GetTableName());
            Q_ASSERT_X(database != nullptr && meta != nullptr, "QdOrmDaoFetchContainerPtr::fetchByQuery()"
                , "database or meta is nullptr");
            t.clear();

            QSqlQuery query = QdOrmDml::fetchRecord(database, meta, condition);
            QSqlError error = query.lastError();
            if (error.isValid())
            {
                return error;
            }

            while (query.next())
            {
                OriginalType* row = new OriginalType;
                foreach(const OrmMetaMemberPtr & member, meta->ormMetaMembers())
                {
                    QByteArray array = member->entityName().toLatin1();
                    row->setProperty(array, query.value(member->fieldName()));
                }
                t << row;
            }

            return error;
        }
    };

    template <class T>
    class QdOrmDaoFetchContainer
    {
    public:
        static inline bool exist(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T::value_type>::value, QdOrmDaoFetchContainerPtr<T>, QdOrmDaoFetchContainerGeneric<T>> ::type type_1;
            typedef typename std::conditional<is_smart_ptr<T::value_type>::value, QdOrmDaoFetchContainerPtr<T>, type_1>::type type_2;
            return type_2::exist(t, database);
        }

        static inline QSqlError fetchById(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T::value_type>::value, QdOrmDaoFetchContainerPtr<T>, QdOrmDaoFetchContainerGeneric<T>> ::type type_1;
            typedef typename std::conditional<is_smart_ptr<T::value_type>::value, QdOrmDaoFetchContainerPtr<T>, type_1>::type type_2;
            return type_2::fetchById(t, database);
        }

        static inline QSqlError fetchByQuery(T& t, QSqlDatabase* database, const QdOrmWhere& condition)
        {
            typedef typename std::conditional<std::is_pointer<T::value_type>::value, QdOrmDaoFetchContainerPtr<T>, QdOrmDaoFetchContainerGeneric<T>> ::type type_1;
            typedef typename std::conditional<is_smart_ptr<T::value_type>::value, QdOrmDaoFetchContainerSmartPtr<T>, type_1>::type type_2;
            return type_2::fetchByQuery(t, database, condition);
        }
    };

    template <class T>
    class QdOrmDaoFetch
    {
    public:
        static inline bool exist(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoFetchPtr<T>, QdOrmDaoFetchGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoFetchPtr<T>, type_1>::type type_2;
            typedef typename std::conditional<is_container<T>::value, QdOrmDaoFetchContainer<T>, type_2>::type type_3;
            return type_3::exist(t, database);
        }

        static inline QSqlError fetchById(T& t, QSqlDatabase* database)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoFetchPtr<T>, QdOrmDaoFetchGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoFetchPtr<T>, type_1>::type type_2;
            typedef typename std::conditional<is_container<T>::value, QdOrmDaoFetchContainer<T>, type_2>::type type_3;
            return type_3::fetchById(t, database);
        }

        static inline QSqlError fetchByQuery(T& t, QSqlDatabase* database, const QdOrmWhere& condition)
        {
            typedef typename std::conditional<std::is_pointer<T>::value, QdOrmDaoFetchPtr<T>, QdOrmDaoFetchGeneric<T>>::type type_1;
            typedef typename std::conditional<is_smart_ptr<T>::value, QdOrmDaoFetchPtr<T>, type_1>::type type_2;
            typedef typename std::conditional<is_container<T>::value, QdOrmDaoFetchContainer<T>, type_2>::type type_3;
            return type_3::fetchByQuery(t, database, condition);
        }
    };
}