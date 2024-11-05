#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Common/System.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Http/Client/Client.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleProtocol/Client/GalaxyDeviceInfoRequest.h"
#include "ModuleProtocol/Client/GalaxyDeviceInfoResponse.h"
#include "ConfigManager.h"
#include "TerminalManager.h"
#include "UrlManager.h"

namespace Extra
{
    TerminalManager::TerminalManager(QObject* parent/* = nullptr*/)
        : Base::AsyncService(parent)
    {
        qRegisterMetaType<Terminal>("Terminal");
        qRegisterMetaType<Extra::TerminalManager::Terminal>("Extra::TerminalManager::Terminal");
        qRegisterMetaType<QMap<QString, Terminal>>("QMap<QString, Terminal>");
        qRegisterMetaType<QMap<QString, Extra::TerminalManager::Terminal>>("QMap<QString, Extra::TerminalManager::Terminal>");
        qRegisterMetaType<TerminalType>("TerminalType");
        qRegisterMetaType<Extra::TerminalManager::TerminalType>("Extra::TerminalManager::TerminalType");
    }

    TerminalManager::~TerminalManager()
    {}

    QMap<QString, TerminalManager::Terminal> TerminalManager::GetTerminals(TerminalType type)
    {
        QReadLocker locker(&m_terminalLock);
        if(type == POS)
            return m_posTerminals;
        else if(type == PVT)
            return m_pvtTerminals;

        return QMap<QString, Terminal>();
    }

    void TerminalManager::Refresh()
    {
        QMetaObject::invokeMethod(this, "onRefresh");
    }

    bool TerminalManager::OnStart()
    {
        // 1min 定时器
        m_checkTimer = new QTimer(this);
        m_checkTimer->setInterval(60000);

        connect(m_checkTimer, &QTimer::timeout, this, &TerminalManager::onCheckTimout);
        connect(Base::GetService<ConfigManager>(), &ConfigManager::signalLoadDeviceSuccess,
            this, &TerminalManager::onLoadDeviceSuccess);
        connect(Base::GetService<ConfigManager>(), &ConfigManager::signalLoadCacheSuccess,
            this, &TerminalManager::onLoadDeviceSuccess);

        return true;
    }

    void TerminalManager::OnStop()
    {
        m_checkTimer->disconnect();
        m_checkTimer->stop();
    }

    void TerminalManager::onRefresh()
    {
        auto urlParam = Base::GetService<UrlManager>()->GetUrl(UrlManager::GALAXY_DEVICE_INFO);
        auto request = QSharedPointer<Http::GalaxyDeviceInfoRequest>::create();
        auto response = QSharedPointer<Http::GalaxyDeviceInfoResponse>::create();

        request->SetUrl(urlParam.url);
        request->SetTimeout(urlParam.timeout);
        Http::Client::Get()->ProcessRequest(request, response);

        if(!response->IsSuccess())
        {
            emit signalRefreshError(response->GetErrorMessage(), response->GetError());
            LOG_INFO(QStringLiteral("刷新终端列表失败").toStdString());
            return;
        }

        auto data = response->GetResult();
        QWriteLocker locker(&m_terminalLock);
        QMap<QString, Terminal> newPosTerminals, newPvtTerminals;

        for(auto item : data.winPosSimpleEquipmentVoList)
        {
            Terminal terminal;
            terminal.id = QString::number(item.id);
            terminal.code = item.externalCode;
            terminal.deskNo = item.customField["deskNum"].toString();
            terminal.connected = m_posTerminals[terminal.id].connected;
            terminal.heartTimestamp = m_posTerminals[terminal.id].heartTimestamp;
            terminal.notUploadCloudServer = m_posTerminals[terminal.id].notUploadCloudServer;
            terminal.notUpLoadInStoreServer = m_posTerminals[terminal.id].notUpLoadInStoreServer;
            newPosTerminals[terminal.id] = terminal;
        }
        m_posTerminals = newPosTerminals;

        for(auto item : data.pvtSimpleEquipmentVoList)
        {
            Terminal terminal;
            terminal.id = QString::number(item.id);
            terminal.code = item.externalCode;
            terminal.deskNo = item.customField["deskNum"].toString();
            terminal.connected = m_pvtTerminals[terminal.id].connected;
            terminal.heartTimestamp = m_pvtTerminals[terminal.id].heartTimestamp;
            terminal.notUploadCloudServer = m_pvtTerminals[terminal.id].notUploadCloudServer;
            terminal.notUpLoadInStoreServer = m_pvtTerminals[terminal.id].notUpLoadInStoreServer;
            newPvtTerminals[terminal.id] = terminal;
        }
        m_pvtTerminals = newPvtTerminals;

        LOG_INFO(QStringLiteral("刷新终端列表成功").toStdString());
        emit signalRefreshSuccess(newPosTerminals, newPvtTerminals);
    }

    void TerminalManager::onCheckTimout()
    {
        QWriteLocker locker(&m_terminalLock);
        for(auto& item : m_posTerminals)
        {
            if(!item.connected)
                continue;

            // 超过2分钟未收到心跳包，认为断开连接
            if(Base::System::GetCurrentTimeMillis() - item.heartTimestamp >= 2 * 60 * 1000)
            {
                item.connected = false;
                LOG_INFO("POS terminal disconnected, id{}", item.id.toStdString());
                emit signalDisconnected(POS, item);
            }
        };

        for(auto& item : m_pvtTerminals)
        {
            if(!item.connected)
                continue;

            if(Base::System::GetCurrentTimeMillis() - item.heartTimestamp >= 2 * 60 * 1000)
            {
                item.connected = false;
                LOG_INFO("PVT terminal disconnected, id{}", item.id.toStdString());
                emit signalDisconnected(PVT, item);
            }
        };
    }

    void TerminalManager::onLoadDeviceSuccess()
    {
        QWriteLocker locker(&m_terminalLock);
        auto device = Core::GlobalData::Get()->GetConfig().device;
        m_posTerminals.clear();
        m_pvtTerminals.clear();

        for(auto item : device.posTerminals)
        {
            Terminal terminal;
            terminal.id = item.id;
            terminal.code = item.code;
            terminal.deskNo = item.deskNo;
            m_posTerminals[terminal.id] = terminal;
        }

        for(auto item : device.pvtTerminals)
        {
            Terminal terminal;
            terminal.id = item.id;
            terminal.code = item.code;
            terminal.deskNo = item.deskNo;
            m_pvtTerminals[terminal.id] = terminal;
        }

        m_checkTimer->stop();
        m_checkTimer->start();
    }

    void TerminalManager::onHeartbeat(int type, const QString& id, int cloud, int inStore)
    {
        QWriteLocker locker(&m_terminalLock);
        if(type == 1)
        {
            // POS
            if(m_posTerminals.contains(id))
            {
                bool refresh = false;
                m_posTerminals[id].heartTimestamp = Base::System::GetCurrentTimeMillis();
                if(!m_posTerminals[id].connected)
                {
                    m_posTerminals[id].connected = true;
                    LOG_INFO("POS terminal connected, id{}", id.toStdString());
                    refresh = true;
                    //emit signalConnected(POS, m_posTerminals[id]);
                }
                if(m_posTerminals[id].notUploadCloudServer != cloud || m_posTerminals[id].notUpLoadInStoreServer != inStore)
                {
                    m_posTerminals[id].notUploadCloudServer = cloud;
                    m_posTerminals[id].notUpLoadInStoreServer = inStore;
                    refresh = true;
                }
                if(refresh)
                {
                    emit signalConnected(POS, m_posTerminals[id]);
                }
            }
        }
        else if(type == 2)
        {
            // PVT
            if(m_pvtTerminals.contains(id))
            {
                bool refresh = false;
                m_pvtTerminals[id].heartTimestamp = Base::System::GetCurrentTimeMillis();
                if(!m_pvtTerminals[id].connected)
                {
                    m_pvtTerminals[id].connected = true;
                    refresh = true;
                    LOG_INFO("PVT terminal connected, id{}", id.toStdString());
                    //emit signalConnected(PVT, m_pvtTerminals[id]);
                }
                if(m_pvtTerminals[id].notUploadCloudServer != cloud || m_pvtTerminals[id].notUpLoadInStoreServer != inStore)
                {
                    refresh = true;
                    m_pvtTerminals[id].notUploadCloudServer = cloud;
                    m_pvtTerminals[id].notUpLoadInStoreServer = inStore;
                }
                if(refresh)
                {
                    emit signalConnected(PVT, m_pvtTerminals[id]);
                }
            }
        }
    }
}