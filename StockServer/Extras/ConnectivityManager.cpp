#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/GlobalData.h"
#include "ConnectivityManager.h"
#include "UrlManager.h"

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
    #include <Windows.h>
    #include <WinSock2.h>
    #include <ws2tcpip.h>
    #include <Wspiapi.h>
    #pragma comment(lib, "Ws2_32.lib")
#endif


namespace Extra
{
    // 监测时间间隔（秒）
    static const int CHECK_INTERVAL_SEC = 3;

    bool WinsockInit()
    {
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
        WSADATA wsaData;
        return 0 == WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
        return false;
#endif
    }

    void WinsockRelease()
    {
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
        WSACleanup();
#endif
    }

    /**
     * @brief WinGetHostByName
     * @param hostName without http or https
     * @return true == valid name return
     * @see https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
     */
    bool WinGetHostByName(const QString& hostName, const QString& scheme)
    {
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
        static bool inited = WinsockInit();
        if(!inited)
            return false;

        ADDRINFOA hints;
        // Setup the hints address info structure
        // which is passed to the getaddrinfo() function
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        PADDRINFOA result;
        if(0 != getaddrinfo(hostName.toStdString().c_str(), scheme.toStdString().c_str(), &hints, &result))
            return false;

        freeaddrinfo(result);
        return true;
#endif
        return false;
    }

    ConnectivityManager::ConnectivityManager(QObject* parent)
        : Base::AsyncService(parent)
    {}

    ConnectivityManager::~ConnectivityManager()
    {}

    bool ConnectivityManager::IsOnlineServiceConnected() const
    {
        return m_onlineServiceConnected;
    }

    bool ConnectivityManager::OnStart()
    {
        // 检测网络连接
        DetectNetwork(0);
        return true;
    }

    void ConnectivityManager::OnStop()
    {
        WinsockRelease();
    }

    void ConnectivityManager::DetectNetwork(quint64 delaySec)
    {
        auto info = Base::GetService<UrlManager>()->GetGWHostInfo();
        if(delaySec)
        {
            QTimer::singleShot(delaySec * 1000, this, std::bind(&ConnectivityManager::DetectNetwork, this, 0));
            return;
        }
        SetOnlineServiceConnected(WinGetHostByName(info.host, info.scheme));
        DetectNetwork(CHECK_INTERVAL_SEC);
    }

    void ConnectivityManager::SetOnlineServiceConnected(bool connected)
    {
        if(m_onlineServiceConnected != connected)
        {
            LOG_INFO(QStringLiteral("网络状态变化：%1").arg(connected ? QStringLiteral("已连接") : QStringLiteral("断开连接")).toStdString());
            m_onlineServiceConnected = connected;
            emit m_onlineServiceConnected ? signalOnlineServiceConnected() : signalOnlineServiceDisconnected();
        }
    }
}