﻿#pragma once

#include "ModuleBase/Service/AsyncService.h"
#include "ModuleData/Core/GlobalStructData.h"

namespace Extra
{
    /** 配置服务*/
    class ConfigManager : public Base::AsyncService
    {
        Q_OBJECT

    public:
       
        ConfigManager(QObject* parent = nullptr);
        virtual ~ConfigManager();

        /** 获取配置商品列表 */
        Core::ConfigWarePtrList& GetConfigWares() { return m_configWares; }

        /** 获取配置商品列表 */
        void UploadWareImage(int index, const QString& imgPath);
    protected:
        virtual bool OnStart() override;
        virtual void OnStop() override;

        /** APP文件缓存写入 */
        bool WriteAppCache(const QString& fileName) const;

        /** 文件缓存读取 */
        bool ReadAppCache(const QString& fileName) const;

        Q_INVOKABLE void onUploadWareImage(int index, const QString& imgPath);
    public Q_SLOTS:
        /** 增加配置商品信号处理 */
        void onAddConfigWareSuccess(Core::ConfigWarePtr item);

        /** 更新配置商品信号处理 */
        void onUpdateConfigWareSuccess(Core::ConfigWarePtr item);

    Q_SIGNALS:
        /** 加载配置商品列表信号 */
        void signalLoadConfigWaresSuccess();

        /** 上传商品图片信号 */
        void signalUploadConfigWareImageError(const QString& msg);
        void signalUploadConfigWareImageSuccess(int index, Core::ConfigWarePtr item);

        /** 修改配置商品成功信号 */
        void  signalAddConfigWareSuccess(Core::ConfigWarePtr item);
        void  signalUpdateConfigWareSuccess(Core::ConfigWarePtr item);
    private:

        // 配置商品列表
        Core::ConfigWarePtrList m_configWares;
    };
}
