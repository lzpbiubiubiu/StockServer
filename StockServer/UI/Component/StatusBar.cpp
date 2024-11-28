#include "ModuleBase/Common/System.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/GlobalData.h"
#include "StatusBar.h"

namespace UI
{
    StatusBar::StatusBar(QObject* parent)
        : QObject(parent)
    {
        m_configManager = Base::GetService<Extra::ConfigManager>();
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &StatusBar::onTimeout);

        m_timer->setInterval(1000);
        m_timer->start();
    }

    StatusBar::~StatusBar()
    {}

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
        SetNetState(true);

        // 系统时间
        QDateTime currentTime = Base::System::GetCurrentDateTime();
        SetTime(currentTime.toString("yyyy-MM-dd HH:mm:ss"));
    }

    void StatusBar::onLoadDeviceSuccess()
    {
        const auto& config = Core::GlobalData::Get()->GetConfig();
        SetVersion(config.version.string);
    }
}
