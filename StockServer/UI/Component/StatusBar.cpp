#include "ModuleBase/Common/System.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/GlobalData.h"
#include "Extras/ConnectivityManager.h"
#include "StatusBar.h"

namespace UI
{
    StatusBar::StatusBar(QObject* parent)
        : QObject(parent)
    {
        m_configManager = Base::GetService<Extra::ConfigManager>();
        m_timer = new QTimer(this);

        connect(m_configManager, &Extra::ConfigManager::signalLoadDeviceSuccess,
            this, &StatusBar::onLoadDeviceSuccess);
        connect(m_timer, &QTimer::timeout, this, &StatusBar::onTimeout);

        m_timer->setInterval(1000);
        m_timer->start();
    }

    StatusBar::~StatusBar()
    {}

    const QString StatusBar::GetStoreName() const
    {
        return m_storeName;
    }

    void StatusBar::SetStoreName(const QString& storeName)
    {
        if(m_storeName != storeName)
        {
            m_storeName = storeName;
            emit signalSotreNameChanged();
        }
    }

    const QString& StatusBar::GetDeviceId() const
    {
        return m_deviceId;
    }

    void StatusBar::SetDeviceId(const QString& deviceId)
    {
        if(m_deviceId != deviceId)
        {
            m_deviceId = deviceId;
            emit signalDeviceIdChanged();
        }
    }

    const QString& StatusBar::GetVersion() const
    {
        return m_version;
    }

    void StatusBar::SetVersion(const QString& version)
    {
        if(m_version != version)
        {
            m_version = version;
            emit signalVersionChanged();
        }
    }

    const QString& StatusBar::GetTime() const
    {
        return m_time;
    }

    void StatusBar::SetTime(const QString& time)
    {
        if(m_time != time)
        {
            m_time = time;
            emit signalTimeChanged();
        }
    }

    bool StatusBar::GetNetState() const
    {
        return m_netState;
    }

    void StatusBar::SetNetState(bool netState)
    {
        if(m_netState != netState)
        {
            m_netState = netState;
            emit signalNetStateChanged();
        }
    }

    void StatusBar::onQuitClicked()
    {
        //UI::Application::Get()->Exit();
    }

    void StatusBar::onTimeout()
    {
        auto nm = Base::GetService<Extra::ConnectivityManager>();

        // 网络状态
        if(nm && nm->IsOnlineServiceConnected())
            SetNetState(true);
        else
            SetNetState(false);

        // 系统时间
        QDateTime currentTime = Base::System::GetCurrentDateTime();
        SetTime(currentTime.toString("yyyy-MM-dd HH:mm:ss"));
    }

    void StatusBar::onLoadDeviceSuccess()
    {
        const auto& config = Core::GlobalData::Get()->GetConfig();
        SetVersion(config.version.string);
        SetStoreName(config.device.storeName);
        SetDeviceId(config.device.deviceId);
    }
}
