#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include "ConfigManager.h"
#include "ModuleBase/Http/Client/Client.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleData/Core/Config.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleProtocol/Client/GalaxyDeviceInfoRequest.h"
#include "ModuleProtocol/Client/GalaxyDeviceInfoResponse.h"
#include "UrlManager.h"

namespace Extra
{
    ConfigManager::ConfigManager(QObject* parent)
        : AsyncService(parent)
    {
        m_loadStatus[Type::DEVICE] = LoadStatus::INVALID;
    }

    ConfigManager::~ConfigManager()
    {
    }

    void ConfigManager::Load(Types types)
    {
        if(types.testFlag(Type::DEVICE))
            QMetaObject::invokeMethod(this, "onLoadDevice", Qt::QueuedConnection);
    }

    void ConfigManager::LoadCache()
    {
        QMetaObject::invokeMethod(this, "onLoadCache", Qt::QueuedConnection);
    }

    bool ConfigManager::Verify()
    {
        if(m_loadStatus[Type::DEVICE] == LoadStatus::INVALID && !QFile::exists(DEVICE_CACHE_FILE))
            return false;

        return true;
    }

    bool ConfigManager::OnStart()
    {
        if(LoadVersionFile(VERSION_FILE))
        {
            if(!QFile::exists(APP_CACHE_FILE))
                return WriteAppCache(APP_CACHE_FILE);
            else
                return ReadAppCache(APP_CACHE_FILE);
        }

        return false;
    }

    void ConfigManager::OnStop()
    {
    }

    bool ConfigManager::LoadVersionFile(const QString& fileName)
    {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            LOG_ERROR(QString("%1 file open failed").arg(fileName).toStdString());
            return false;
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if(parseError.error != QJsonParseError::NoError)
        {
            LOG_ERROR(QString("%1 json parse failed").arg(fileName).toStdString());
            return false;
        }

        auto& version = Core::GlobalData::Get()->GetConfig().version;
        QJsonObject root = doc.object();
        version.string = root["version"].toString();
        version.code = root["code"].toString();
        version.date = root["date"].toString();
        version.env = root["env"].toString();
        return true;
    }

    void ConfigManager::Parse(Type type, QSharedPointer<Http::Request> request, QSharedPointer<Http::Response> response) const
    {
        auto& config = Core::GlobalData::Get()->GetConfig();

        if(type == Type::DEVICE)
        {
            auto rsp = qSharedPointerCast<Http::GalaxyDeviceInfoResponse>(response);
            Q_ASSERT_X(rsp != nullptr, "Parse()", "response is invalid");

            auto data = rsp->GetResult();

            config.device.deviceId = QString::number(data.id);
            config.device.deviceCode = data.externalCode;
            config.device.venderId = QString::number(data.tenantId);
            config.device.venderName = data.tenantName;
            config.device.storeId = QString::number(data.orgId);
            config.device.storeCode = data.orgCode;
            config.device.storeName = data.orgName;
            config.device.storeAddress = data.address;
            config.device.storeType = QString::number(data.orgType);
            config.device.ip = data.customField.inStoreServerIp;
            config.device.port = data.customField.inStoreServerPort.toInt();

            config.device.posTerminals.clear();
            for(auto pos : data.winPosSimpleEquipmentVoList)
            {
                Core::CommonConfig::Device::Terminal terminal;
                terminal.id = QString::number(pos.id);
                terminal.code = pos.externalCode;
                terminal.deskNo = pos.customField["deskNum"].toString();
                config.device.posTerminals[terminal.id] = terminal;
            }

            config.device.pvtTerminals.clear();
            for(auto pvt : data.pvtSimpleEquipmentVoList)
            {
                Core::CommonConfig::Device::Terminal terminal;
                terminal.id = QString::number(pvt.id);
                terminal.code = pvt.externalCode;
                terminal.deskNo = pvt.customField["deskNum"].toString();
                config.device.pvtTerminals[terminal.id] = terminal;
            }
        }
        else
        {
            Q_ASSERT_X(0, "Parse()", "type is invalid");
        }
    }

    bool ConfigManager::WriteCache(Type type) const
    {
        if(type == Type::DEVICE)
        {
            QFile file(DEVICE_CACHE_FILE);
            if(!file.open(QIODevice::WriteOnly))
                return false;

            auto device = Core::GlobalData::Get()->GetConfig().device;
            QJsonObject root;
            root["deviceId"] = device.deviceId;
            root["deviceCode"] = device.deviceCode;
            root["venderId"] = device.venderId;
            root["venderName"] = device.venderName;
            root["storeId"] = device.storeId;
            root["storeCode"] = device.storeCode;
            root["storeName"] = device.storeName;
            root["storeAddress"] = device.storeAddress;
            root["storeType"] = device.storeType;
            root["ip"] = device.ip;
            root["port"] = device.port;

            QJsonArray posTerminals;
            for(auto pos : device.posTerminals)
            {
                QJsonObject terminal;
                terminal["id"] = pos.id;
                terminal["code"] = pos.code;
                terminal["deskNo"] = pos.deskNo;
                posTerminals << terminal;
            }
            root["posTerminals"] = posTerminals;

            QJsonArray pvtTerminals;
            for(auto pvt : device.pvtTerminals)
            {
                QJsonObject terminal;
                terminal["id"] = pvt.id;
                terminal["code"] = pvt.code;
                terminal["deskNo"] = pvt.deskNo;
                pvtTerminals << terminal;
            }
            root["pvtTerminals"] = pvtTerminals;

            QJsonDocument doc;
            doc.setObject(root);
            file.write(doc.toJson(QJsonDocument::Indented));
            file.flush();
            file.close();
            return true;
        }
        else
        {
            Q_ASSERT_X(0, "WriteCache()", "type is invalid");
        }
    
        return false;
    }

    bool ConfigManager::ReadCache(Type type) const
    {
        if(type == Type::DEVICE)
        {
            QFile file(DEVICE_CACHE_FILE);
            if(!file.open(QIODevice::ReadOnly))
            {
                LOG_ERROR(QString("%1 file open failed").arg(DEVICE_CACHE_FILE).toStdString());
                return false;
            }

            QByteArray data = file.readAll();
            file.close();

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
            if(parseError.error != QJsonParseError::NoError)
            {
                LOG_ERROR(QString("%1 json parse failed").arg(DEVICE_CACHE_FILE).toStdString());
                return false;
            }

            auto& device = Core::GlobalData::Get()->GetConfig().device;
            QJsonObject root = doc.object();
            device.deviceId = root["deviceId"].toString();
            device.deviceCode = root["deviceCode"].toString();
            device.venderId = root["venderId"].toString();
            device.venderName = root["venderName"].toString();
            device.storeId = root["storeId"].toString();
            device.storeCode = root["storeCode"].toString();
            device.storeName = root["storeName"].toString();
            device.storeAddress = root["storeAddress"].toString();
            device.storeType = root["storeType"].toString();
            device.ip = root["ip"].toString();
            device.port = root["port"].toInt();

            device.posTerminals.clear();
            QJsonArray posTerminals = root["posTerminals"].toArray();
            for(auto terminal : posTerminals)
            {
                Core::CommonConfig::Device::Terminal pos;
                pos.id = terminal.toObject()["id"].toString();
                pos.code = terminal.toObject()["code"].toString();
                pos.deskNo = terminal.toObject()["deskNo"].toString();
                device.posTerminals[pos.id] = pos;
            }

            device.pvtTerminals.clear();
            QJsonArray pvtTerminals = root["pvtTerminals"].toArray();
            for(auto terminal : pvtTerminals)
            {
                Core::CommonConfig::Device::Terminal pvt;
                pvt.id = terminal.toObject()["id"].toString();
                pvt.code = terminal.toObject()["code"].toString();
                pvt.deskNo = terminal.toObject()["deskNo"].toString();
                device.pvtTerminals[pvt.id] = pvt;
            }

            return true;
        }
        else
        {
            Q_ASSERT_X(0, "WriteCache()", "type is invalid");
        }

        return false;
    }

    bool ConfigManager::WriteAppCache(const QString& fileName) const
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            LOG_ERROR(QString("%1 file open failed").arg(fileName).toStdString());
            return false;
        }

        auto& app = Core::GlobalData::Get()->GetConfig().app;
        auto version = Core::GlobalData::Get()->GetConfig().version;

        app.render = Core::Render::RENDER_HARDWARE;
        app.lang = "en_US";
        if(version.env.compare("DEV", Qt::CaseInsensitive) == 0)
            app.env = Core::ServiceEnvironment::ENV_DEV;
        else if(version.env.compare("UAT", Qt::CaseInsensitive) == 0)
            app.env = Core::ServiceEnvironment::ENV_UAT;
        else if(version.env.compare("PRO", Qt::CaseInsensitive) == 0)
            app.env = Core::ServiceEnvironment::ENV_PRO;
        else
            app.env = Core::ServiceEnvironment::ENV_PRO;

        QJsonObject root;
        if(app.render == Core::Render::RENDER_SOFTWARE)
            root["render"] = "software";
        else if(app.render == Core::Render::RENDER_HARDWARE)
            root["render"] = "hardware";
        else
            Q_ASSERT_X(0, "WriteApp()", "render type is invalid");

        if(app.env == Core::ServiceEnvironment::ENV_DEV)
            root["env"] = "DEV";
        else if(app.env == Core::ServiceEnvironment::ENV_UAT)
            root["env"] = "UAT";
        else if(app.env == Core::ServiceEnvironment::ENV_PRO)
            root["env"] = "PRO";
        else
            Q_ASSERT_X(0, "WriteApp()", "env type is invalid");

        root["lang"] = app.lang;

        QJsonDocument doc;
        doc.setObject(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.flush();
        file.close();
        return true;
    }

    bool ConfigManager::ReadAppCache(const QString& fileName) const
    {
        auto& app = Core::GlobalData::Get()->GetConfig().app;

        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            LOG_ERROR(QString("%1 file open failed").arg(fileName).toStdString());
            return false;
        }
        QByteArray data = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if(parseError.error != QJsonParseError::NoError)
        {
            LOG_ERROR(QString("%1 json parse failed").arg(fileName).toStdString());
            return false;
        }

        QJsonObject root = doc.object();

        if(root["render"].toString().compare("software", Qt::CaseInsensitive) == 0)
            app.render = Core::Render::RENDER_SOFTWARE;
        else if(root["render"].toString().compare("hardware", Qt::CaseInsensitive) == 0)
            app.render = Core::Render::RENDER_HARDWARE;
        else
            Q_ASSERT_X(0, "ReadApp()", "render type is invalid");

        if(root["env"].toString().compare("DEV", Qt::CaseInsensitive) == 0)
            app.env = Core::ServiceEnvironment::ENV_DEV;
        else if(root["env"].toString().compare("UAT", Qt::CaseInsensitive) == 0)
            app.env = Core::ServiceEnvironment::ENV_UAT;
        else if(root["env"].toString().compare("PRO", Qt::CaseInsensitive) == 0)
            app.env = Core::ServiceEnvironment::ENV_PRO;
        else
            Q_ASSERT_X(0, "ReadApp()", "env type is invalid");

        app.lang = root["lang"].toString();

        return true;
    }

    void ConfigManager::onLoadDevice()
    {
        auto urlParam = Base::GetService<UrlManager>()->GetUrl(UrlManager::GALAXY_DEVICE_INFO);
        auto request = QSharedPointer<Http::GalaxyDeviceInfoRequest>::create();
        auto response = QSharedPointer<Http::GalaxyDeviceInfoResponse>::create();

        request->SetUrl(urlParam.url);
        request->SetTimeout(urlParam.timeout);
        Http::Client::Get()->ProcessRequest(request, response);

        if(!response->IsSuccess())
        {
            emit signalLoadDeviceError(response->GetError(), response->GetErrorMessage());
            LOG_INFO(QStringLiteral("加载设备配置失败").toStdString());
            return;
        }

        // 设备停用
        if(response->GetResult().status == 2)
        {
            emit signalLoadDeviceError(QStringLiteral("GLXGATEWAY2008"), QStringLiteral("Device not found, please enable the device first."));
            LOG_INFO(QStringLiteral("设备停用").toStdString());
            return;
        }

        Parse(Type::DEVICE, request, response);
        WriteCache(Type::DEVICE);
        m_loadStatus[Type::DEVICE] = LoadStatus::LOADED_FROM_CLOUD;
        emit signalLoadDeviceSuccess();
        LOG_INFO(QStringLiteral("加载设备配置成功 [云端加载]").toStdString());
    }

    void ConfigManager::onLoadCache()
    {
        if(m_loadStatus[DEVICE] == LoadStatus::INVALID)
        {
            if(ReadCache(Type::DEVICE))
            {
                m_loadStatus[Type::DEVICE] = LoadStatus::LOADED_FROM_CACHE;
                LOG_INFO(QStringLiteral("加载设备配置成功 [缓存加载]").toStdString());
            }
            else
            {
                emit signalLoadCacheError(QStringLiteral("Failed to load device configuration cache."));
                LOG_INFO(QStringLiteral("加载设备配置失败 [缓存加载]").toStdString());
                return;
            }
        }

        emit signalLoadCacheSuccess();
    }
}