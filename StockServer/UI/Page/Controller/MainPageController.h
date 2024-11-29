#pragma once

#include "PageController.h"
#include "UI/Page/MainPage.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "Extras/ConfigManager.h"

namespace UI
{
    class MainPageController : public PageController
    {
        Q_OBJECT

    public:
        MainPageController(Page* parent);

        virtual ~MainPageController();

        /** 商品项点击 */
        Q_INVOKABLE void itemClicked(int index);

        /** 商品商品图片 */
        Q_INVOKABLE void uploadWareImage(int wareIndex, const QString& imgPath);

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
        /** 加载配置商品列表信号处理 */
        void onLoadConfigWaresSuccess();

        /** 上传商品图片信息信号处理*/
        void onUploadConfigWareImageSuccess(int index, Core::ConfigWarePtr item);
        void onUploadConfigWareImageError(const QString& msg);

        /** 增加配置商品信号处理 */
        void onAddConfigWareSuccess(Core::ConfigWarePtr item);

        /** 更新配置商品信号处理 */
        void onUpdateConfigWareSuccess(Core::ConfigWarePtr item);

    private:
        // 页面
        MainPage* m_page = nullptr;

        // 配置管理器
        Extra::ConfigManager* m_configManager = nullptr;
    };
}
