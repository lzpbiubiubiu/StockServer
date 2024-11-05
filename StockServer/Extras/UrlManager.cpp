#include <QUrl>
#include "ModuleData/Core/GlobalData.h"
#include "UrlManager.h"

namespace Extra
{
    // 交易网关主机
    static const char* GWPOS_HOST_TABLE[] =
    {
        // DEV
        "https://devgwpos.sm-os.com",

        // UAT
        "https://uatgwpos.smmarkets.co",

        // PRO
        "https://gwpos.smmarkets.co"
    };

    // 管理网关主机
    static const char* GWMANAGE_HOST_TABLE[] =
    {
        // DEV
        "https://devgwbasics.sm-os.com",

        // UAT
        "https://uatgwbasics.smmarkets.co",

        // PRO
        "https://gwbasics.smmarkets.co"
    };

    UrlManager::UrlManager(QObject* parent/* = nullptr*/)
        : Base::SyncService(parent)
    {}

    UrlManager::~UrlManager()
    {}

    void UrlManager::AddUrl(Key key, const QString& host, const QString& path, int timeout, const QString& description/* = QString()*/)
    {
        if(m_urlTabs.contains(key))
            return;

        Param param;
        param.host = host;
        param.path = path;
        param.timeout = timeout;
        param.description = description;
        param.url = param.host + param.path;

        m_urlTabs[key] = param;
    }

    UrlManager::Param UrlManager::GetUrl(Key key) const
    {
        if(!m_urlTabs.contains(key))
            return Param();

        return m_urlTabs[key];
    }

    UrlManager::HostInfo UrlManager::GetGWHostInfo() const
    {
        auto env = Core::GlobalData::Get()->GetConfig().app.env;
        auto gwInfo = QUrl(GWPOS_HOST_TABLE[env]);

        HostInfo info;
        info.scheme = gwInfo.scheme();
        info.host = gwInfo.host();
        info.port = gwInfo.port();

        return info;
    }

    bool UrlManager::OnStart()
    {
        auto env = Core::GlobalData::Get()->GetConfig().app.env;
        const auto GWPOS = GWPOS_HOST_TABLE[env];
        const auto GWMANAGE = GWMANAGE_HOST_TABLE[env];

        // 交易网关
        AddUrl(HEART, GWPOS, "/heart", 5000, QStringLiteral("心跳"));

        // api/iot
        AddUrl(API_IOT_REPORT_EQUIPMENT_COMMAND_RESULT, GWPOS, "/api/iot/reportEquipmentCommandResult", 5000, QStringLiteral("设备指令回执上报"));
        
        // galaxy
        AddUrl(GALAXY_DEVICE_INFO, GWPOS, "/galaxy/deviceInfo", 5000, QStringLiteral("银河拉取设备信息"));
        AddUrl(GALAXY_VERSION, GWPOS, "/galaxy/version", 5000, QStringLiteral("银河设备版本升级检查"));
        AddUrl(GALAXY_REPORT_VERSION, GWPOS, "/api/galaxy/reportAppVersion", 5000, QStringLiteral("银河设备版本上报"));

        // 管理网关
        AddUrl(FILE_UPLOAD, GWMANAGE, "/basics/nessau/io/file/upload", 10 * 60 * 1000, QStringLiteral("文件上传"));

        // galaxy
        AddUrl(OFFLINE_CHECK_ACCOUNTS, GWPOS, "/api/posman/offlineCheckAccounts", 5000, QStringLiteral("离线对账"));
        return true;
    }

    void UrlManager::OnStop()
    {}
}// namespace Extra