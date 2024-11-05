#pragma once

#include "ModuleBase/ModuleBaseApi.h"
#include "ModuleBase/Common/Singleton.h"
#include "ModuleBase/Database/Orm/QdOrmClass.h"

namespace Orm
{
    class MODULE_BASE_API QdOrmFactory
    {
        TO_BE_SINGLETON(QdOrmFactory)

    public:
        ~QdOrmFactory();

        /** 注册数据结构模板类 */
        template <class T>
        void registerClass(const QString& skey);

        /** 获取指定数据结构指针 */
        const QdOrmClass* getAppointClass(const QString& skey) const;

    private:
        QMap<QString, QdOrmClass*> m_ormClasses;
    };

    template <class T>
    void QdOrmFactory::registerClass(const QString& skey)
    {
        if(!m_ormClasses.contains(skey))
        {
            QdOrmClassDao<T>* ormClass = new QdOrmClassDao<T>();
            ormClass->registerClass();
            m_ormClasses[skey] = ormClass;
        }
    }

    template <class T>
    class QdOrmFactoryHelper
    {
    public:
        QdOrmFactoryHelper(const QString& name)
        {
            QdOrmFactory::Get()->registerClass<T>(name);
        }
    };
}