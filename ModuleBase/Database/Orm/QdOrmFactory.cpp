#include "QdOrmFactory.h"

namespace Orm
{
    QdOrmFactory::~QdOrmFactory()
    {
        for(auto& ormClass : m_ormClasses)
        {
            if(nullptr != ormClass)
            {
                delete ormClass;
                ormClass = nullptr;
            }
        }

        m_ormClasses.clear();
    }

    const QdOrmClass* QdOrmFactory::getAppointClass(const QString& skey) const
    {
        return m_ormClasses.contains(skey) ? m_ormClasses[skey] : nullptr;
    }
}