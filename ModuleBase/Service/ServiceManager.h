#pragma once

#include <QThread>
#include <QHash>
#include <QMetaObject>
#include "Service.h"
#include "ModuleBase/ModuleBaseApi.h"
#include "ModuleBase/Common/Singleton.h"

namespace Base
{
    /** 服务管理器，负责服务的创建和启动 */
    class MODULE_BASE_API ServiceManager : public QObject
    {
        Q_OBJECT
        TO_BE_SINGLETON(ServiceManager)

    public:
        /** 启动选项 */
        enum StartupOption
        {
            /** 开机启动 */
            BOOT = 0,

            /** 延迟启动 */
            LAZY
        };
        Q_ENUM(StartupOption)

        /** 添加服务 */
        template<typename T>
        void AddService(StartupOption option = BOOT);

        /** 移除服务 */
        template<typename T>
        void RemoveService();

        /** 获取服务 */
        template<typename T>
        Service* GetService();

        /** 加载服务 */
        void Load(StartupOption option);

        /** 卸载服务 */
        void Unload();

    Q_SIGNALS:
        /** 服务加载完成信号 */
        void signalLoaded(StartupOption option);

        /** 服务加载出错信号 */
        void signalLoadError(const QString& message);

        /** 服务卸载完成信号 */
        void signalUnloaded();

    protected:
        ServiceManager(QObject* parent = nullptr);
        ~ServiceManager();

        Q_INVOKABLE void onLoad(StartupOption option);
        Q_INVOKABLE void onUnload();

    private:
        struct ServiceEntry
        {
            StartupOption option = BOOT;
            Service* instance = nullptr;
        };

        QThread m_thread;
        QHash<QString, ServiceEntry> m_entries;
        QStringList m_entryNames;
    };

    /** 添加服务 */
    template<typename T>
    void ServiceManager::AddService(StartupOption option)
    {
        const QString& name = T::staticMetaObject.className();
        if(!m_entries.contains(name))
        {
            ServiceEntry entry;
            entry.option = option;
            entry.instance = new T();
            m_entries[name] = entry;
            m_entryNames.append(name);
        }
    }

    /** 移除服务 */
    template<typename T>
    void ServiceManager::RemoveService()
    {
        const QString& name = T::staticMetaObject.className();
        if(m_entries.contains(name))
            m_entries.remove(name);

        if(m_entryNames.contains(name))
            m_entryNames.removeOne(name);
    }

    /** 获取服务 */
    template<typename T>
    Service* ServiceManager::GetService()
    {
        const QString& name = T::staticMetaObject.className();
        Service* instance = nullptr;
        if(m_entries.contains(name))
            instance = m_entries[name].instance;
        return instance;
    }

    /** 获取服务实例 */
    template<typename T>
    T* GetService()
    {
        T* instance = qobject_cast<T*>(ServiceManager::Get()->GetService<T>());
        Q_ASSERT_X(instance, "GetService()", "instance is null");
        return instance;
    }
}