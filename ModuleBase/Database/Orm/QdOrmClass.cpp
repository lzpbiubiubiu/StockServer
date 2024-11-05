#include "QdOrmClass.h"

namespace Orm
{
    QdOrmClass::~QdOrmClass()
    {
        m_ormMembers.clear();
    }

    OrmMetaMemberPtr QdOrmClass::id(const QString& sKey, const QString& sForm,const QString& eName, long lVersion)
    {
        OrmMetaMemberPtr member = OrmMetaMemberPtr::create();
        member->setFieldName(sKey);
        member->setEntityName(eName);
        member->setFieldForm(sForm);
        member->setIsPrimaryKey(true);
        member->setIsNotNull(true);
        member->setFieldVersion(lVersion);
        m_primaryKey = sKey;
        m_ormMembers << member;
        return member;
    }

    OrmMetaMemberPtr QdOrmClass::data(const QString& sKey, const QString& sForm, const QString& eName, long lVersion)
    {
        OrmMetaMemberPtr member = OrmMetaMemberPtr::create();
        member->setFieldName(sKey);
        member->setEntityName(eName);
        member->setFieldForm(sForm);
        member->setFieldVersion(lVersion);
        m_ormMembers << member;
        return member;
    }

    void QdOrmClass::setName(const QString& name)
    {
        m_tableName = name;
    }

    QString QdOrmClass::primaryKey() const
    {
        return m_primaryKey;
    }

    QString QdOrmClass::tableName() const
    {
        return m_tableName;
    }

    void QdOrmClass::setVersion(long version)
    {
        m_version = version;
    }

    long QdOrmClass::version() const
    {
        return m_version;
    }

    const QList<OrmMetaMemberPtr>& QdOrmClass::ormMetaMembers() const
    {
        return m_ormMembers;
    }
}