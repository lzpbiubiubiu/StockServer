#pragma once

#include "ModuleBase/Common/ActionSequence.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "Extras/ConfigManager.h"
#include "Extras/ConnectivityManager.h"
#include "Extras/PackageManager.h"
#include "Extras/UrlManager.h"
#include "PageController.h"
#include "UI/Page/StartupPage.h"

namespace UI
{
    class StartupPageController : public PageController
    {
        Q_OBJECT

    public:
        /** 动作ID */
        enum ActionID
        {
            /** 加载服务 */
            ACTION_SYS_LOAD_SERVICE = 0,

            /** 网络检测 */
            ACTION_SYS_CHECK_NETWORK,

            /** 获取设备信息 */
            ACTION_GET_DEVICE_INFO,

            /** 检查更新 */
            ACTION_CHECK_UPDATE
        };
        Q_ENUM(ActionID)

        StartupPageController(Page* parent);

        virtual ~StartupPageController();

    protected:
        /** 重走启动流程 */
        Q_INVOKABLE void retry();

        /** 退出 */
        Q_INVOKABLE void quit();

        /** 缓存启动 */
        Q_INVOKABLE void start();

        /** 忽略更新 */
        Q_INVOKABLE void ignoreUpdate();

        /** 页面加载时回调 */
        virtual void OnPageLoad() override;

        /** 页面显示前回调 */
        virtual void OnPageShow() override;

        /** 页面隐藏前回调 */
        virtual void OnPageHide() override;

        /** 页面卸载前回调 */
        virtual void OnPageUnload() override;

        /** 初始化UI */
        void InitUI();

        /** 加载服务 */
        void LoadService();

        /** 检测网络 */
        void CheckNetwork();

        /** 获取设备信息 */
        void GetDeviceInfo();

        /** 检查更新 */
        void CheckUpdate();

    protected Q_SLOTS:
        /** 动作序列信号处理 */
        void onAllActionFinished();

        /** 服务初始化信号处理 */
        void onLoaded(Base::ServiceManager::StartupOption option);
        void onLoadError(const QString& message);

        /** 加载设备信息结果 */
        void onLoadDeviceSuccess();
        void onLoadDeviceError(const QString& code, const QString& message);

        /**  加载缓存信息结果 */
        void onLoadCacheSuccess();
        void onLoadCacheError(const QString& message);

        /** 检查更新结果 */
        void onCheckUpdateSuccess(bool found, const Extra::PackageManager::PatchInfo& info);
        void onCheckUpdateError(const QString& message);

        /** 补丁下载结果 */
        void onDownloadPackageSuccess(const Extra::PackageManager::PatchInfo& info, const QString& filePath);
        void onDownloadPackageError(const Extra::PackageManager::PatchInfo& info, const QString& message);
        void onDownloadPackageProgress(const Extra::PackageManager::PatchInfo& info, qint64 bytesReceived, qint64 bytesTotal);

        /** 补丁安装结果 */
        void onUpdatePackageSuccess(Extra::PackageManager::PatchInfo info);
        void onUpdatePackageError(Extra::PackageManager::PatchInfo info, Extra::PackageManager::PatchError code, const QString& message);
        void onUpdatePackageProgress(Extra::PackageManager::PatchInfo info, const QString& message);

    private:
        // 页面
        StartupPage* m_page = nullptr;

        // 动作队列
        Base::ActionSequence* m_actionSequence = nullptr;

        // 服务管理器
        Base::ServiceManager* m_serviceManager = nullptr;

        // 网络连接管理器
        Extra::ConnectivityManager* m_connectivityManager = nullptr;

        // URL管理器
        Extra::UrlManager* m_urlManager = nullptr;

        // 配置管理器
        Extra::ConfigManager* m_configManager = nullptr;

        // 包管理器
        Extra::PackageManager* m_packageManager = nullptr;
    };
}
