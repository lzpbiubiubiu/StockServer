#pragma once

#include "PageController.h"
#include "UI/Page/MainPage.h"
#include "ModuleBase/Service/ServiceManager.h"

namespace UI
{
    class MainPageController : public PageController
    {
        Q_OBJECT

    public:
        MainPageController(Page* parent);

        virtual ~MainPageController();

    protected:
        /** 页面加载时回调 */
        virtual void OnPageLoad() override;

        /** 页面显示前回调 */
        virtual void OnPageShow() override;

        /** 页面隐藏前回调 */
        virtual void OnPageHide() override;

        /** 页面卸载前回调 */
        virtual void OnPageUnload() override;

    protected Q_SLOTS:

    private:
        // 页面
        MainPage* m_page = nullptr;
    };
}
