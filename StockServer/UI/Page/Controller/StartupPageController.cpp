#include "ModuleBase/Common/System.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/GlobalData.h"
#include "StartupPageController.h"
#include "Extras/HttpServerManager.h"
#include "Extras/RemoteManager.h"

#define ERROR_CODE_DEVICE_INFO      "3001"                  // 设备未注册
#define ERROR_CODE_DEVICE_NOT_MATCH "3157"                  // 设备不匹配
#define ERROR_CODE_DEVICE_STOP      "GLXGATEWAY2008"        // 设备停用

namespace UI
{
    StartupPageController::StartupPageController(Page* parent)
        : PageController(parent)
    {
        m_page = GetPage<StartupPage>();
        m_actionSequence = new Base::ActionSequence(this);
        m_serviceManager = Base::ServiceManager::Get();
        m_connectivityManager = Base::GetService<Extra::ConnectivityManager>();
        m_urlManager = Base::GetService<Extra::UrlManager>();
        m_configManager = Base::GetService<Extra::ConfigManager>();
        m_packageManager = Base::GetService<Extra::PackageManager>();
        Base::ActionSequence::Action action;

        // 系统信息Action初始化
        action.id = ACTION_SYS_LOAD_SERVICE;
        action.callback = std::bind(&StartupPageController::LoadService, this);
        m_actionSequence->Add(action);

        action.id = ACTION_SYS_CHECK_NETWORK;
        action.callback = std::bind(&StartupPageController::CheckNetwork, this);
        m_actionSequence->Add(action);

        action.id = ACTION_GET_DEVICE_INFO;
        action.callback = std::bind(&StartupPageController::GetDeviceInfo, this);
        m_actionSequence->Add(action);

        action.id = ACTION_CHECK_UPDATE;
        action.callback = std::bind(&StartupPageController::CheckUpdate, this);
        m_actionSequence->Add(action);

        connect(m_actionSequence, &Base::ActionSequence::signalAllActionFinished, this,
            &StartupPageController::onAllActionFinished, Qt::QueuedConnection);
    }

    StartupPageController::~StartupPageController()
    {
    }

    void StartupPageController::retry()
    {
        InitUI();
        m_actionSequence->Next();
    }

    void StartupPageController::quit()
    {
        UI::Application::Get()->Exit();
    }

    void StartupPageController::start()
    {
        UI::Application::GetMainWindow()->ShowLoading();
        m_configManager->LoadCache();
    }

    void StartupPageController::ignoreUpdate()
    {
        m_page->SetButtons(StartupPage::BUTTON_NONE);
        m_actionSequence->Next();
    }

    void StartupPageController::OnPageLoad()
    {
        LOG_INFO("StartupPage => OnPageLoad()");
    }

    void StartupPageController::OnPageShow()
    {
        LOG_INFO("StartupPage => OnPageShow()");
        connect(m_serviceManager, &Base::ServiceManager::signalLoaded, 
            this, &StartupPageController::onLoaded, Qt::UniqueConnection);
        connect(m_serviceManager, &Base::ServiceManager::signalLoadError, 
            this, &StartupPageController::onLoadError, Qt::UniqueConnection);
        connect(m_configManager, &Extra::ConfigManager::signalLoadDeviceSuccess,
            this, &StartupPageController::onLoadDeviceSuccess, Qt::UniqueConnection);
        connect(m_configManager, &Extra::ConfigManager::signalLoadDeviceError,
            this, &StartupPageController::onLoadDeviceError, Qt::UniqueConnection);
        connect(m_configManager, &Extra::ConfigManager::signalLoadCacheSuccess,
            this, &StartupPageController::onLoadCacheSuccess, Qt::UniqueConnection);
        connect(m_configManager, &Extra::ConfigManager::signalLoadCacheError,
            this, &StartupPageController::onLoadCacheError, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalCheckUpdateSuccess,
            this, &StartupPageController::onCheckUpdateSuccess, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalCheckUpdateError,
            this, &StartupPageController::onCheckUpdateError, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalDownloadPackageSuccess,
            this, &StartupPageController::onDownloadPackageSuccess, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalDownloadPackageError,
            this, &StartupPageController::onDownloadPackageError, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalDownloadPackageProgress,
            this, &StartupPageController::onDownloadPackageProgress, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalUpdatePackageSuccess,
            this, &StartupPageController::onUpdatePackageSuccess, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalUpdatePackageError,
            this, &StartupPageController::onUpdatePackageError, Qt::UniqueConnection);
        connect(m_packageManager, &Extra::PackageManager::signalUpdatePackageProgress,
            this, &StartupPageController::onUpdatePackageProgress, Qt::UniqueConnection);
        
        InitUI();
        m_actionSequence->Next();
    }

    void StartupPageController::OnPageHide()
    {
        LOG_INFO("StartupPage => OnPageHide()");
        disconnect(m_serviceManager, &Base::ServiceManager::signalLoaded, 
            this, &StartupPageController::onLoaded);
        disconnect(m_serviceManager, &Base::ServiceManager::signalLoadError, 
            this, &StartupPageController::onLoadError);
        disconnect(m_configManager, &Extra::ConfigManager::signalLoadDeviceSuccess,
            this, &StartupPageController::onLoadDeviceSuccess);
        disconnect(m_configManager, &Extra::ConfigManager::signalLoadDeviceError,
            this, &StartupPageController::onLoadDeviceError);
        disconnect(m_configManager, &Extra::ConfigManager::signalLoadCacheSuccess,
            this, &StartupPageController::onLoadCacheSuccess);
        disconnect(m_configManager, &Extra::ConfigManager::signalLoadCacheError,
            this, &StartupPageController::onLoadCacheError);
        disconnect(m_packageManager, &Extra::PackageManager::signalCheckUpdateSuccess,
            this, &StartupPageController::onCheckUpdateSuccess);
        disconnect(m_packageManager, &Extra::PackageManager::signalCheckUpdateError,
            this, &StartupPageController::onCheckUpdateError);
        disconnect(m_packageManager, &Extra::PackageManager::signalDownloadPackageSuccess,
             this, &StartupPageController::onDownloadPackageSuccess);
        disconnect(m_packageManager, &Extra::PackageManager::signalDownloadPackageError,
             this, &StartupPageController::onDownloadPackageError);
        disconnect(m_packageManager, &Extra::PackageManager::signalDownloadPackageProgress,
             this, &StartupPageController::onDownloadPackageProgress);
        disconnect(m_packageManager, &Extra::PackageManager::signalUpdatePackageSuccess,
             this, &StartupPageController::onUpdatePackageSuccess);
        disconnect(m_packageManager, &Extra::PackageManager::signalUpdatePackageError,
             this, &StartupPageController::onUpdatePackageError);
        disconnect(m_packageManager, &Extra::PackageManager::signalUpdatePackageProgress,
             this, &StartupPageController::onUpdatePackageProgress);
    }

    void StartupPageController::OnPageUnload()
    {
        LOG_INFO("StartupPage => OnPageUnload()");
    }

    void StartupPageController::InitUI()
    {
        m_page->ResetInfos();
        m_page->SetButtons(StartupPage::BUTTON_NONE);
        m_actionSequence->SeekTo(ACTION_SYS_LOAD_SERVICE);
    }

    void StartupPageController::LoadService()
    {
       m_page->AddInfo(QStringLiteral("System Services"), QStringLiteral("Loading..."));

       // 加载开机启动服务
       m_serviceManager->Load(Base::ServiceManager::StartupOption::BOOT);
    }

    void StartupPageController::CheckNetwork()
    {
        auto hostInfo = m_urlManager->GetGWHostInfo();

        // 网络已连接
        if(m_connectivityManager->IsOnlineServiceConnected())
        {
            m_page->AddInfo(QStringLiteral("Network Connection"), QString("(%1)Normal").arg(hostInfo.host));
            m_actionSequence->Next();
            return;
        }

        // 网络未连接,等待3s再检查
        m_page->AddInfo(QStringLiteral("Network Connection"), QString("(%1)Checking...").arg(hostInfo.host));
        QTimer::singleShot(3000, this, [this, hostInfo] {     
            if(m_connectivityManager->IsOnlineServiceConnected())
            {
                m_page->AddInfo(QStringLiteral("Network Connection"), QString("(%1)Normal").arg(hostInfo.host));
                m_actionSequence->Next();
                return;
            }
            else
            {
                m_page->UpdateInfo(QStringLiteral("Network Connection"), QString("(%1)Network connection failed. Please contact IT Help.").arg(hostInfo.host));
                if(m_configManager->Verify())
                    m_page->SetButtons(StartupPage::Buttons(StartupPage::BUTTON_START | StartupPage::BUTTON_RETRY));
                else
                    m_page->SetButtons(StartupPage::BUTTON_RETRY);
            }
        });
    }

    void StartupPageController::GetDeviceInfo()
    {
        m_page->AddInfo(QStringLiteral("Fetch Configuration"), QStringLiteral("Fetching..."));
        m_configManager->Load(Extra::ConfigManager::DEVICE);
    }

    void StartupPageController::CheckUpdate()
    {
        m_page->AddInfo(QStringLiteral("Application Version"), Core::GlobalData::Get()->GetConfig().version.string);
        m_packageManager->CheckUpdate();
    }

    void StartupPageController::onAllActionFinished()
    {
        // 启动http服务
        Base::GetService<Extra::HttpServerManager>()->StartServer();

        // 上报程序版本号
        m_packageManager->ReportVersion();

        // 开始远端连接心跳
        Base::GetService<Extra::RemoteManager>()->StartHeartbeat();

        // 上传程序遗留的dump文件
        Base::GetService<Extra::RemoteManager>()->UploadDumpFile();

        // 跳转主页面
        UI::Application::GetMainWindow()->LoadPage("mainPage");

        // 最小化托盘
        UI::Application::GetMainWindow()->Tray();
    }

    void StartupPageController::onLoaded(Base::ServiceManager::StartupOption option)
    {
        // 延迟加载服务不处理
        if(option == Base::ServiceManager::LAZY)
            return;

        m_page->UpdateInfo(QStringLiteral("System Services"), QStringLiteral("Completed"));
        m_actionSequence->Next();
    }

    void StartupPageController::onLoadError(const QString& message)
    {
        m_page->UpdateInfo(QStringLiteral("System Services"), message);
        m_page->SetButtons(StartupPage::BUTTON_RETRY);
    }

    void StartupPageController::onLoadDeviceSuccess()
    {
        const auto& device = Core::GlobalData::Get()->GetConfig().device;
        m_page->RemoveInfo(QStringLiteral("Fetch Configuration"));
        m_page->AddInfo(QStringLiteral("Current Time"), Base::System::GetCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        m_page->AddInfo(QStringLiteral("MAC"), Base::System::GetMAC());
        m_page->AddInfo(QStringLiteral("POS Server ID"), device.deviceId);
        m_page->AddInfo(QStringLiteral("Store"), device.storeName);

        m_actionSequence->Next();
    }

    void StartupPageController::onLoadDeviceError(const QString& code, const QString& message)
    {
        m_page->UpdateInfo(QStringLiteral("Fetch Configuration"), message);

        // 未创建设备
        if(code == ERROR_CODE_DEVICE_INFO)
        {
            UI::Application::GetMainWindow()->ShowMessageBox(QStringLiteral("Message"), QStringLiteral("The device has not been configured, please register the device first."));
        }

        // 设备停用或者未创建设备或者设备不匹配或者配置文件校验失败
        if(code == ERROR_CODE_DEVICE_STOP || code == ERROR_CODE_DEVICE_INFO 
            || code == ERROR_CODE_DEVICE_NOT_MATCH || !m_configManager->Verify())
            m_page->SetButtons(StartupPage::BUTTON_RETRY);
        else
            m_page->SetButtons(StartupPage::Buttons(StartupPage::BUTTON_START | StartupPage::BUTTON_RETRY)) ;
    }

    void StartupPageController::onLoadCacheSuccess()
    {
        UI::Application::GetMainWindow()->CloseLoading();
        m_page->SetButtons(StartupPage::BUTTON_NONE);
        onAllActionFinished();
    }

    void StartupPageController::onLoadCacheError(const QString& message)
    {
        UI::Application::GetMainWindow()->CloseLoading();
        m_page->SetButtons(StartupPage::BUTTON_RETRY);
        UI::Application::GetMainWindow()->ShowMessageBox(QStringLiteral("Message"), message);
    }

    void StartupPageController::onCheckUpdateSuccess(bool found, const Extra::PackageManager::PatchInfo& info)
    {
        if(found)
        {
            QString msg = QStringLiteral("%1<font color='#FF0000'> (New version %2 available, downloading...)</font>")
                .arg(Core::GlobalData::Get()->GetConfig().version.string)
                .arg(info.version);
            m_page->UpdateInfo(QStringLiteral("Application Version"), msg);

            //下载更新包
            m_packageManager->DownloadPackage(info);
        }
        else
        {
            m_actionSequence->Next();
        }
    }

    void StartupPageController::onCheckUpdateError(const QString& message)
    {
        QString msg = QString("<font color='#FF0000'>%1</font>").arg(message);
        m_page->UpdateInfo(QStringLiteral("Application Version"), msg);
        m_page->SetButtons(StartupPage::Buttons(StartupPage::BUTTON_RETRY | StartupPage::BUTTON_IGNORE_UPDATE));
    }

    void StartupPageController::onDownloadPackageSuccess(const Extra::PackageManager::PatchInfo& info, const QString& filePath)
    {
        QString msg = QStringLiteral("%1<font color='#FF0000'> (New version %2 available, upgrading...)</font>")
            .arg(Core::GlobalData::Get()->GetConfig().version.string).arg(info.version);
        m_page->UpdateInfo(QStringLiteral("Application Version"), msg);

        m_packageManager->UpdatePackage(info, filePath);
    }

    void StartupPageController::onDownloadPackageError(const Extra::PackageManager::PatchInfo& info, const QString& message)
    {
        QString msg = QStringLiteral("%1<font color='#FF0000'>(New version %2 available, upgrade failed, %3)</font>")
            .arg(Core::GlobalData::Get()->GetConfig().version.string)
            .arg(info.version).arg(message);

        m_page->UpdateInfo(QStringLiteral("Application Version"), msg);

        if(info.isForced)
        {
            // 如果开启强制升级，则升级失败后不允许跳过
            m_page->SetButtons(StartupPage::BUTTON_RETRY);
        }
        else
        {
            m_page->SetButtons(StartupPage::Buttons(StartupPage::BUTTON_RETRY | StartupPage::BUTTON_IGNORE_UPDATE));
        }
    }

    void StartupPageController::onDownloadPackageProgress(const Extra::PackageManager::PatchInfo& info, qint64 bytesReceived, qint64 bytesTotal)
    {
        QString tick = QString::number((int)(bytesReceived * 100.0 / bytesTotal));
        QString msg = QStringLiteral("%1<font color='#FF0000'>(New version %2 detected. Downloading... %3%)</font>")
            .arg(Core::GlobalData::Get()->GetConfig().version.string)
            .arg(info.version)
            .arg(tick);

        m_page->UpdateInfo(QStringLiteral("Application Version"), msg);
    }

    void StartupPageController::onUpdatePackageSuccess(Extra::PackageManager::PatchInfo info)
    {
        QString msg = QStringLiteral("%1<font color='#FF0000'>(Upgraded successfully, restarting...)</font>")
            .arg(info.version);

        m_page->UpdateInfo(QStringLiteral("Application Version"), msg);

        // 重启程序
        QTimer::singleShot(1000, this, [] {
            Core::GlobalData::Get()->GetConfig().exit.restart = true;
            UI::Application::Get()->Exit();
        });
    }

    void StartupPageController::onUpdatePackageError(Extra::PackageManager::PatchInfo info, Extra::PackageManager::PatchError code, const QString& message)
    {
        QString msg = QStringLiteral("%1<font color='#FF0000'>(New version %2 available, upgrade failed, %3)</font>")
            .arg(Core::GlobalData::Get()->GetConfig().version.string)
            .arg(info.version)
            .arg(message);

        m_page->UpdateInfo(QStringLiteral("Application Version"), msg);

        if(info.isForced)
        {
            // 如果开启强制升级，则升级失败后不允许跳过
            m_page->SetButtons(StartupPage::BUTTON_RETRY);
        }
        else
        {
            m_page->SetButtons(StartupPage::Buttons(StartupPage::BUTTON_RETRY | StartupPage::BUTTON_IGNORE_UPDATE));
        }
    }

    void StartupPageController::onUpdatePackageProgress(Extra::PackageManager::PatchInfo info, const QString& message)
    {
        QString msg = QStringLiteral("%1<font color='#FF0000'>(New version %2 available, %3...)</font>")
            .arg(Core::GlobalData::Get()->GetConfig().version.string)
            .arg(info.version)
            .arg(message);

        m_page->UpdateInfo(QStringLiteral("Application Version"), msg);
    }

} // namespace UI