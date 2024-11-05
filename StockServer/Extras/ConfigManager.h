#pragma once

#include "ModuleBase/Service/AsyncService.h"
#include "ModuleBase/Http/Client/Request.h"
#include "ModuleBase/Http/Client/Response.h"

namespace Extra
{
    /** 配置服务，实现核心配置的拉取 */
    class ConfigManager : public Base::AsyncService
    {
        Q_OBJECT

    public:
        /** 配置类型 */
        enum Type
        {
            /** 设备信息 */
            DEVICE = 0
        };
        Q_ENUM(Type)
        Q_DECLARE_FLAGS(Types, Type)

        /** 配置加载状态 */
        enum LoadStatus
        {
            /** 未加载 */
            INVALID,

            /** 云端加载 */
            LOADED_FROM_CLOUD,

            /** 缓存加载 */
            LOADED_FROM_CACHE
        };
        Q_ENUM(LoadStatus)

        ConfigManager(QObject* parent = nullptr);
        virtual ~ConfigManager();

        /** 加载配置(云端加载) */
        void Load(Types types);

        /** 加载配置(缓存文件加载) */
        void LoadCache();

        /** 校验配置完整 */
        bool Verify();

    protected:
        virtual bool OnStart() override;
        virtual void OnStop() override;

        /** 加载文件 */
        bool LoadVersionFile(const QString& fileName);

        /** 解析配置并同步到全局内存 */
        void Parse(Type type, QSharedPointer<Http::Request> request, QSharedPointer<Http::Response> response) const;

        /** 全局内存写缓存 */
        bool WriteCache(Type type) const;

        /** 全局内存读缓存 */
        bool ReadCache(Type type) const;

        /** 写App.json */
        bool WriteAppCache(const QString& fileName) const;

        /** 读App.json */
        bool ReadAppCache(const QString& fileName) const;

        /** 加载设备信息 */
        Q_INVOKABLE void onLoadDevice();

        /** 缓存加载配置信息 */
        Q_INVOKABLE void onLoadCache();

    Q_SIGNALS:
        /** 加载设备信息 */
        void signalLoadDeviceSuccess();
        void signalLoadDeviceError(const QString& code, const QString& message);

        /** 缓存加载配置信息 */
        void signalLoadCacheSuccess();
        void signalLoadCacheError(const QString& message);

    private:
        QHash<Type, LoadStatus> m_loadStatus;
    };
}
