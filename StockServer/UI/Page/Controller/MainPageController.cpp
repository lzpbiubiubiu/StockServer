#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/GlobalData.h"
#include "Extras/HttpServerManager.h"
#include "MainPageController.h"

namespace UI
{
    MainPageController::MainPageController(Page* parent)
        : PageController(parent)
    {

        m_page = GetPage<MainPage>();

    }

    MainPageController::~MainPageController()
    {
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

} // namespace UI