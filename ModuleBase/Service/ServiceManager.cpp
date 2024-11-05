#include <QMutex>
#include <QTime>
#include <QSemaphore>
#include <QMetaEnum>
#include "ModuleBase/Log/Log.h"
#include "ServiceManager.h"

namespace Base
{
    ServiceManager::ServiceManager(QObject* parent)
        : QObject(parent)
    {
        qRegisterMetaType<Base::ServiceManager::StartupOption>("StartupOption");
        moveToThread(&m_thread);
        m_thread.start();
    }

    ServiceManager::~ServiceManager()
    {
        for(auto& entry : m_entries.values())
        {
            if(entry.instance != nullptr)
            {
                delete entry.instance;
                entry.instance = nullptr;
            }
        }
        m_entries.clear();
        m_entryNames.clear();

        m_thread.quit();
        m_thread.wait();
    }

    void ServiceManager::Load(StartupOption option)
    {
        // 以ServiceName枚举定义顺序同步加载
        QMetaObject::invokeMethod(this, "onLoad", Q_ARG(StartupOption, option));
    }

    void ServiceManager::Unload()
    {
        // 以ServiceName枚举定义反向顺序同步卸载
        QMetaObject::invokeMethod(this, "onUnload");
    }

    void ServiceManager::onLoad(StartupOption option)
    {
        LOG_INFO("Start Services...");

        // 按枚举定义顺序启动服务线程
        bool exit = false;
        QTime tick;
        QSemaphore semp;
        
        QStringList::const_iterator itr = m_entryNames.cbegin();
        for(; itr != m_entryNames.cend(); ++itr)
        {
            QString serviceName = (*itr);
            if(!m_entries.contains(serviceName))
                continue;

            if(m_entries[serviceName].instance->GetState() == IService::RUNNING)
                continue;

            // 启动方式不匹配
            if(m_entries[serviceName].option != option)
                continue;

            if(m_entries[serviceName].instance->IsAsync())
            {
                // 断开之前的连接，防止重复连接响应槽函数引起崩溃
                QObject::disconnect(m_entries[serviceName].instance, &Service::signalStarted, this, 0);
                QObject::connect(m_entries[serviceName].instance, &Service::signalStarted, this, [&](bool success)
                {
                    if(!success)
                    {
                        QString message = "Failed to start services";
                        m_entries[serviceName].instance->m_thread.quit();
                        emit signalLoadError(message + QString("(%1)").arg(serviceName));
                        QString errmsg = QStringLiteral("[%1]...FAILED,error: %2").arg(serviceName).arg(message);
                        LOG_ERROR(errmsg.toStdString());
                        exit = true;
                    }

                    semp.release();
                }, Qt::DirectConnection);

                // 启动服务实例
                tick.restart();
                m_entries[serviceName].instance->m_thread.start();

                // 等待启动完成
                semp.acquire();
            }
            else
            {
                bool success = m_entries[serviceName].instance->OnStart();
                if(!success)
                {
                    QString message = "Failed to start services";
                    emit signalLoadError(message + QString("(%1)").arg(serviceName));
                    QString errmsg = QStringLiteral("[%1]...FAILED,error: %2").arg(serviceName).arg(message);
                    LOG_ERROR(errmsg.toStdString());
                    exit = true;
                }
                else
                {
                    m_entries[serviceName].instance->m_state = IService::RUNNING;
                }
            }

            LOG_INFO(QStringLiteral("[%1]...STARTED %2ms").arg(serviceName).arg(tick.elapsed()).toStdString());
            m_entries[serviceName].instance->OnAfterStart();

            // 错误中断
            if(exit) break;
        }

        if(!exit)
        {
            LOG_INFO("Start Services Success");
            emit signalLoaded(option);
        }
    }

    void ServiceManager::onUnload()
    {
        LOG_INFO("Stop Services...");

        // 按枚举定义倒序终止服务线程
        QTime tick;
        QStringList::const_reverse_iterator itr = m_entryNames.crbegin();
        for(; itr != m_entryNames.crend(); ++itr)
        {
            QString serviceName = (*itr);
            if(!m_entries.contains(serviceName))
                continue;

            if(m_entries[serviceName].instance->GetState() == IService::STOPPED)
                continue;

            m_entries[serviceName].instance->OnBeforeStop();

            tick.restart();
            if(m_entries[serviceName].instance->IsAsync())
            {
                m_entries[serviceName].instance->m_thread.quit();
                m_entries[serviceName].instance->m_thread.wait(20000);
            }
            else
            {
                m_entries[serviceName].instance->OnStop();
                m_entries[serviceName].instance->m_state = IService::STOPPED;
            }

            LOG_INFO(QStringLiteral("[%1]...STOPPED %2ms").arg(serviceName).arg(tick.elapsed()).toStdString());
        }

        LOG_INFO("Stop Services Finished");
        emit signalUnloaded();
    }
}