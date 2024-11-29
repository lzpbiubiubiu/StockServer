#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/GlobalData.h"
#include "Extras/HttpServerManager.h"
#include "Extras/ConfigManager.h"
#include "MainPageController.h"

namespace UI
{
    MainPageController::MainPageController(Page* parent)
        : PageController(parent)
    {
        m_page = GetPage<MainPage>();
        m_configManager = Base::GetService<Extra::ConfigManager>();
        connect(m_configManager, &Extra::ConfigManager::signalLoadConfigWaresSuccess, this, &MainPageController::onLoadConfigWaresSuccess);
        connect(m_configManager, &Extra::ConfigManager::signalUploadConfigWareImageError, this, &MainPageController::onUploadConfigWareImageError);
        connect(m_configManager, &Extra::ConfigManager::signalUploadConfigWareImageSuccess, this, &MainPageController::onUploadConfigWareImageSuccess);
        connect(m_configManager, &Extra::ConfigManager::signalAddConfigWareSuccess, this, &MainPageController::onAddConfigWareSuccess);
        connect(m_configManager, &Extra::ConfigManager::signalUpdateConfigWareSuccess, this, &MainPageController::onUpdateConfigWareSuccess);
        
    }

    MainPageController::~MainPageController()
    {
    }

    void MainPageController::itemClicked(int index)
    {
        m_page->SetCurrentWareInfo(index);
    }

    void MainPageController::uploadWareImage(int wareIndex, const QString& imgPath)
    {
        UI::Application::GetMainWindow()->ShowLoading("");
        QString realPath = QUrl(imgPath).toLocalFile();
        m_configManager->UploadWareImage(wareIndex, realPath);
    }

    void MainPageController::OnPageLoad()
    {
        LOG_INFO("MainPage => OnPageLoad()");
        // 加载开机启动服务
        Base::ServiceManager::Get()->Load(Base::ServiceManager::StartupOption::BOOT);
        // 启动http服务
        Base::GetService<Extra::HttpServerManager>()->StartServer();
    }

    void MainPageController::OnPageShow()
    {
        LOG_INFO("MainPage => OnPageShow()");
      
    }

    void MainPageController::OnPageHide()                      
    {
        LOG_INFO("MainPage => OnPageHide()");

    }

    void MainPageController::OnPageUnload()
    {
        LOG_INFO("MainPage => OnPageUnload()");
    }

    void MainPageController::onUploadConfigWareImageSuccess(int index, Core::ConfigWarePtr item)
    {
        UI::Application::GetMainWindow()->CloseLoading();
        m_page->GetStockWarePanel()->OnUpdateItem(index, item);
        m_page->SetCurrentWareInfo(index);
    }

    void MainPageController::onUploadConfigWareImageError(const QString& msg)
    {
        UI::Application::GetMainWindow()->CloseLoading();
        UI::Application::Get()->GetMainWindow()->ShowMessageBox("Message", msg);
    }

    void MainPageController::onAddConfigWareSuccess(Core::ConfigWarePtr item)
    {
        if(!item)
            return;
        m_page->GetStockWarePanel()->OnAddItem(item);
    }

    void MainPageController::onUpdateConfigWareSuccess(Core::ConfigWarePtr item)
    {
        if(!item)
            return;
        auto stockWarePanel = m_page->GetStockWarePanel();
        auto index = stockWarePanel->GetModel()->GetIndex(item->code);
        stockWarePanel->OnUpdateItem(index, item);
    }

    void MainPageController::onLoadConfigWaresSuccess()
    {
        m_page->GetStockWarePanel()->InitWares();
    }

} // namespace UI