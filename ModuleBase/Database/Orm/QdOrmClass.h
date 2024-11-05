#pragma once

#include <QObject>
#include "ModuleBase/ModuleBaseApi.h"
#include "ModuleBase/Database/Orm/QdOrmMember.h"

namespace Orm
{
    class MODULE_BASE_API QdOrmClass
    {
    public:
        virtual ~QdOrmClass();

        /** 注册主键 */
        OrmMetaMemberPtr id(const QString& sKey, const QString& sForm, const QString& eName, long lVersion = 1);

        /** 注册数据列 */
        OrmMetaMemberPtr data(const QString& sKey, const QString& sForm, const QString& eName, long lVersion);

        /** 获取主键字段名 */
        QString primaryKey() const;

        /** 设置数据库名称 */
        void setName(const QString& sKey);

        /** 获取数据库名称 */
        QString tableName() const;

        /** 设置版本号 */
        void setVersion(long version);

        /** 获取版本号 */
        long version() const;

        /** 获取当前Meta下所有成员数据 */
        const QList<OrmMetaMemberPtr>& ormMetaMembers() const;

    private:
        // 数据库名称
        QString m_tableName;

        // 主键列名称
        QString m_primaryKey;

        // 成员数据
        QList<OrmMetaMemberPtr> m_ormMembers;

        // 当前版本号
        long m_version = 1;
    };
 
    template <class T>
    void register_class(T& t)
    {
        Q_UNUSED(t);
        Q_ASSERT(false);
    }

    template <class T>
    class QdOrmClassDao : public QdOrmClass
    {
    public:
        void registerClass()
        {
            register_class<QdOrmClassDao<T>>(*this);
        }
    };
}