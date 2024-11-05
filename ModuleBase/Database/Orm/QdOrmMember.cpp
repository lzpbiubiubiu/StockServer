#include "QdOrmMember.h"
#include <QHash>

namespace Orm
{
    const QHash<QString, QString> SQL_FIELD_MAP =
    {
        {"QString",    "TEXT"},
        {"qint32",     "INTEGER"},
        {"qint64",     "INTEGER"},
        {"int",        "INTEGER"},
        {"QByteArray", "BLOB"},
        {"bool",       "SMALLINT"},
        {"QDateTime",  "TIMESTAMP"},
        {"QDate",      "DATE"},
        {"QTime",      "TIME"},
        {"double",     "FLOAT"},
        {"float",      "FLOAT"},
    };

    void QdOrmMember::setIsNotNull(bool isNotNull)
    {
        m_isNotNull = isNotNull;
    }

    bool QdOrmMember::isNotNull() const
    {
        return m_isNotNull;
    }

    void QdOrmMember::setIsUnique(bool isUnique)
    {
        m_isUnique = isUnique;
    }

    bool QdOrmMember::isUnique() const
    {
        return m_isUnique;
    }

    void QdOrmMember::setIsPrimaryKey(bool isPrimaryKey)
    {
        m_isPrimaryKey = isPrimaryKey;
    }

    bool QdOrmMember::isPrimaryKey() const
    {
        return m_isPrimaryKey;
    }

    void QdOrmMember::setIsIndex(bool isIndex)
    {
        m_isIndex = isIndex;
    }

    bool QdOrmMember::isIndex() const
    {
        return m_isIndex;
    }

    void QdOrmMember::setFieldForm(const QString& form)
    {
        m_fieldForm = form;
    }

    QString QdOrmMember::fieldForm() const
    {
        return m_fieldForm;
    }

    void QdOrmMember::setFieldName(const QString& name)
    {
        m_fieldName = name;
    }

    QString QdOrmMember::fieldName() const
    {
        return m_fieldName;
    }

    void QdOrmMember::setEntityName(const QString& name)
    {
        m_entityName = name;
    }

    QString QdOrmMember::entityName() const
    {
        return m_entityName;
    }

    void QdOrmMember::setFieldVersion(long version)
    {
        m_fieldVersion = version;
    }

    long QdOrmMember::fieldVersion() const
    {
        return m_fieldVersion;
    }

    QString QdOrmMember::fieldSqlForm() const
    {
        return SQL_FIELD_MAP[m_fieldForm];
    }
}