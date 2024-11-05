#pragma once

#include <QObject>
#include <QVariant>
#include <QSharedPointer>
#include "ModuleBase/ModuleBaseApi.h"

namespace Orm
{
    class MODULE_BASE_API QdOrmMember
    {
    public:
        void setIsNotNull(bool isNotNull);
        bool isNotNull() const;

        void setIsUnique(bool isUnique);
        bool isUnique() const;

        void setIsPrimaryKey(bool isPrimaryKey);
        bool isPrimaryKey() const;

        void setIsIndex(bool isIndex);
        bool isIndex() const;

        void setFieldForm(const QString& form);
        QString fieldForm() const;

        void setFieldName(const QString& name);
        QString fieldName() const;

        void setEntityName(const QString& name);
        QString entityName() const;

        void setFieldVersion(long version);
        long fieldVersion() const;

        QString fieldSqlForm() const;

    private:
        bool m_isNotNull = false;

        bool m_isUnique = false;

        bool m_isPrimaryKey = false;

        bool m_isIndex = false;

        QString m_fieldForm;

        QString m_entityName;

        QString m_fieldName;

        long m_fieldVersion = 1;
    };

    using OrmMetaMemberPtr = QSharedPointer<QdOrmMember>;
}