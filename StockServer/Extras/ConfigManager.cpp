#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include "ConfigManager.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Database/Base/DmlBase.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleData/Core/ConvertUtil.h"
#include "ModuleData/Entity/ConfigWareEntity.h"
#include "UrlManager.h"

namespace Extra
{
    ConfigManager::ConfigManager(QObject* parent)
        : AsyncService(parent)
    {

    }

    ConfigManager::~ConfigManager()
    {
    }

    void ConfigManager::UploadWareImage(int index, const QString& imgPath)
    {
        QMetaObject::invokeMethod(this, "onUploadWareImage", Q_ARG(int, index), Q_ARG(QString, imgPath));
    }

    bool ConfigManager::OnStart()
    {
        // 查询商品信息
        auto db = Base::GetService<Extra::DatabaseManager>()->GetStockServerDatabase();
        Sql::DmlBase<Sql::ConfigWareEntity> dml;
        dml.SetDatabase(db);
        Sql::ConfigWareEntityPtrList entities;
        QString sqlError;

        if(!dml.ListAll(entities, sqlError))
        {
            Q_ASSERT_X(0, "", sqlError.toUtf8());
            LOG_ERROR(QStringLiteral("查询商品失败：%1").arg(sqlError).toStdString());
            return false;
        }

        m_configWares = Core::ConvertUtil::FromEntityList(entities);

        emit signalLoadConfigWaresSuccess();
        // 读取配置文件
        if(!QFile::exists(APP_CACHE_FILE))
            return WriteAppCache(APP_CACHE_FILE);
        else
            return ReadAppCache(APP_CACHE_FILE);

        return false;
    }

    void ConfigManager::OnStop()
    {
    }

    bool ConfigManager::WriteAppCache(const QString& fileName) const
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            LOG_ERROR(QString("%1 file open failed").arg(fileName).toStdString());
            return false;
        }

        const auto& app = Core::GlobalData::Get()->GetConfig().app;
        QJsonObject root;
        if(app.render == Core::Render::RENDER_SOFTWARE)
            root["render"] = "software";
        else if(app.render == Core::Render::RENDER_HARDWARE)
            root["render"] = "hardware";
        else
            Q_ASSERT_X(0, "WriteApp()", "render type is invalid");
        root["port"] = app.port;
        root["ip"] = app.ip;

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

        app.port = root["port"].toVariant().toLongLong();
        app.ip = root["ip"].toString();

        return true;
    }

    void ConfigManager::onUploadWareImage(int index, const QString& imgPath)
    {
        if(index < 0 || index >= m_configWares.size())
        {
            emit signalUploadConfigWareImageError(QStringLiteral("商品不存在"));
            LOG_ERROR(QStringLiteral("商品不存在").toStdString());
            return;
        }
        auto wareItem = m_configWares[index];

        QString targetPath = Base::PathUtil::GetFileStationDir();
        QFileInfo sourceFile(imgPath);
        wareItem->imageFileName = sourceFile.fileName();
        auto sourceDir = sourceFile.absoluteDir();
        if((sourceDir.absolutePath() + "/") != targetPath)
        {
            if(!Base::PathUtil::CopyFileToPath(imgPath, targetPath))
            {
                LOG_ERROR(QStringLiteral("拷贝文件失败，%1, %2").arg(imgPath).arg(targetPath).toStdString());
                emit signalUploadConfigWareImageError(QStringLiteral("商品图片上传失败"));
                return;
            }
        }
        wareItem->imageMd5 = Base::MathUtil::GetFileMD5(targetPath + wareItem->imageFileName);
        // 查询商品信息
        auto db = Base::GetService<Extra::DatabaseManager>()->GetStockServerDatabase();
        Sql::DmlBase<Sql::ConfigWareEntity> dml;
        dml.SetDatabase(db);
        Sql::ConfigWareEntityPtr entitie = Core::ConvertUtil::ToEntity(wareItem);
        QString sqlError;

        if(!dml.UpdateById(entitie, QStringList(), sqlError))
        {
            LOG_ERROR(QStringLiteral("更新商品失败：%1").arg(sqlError).toStdString());
            emit signalUploadConfigWareImageError(QStringLiteral("商品图片上传失败"));
            return;
        }
        emit signalUploadConfigWareImageSuccess(index, wareItem);
    }
}