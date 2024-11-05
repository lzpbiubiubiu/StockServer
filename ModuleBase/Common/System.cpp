#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include "System.h"
#include "Util/PathUtil.h"

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#include <Windows.h>
//#include <Netcfgx.h>
#include <Strsafe.h>
#include <Iphlpapi.h>
#include <Shlwapi.h>
#include <Shellapi.h>
#include <Tlhelp32.h>
#include <GL/gl.h>
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "opengl32.lib")

#include "ModuleBase/Log/Log.h"

// 查询网卡特征参数，Windows XP SP2以上兼容
UINT GetAdapterCharacteristics(LPCSTR lpszAdapterName)
{
    if(lpszAdapterName == NULL)
        return 0;

    HKEY hRoot = NULL;
    LPCSTR lpszNetworkAdapterGUID = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}";
    if(ERROR_SUCCESS != RegOpenKeyExA(HKEY_LOCAL_MACHINE, lpszNetworkAdapterGUID, 0, KEY_READ, &hRoot))
        return 0;

    DWORD dwSubkeys = 100;
    if(RegQueryInfoKeyA(hRoot, NULL, NULL, NULL, &dwSubkeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
        dwSubkeys = 100;

    DWORD dwRetVal = 0;
    for(DWORD i = 0; i < dwSubkeys; i++)
    {
        CHAR szSubKey[32];
        ::ZeroMemory(szSubKey, sizeof(szSubKey));
        ::StringCbPrintfA(szSubKey, sizeof(szSubKey), "%04u", i);

        // 打开子键
        HKEY hKey = NULL;
        if(::RegOpenKeyExA(hRoot, szSubKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            continue;

        // 查询适配器名称
        char szName[MAX_PATH];
        DWORD dwType = 0;
        DWORD dwSize = MAX_PATH;
        if(::RegQueryValueExA(hKey, "NetCfgInstanceId", NULL, &dwType, (LPBYTE)szName, &dwSize) != ERROR_SUCCESS)
        {
            ::RegCloseKey(hKey);
            continue;
        }

        // 检查名称是否匹配
        if(::StrCmpIA(szName, lpszAdapterName) != 0)
        {
            ::RegCloseKey(hKey);
            continue;
        }

        // 读取适配器特征值
        DWORD dwRet = 0;
        dwSize = 4;
        LONG lStatus = ::RegQueryValueExA(hKey, "Characteristics", NULL, &dwType, (LPBYTE)&dwRet, &dwSize);
        ::RegCloseKey(hKey);
        if(lStatus == ERROR_SUCCESS)
        {
            dwRetVal = dwRet;
            break;
        }
    }
    ::RegCloseKey(hRoot);
    return dwRetVal;
}
#endif

namespace Base
{
    QString System::s_mac = "";

    QDateTime System::GetCurrentDateTime()
    {
        return QDateTime::currentDateTime();
    }

    qint64 System::GetCurrentTimeMillis()
    {
        return QDateTime::currentMSecsSinceEpoch();
    }

    QString System::GetMAC()
    {
        if(!s_mac.isEmpty())
            return s_mac;

        // 缓存文件读取MAC
        QString fileName = PathUtil::GetConfigCacheDir() + "Device.json";
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly))
        {
            QJsonParseError parseError;
            auto doc = QJsonDocument::fromJson(file.readAll(), &parseError);
            if(parseError.error == QJsonParseError::NoError)
            {
                auto root = doc.object();
                s_mac = root["deviceCode"].toString();
            }
            file.close();
            if(!s_mac.isEmpty())
                return s_mac;
        }

        // WIN32平台接口获取MAC
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
        CHAR szMac[MAX_PATH];
        ::ZeroMemory(szMac, sizeof(szMac));
        PIP_ADAPTER_INFO pAdapterInfo;
        DWORD dwAdapterInfoSize = 0;
        DWORD dwRetVal = 0;
        ::GetAdaptersInfo(NULL, &dwAdapterInfoSize);
        pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, dwAdapterInfoSize);
        ::GetAdaptersInfo(pAdapterInfo, &dwAdapterInfoSize);
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while(pAdapter)
        {
            UINT uFlags = GetAdapterCharacteristics(pAdapter->AdapterName);
            // 检查是否物理网卡
            if(uFlags != 0 && ((uFlags & 0x4) == 0x4))
            {
                if(pAdapter->AddressLength == 6)
                {
                    // 只取第一张网卡地址
                    ::StringCbPrintfA(szMac, sizeof(szMac), "%02X-%02X-%02X-%02X-%02X-%02X",
                        pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2],
                        pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
                    break;
                }
            }
            pAdapter = pAdapter->Next;
        }
        ::GlobalFree(pAdapterInfo);
        s_mac = QString::fromLatin1(szMac);
#endif
        if(!s_mac.isEmpty())
            return s_mac;

        // Qt接口获取MAC
        auto nets = QNetworkInterface::allInterfaces();
        for(int i = 0; i < nets.length(); ++i)
        {
            if(nets[i].isValid() && nets[i].flags().testFlag(QNetworkInterface::IsUp)
                && nets[i].flags().testFlag(QNetworkInterface::IsRunning)
                && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
            {
                s_mac = nets[i].hardwareAddress();
                break;
            }
        }
        return s_mac;
    }

    void System::StartStockServer()
    {
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
        QDir dir(QCoreApplication::applicationDirPath());
        QString appPath = dir.absolutePath() + QDir::separator();
        dir.cdUp();
        QString rootPath = dir.absolutePath() + QDir::separator();
        QString program = "\"" + appPath + "StockServer.exe\"";
        ShellExecute(NULL, TEXT("open"), program.toStdWString().c_str(), NULL, NULL, SW_SHOW);

#endif
    }

    bool System::GetOpenGLVersion(int& major, int& minor)
    {
        bool success = false;
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        HDC hdc = NULL;
        HGLRC hglrc = NULL;
        do
        {
            hdc = ::GetDC(NULL);
            if(!hdc)
                break;

            int pixelFormat = ::ChoosePixelFormat(hdc, &pfd);
            if(!pixelFormat)
                break;

            if(!::SetPixelFormat(hdc, pixelFormat, &pfd))
                break;

            int activePixelFormat = ::GetPixelFormat(hdc);
            if(!activePixelFormat)
                break;

            if(!::DescribePixelFormat(hdc, activePixelFormat, sizeof pfd, &pfd))
                break;

            if((pfd.dwFlags & PFD_SUPPORT_OPENGL) != PFD_SUPPORT_OPENGL)
                break;

            hglrc = ::wglCreateContext(hdc);
            if(!hglrc)
                break;

            if(!::wglMakeCurrent(hdc, hglrc))
                break;

            const char* verstr = (const char*)glGetString(GL_VERSION);
            if((verstr == NULL) || (sscanf(verstr, "%d.%d", &major, &minor) != 2))
                major = minor = 0;
            else
                success = true;

        } while(0);

        if(hglrc != NULL)
            ::wglDeleteContext(hglrc);

        if(hdc != NULL)
            ::ReleaseDC(NULL, hdc);
#endif
        return success;
    }

    bool System::IsHardwareRenderEnabled()
    {
        QString appJsonFile = Base::PathUtil::GetDataDir() + "App.json";
        QFile file(appJsonFile);
        if(file.open(QIODevice::ReadOnly))
        {
            QByteArray data = file.readAll();
            file.close();
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
            if(parseError.error == QJsonParseError::NoError)
            {
                QJsonObject root = doc.object();
                QString str = root["render"].toString();
                if(str.compare("software", Qt::CaseInsensitive) == 0)
                    return false;
            }
        }
        return true;
    }

    double System::GetScreenScaleFactor()
    {
        double sw = ::GetSystemMetrics(SM_CXSCREEN);
        HWND hWnd = ::GetDesktopWindow();
        HDC hdc = ::GetDC(hWnd);
        int width = GetDeviceCaps(hdc, DESKTOPHORZRES);
        return width / sw;
    }
}// namespace Base