#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QSettings>
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleBase/Util/ZipUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleProtocol/Client/GalaxyReportVersionRequest.h"
#include "ModuleProtocol/Client/GalaxyVersionRequest.h"
#include "ModuleProtocol/Client/GalaxyVersionResponse.h"
#include "CourgetteImpl.h"
#include "PackageManager.h"
#include "UrlManager.h"
#include "UpdateXml.h"

// 自动检查更新时间间隔，默认4小时
static const int MAX_AUTO_CHECK_INTERVAL = 4 * 60 * 60 * 1000;

namespace Extra
{
    PackageManager::PackageManager(QObject* parent)
        : AsyncService(parent)
    {
        qRegisterMetaType<PackageManager::PatchInfo>("PatchInfo");
    }

    PackageManager::~PackageManager()
    {
    }

    void PackageManager::CheckUpdate()
    {
        QMetaObject::invokeMethod(this, "onCheckUpdate");
    }

    void PackageManager::DownloadPackage(const PatchInfo& info)
    {
        CHECK_AND_SET_FROZEN_FLAG();
        QMetaObject::invokeMethod(this, "onDownloadPackage", Q_ARG(PatchInfo, info));
    }

    void PackageManager::UpdatePackage(const PatchInfo& info, const QString& fileName)
    {
        CHECK_AND_SET_FROZEN_FLAG();
        QMetaObject::invokeMethod(this, "onUpdatePackage", Q_ARG(PatchInfo, info), Q_ARG(QString, fileName));
    }

    void PackageManager::ReportVersion()
    {
        QMetaObject::invokeMethod(this, "onReportVersion");
    }

    bool PackageManager::OnStart()
    {
        // 定时器初始化
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &PackageManager::onTimeout);
        m_timer->start(MAX_AUTO_CHECK_INTERVAL);

        return true;
    }

    void PackageManager::OnStop()
    {
        m_timer->disconnect();
        m_timer->stop();
    }

    bool PackageManager::QueryVersion(bool& found, PackageManager::PatchInfo& info, QString& message)
    {
        auto urlParam = Base::GetService<UrlManager>()->GetUrl(UrlManager::GALAXY_VERSION);
        auto request = QSharedPointer<Http::GalaxyVersionRequest>::create();
        auto response = QSharedPointer<Http::GalaxyVersionResponse>::create();

        request->SetUrl(urlParam.url);
        request->SetTimeout(urlParam.timeout);
        request->SetCurrentVersion(Core::GlobalData::Get()->GetConfig().version.code);
        request->SetPackageName(Core::GlobalData::Get()->GetAppName());
        request->SetAppId(Core::GlobalData::Get()->GetAppId());
        Http::Client::Get()->ProcessRequest(request, response);

        if(!response->IsSuccess())
        {
            message = response->GetErrorMessage();
            return false;
        }

        auto data = response->GetResult();
        if(!data.versionList.isEmpty())
        {
            QString version = data.versionList[0].versionName;
            if(version != Core::GlobalData::Get()->GetConfig().version.string)
            {
                found = true;
                info.version = version;
                info.url = data.versionList[0].versionPath;
                info.md5 = data.versionList[0].md5;
                info.isPatched = data.versionList[0].packageType == 2;
                info.isForced = data.versionList[0].forceUpdate == 1;
            }
        }

        return true;
    }

    PackageManager::PatchError PackageManager::ApplyPatch(const PatchInfo& info, const QString& patchFile, QString& message)
    {
        // 日志消息
        QString logmsg;

        message.clear();

        // 补丁包MD5校验
        if(!QFile::exists(patchFile))
        {
            message = QStringLiteral("ERROR: PATCH_ERR_FILE_NOT_EXIST");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_FILE_NOT_EXIST 补丁文件不存在");
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_FILE_NOT_EXIST;
        }

        // 解压缩补丁包
        QFileInfo fi(patchFile);
        QString patchDir = QDir::toNativeSeparators(fi.absolutePath() + QDir::separator() + fi.baseName() + QDir::separator());
        QDir dir;
        if(!dir.mkpath(patchDir))
        {
            message = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY 创建补丁解压目录失败");
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_CREATE_DIRECTORY;
        }

        if(!Base::ZipUtil::ExtractDir(patchFile, patchDir))
        {
            message = QStringLiteral("ERROR: PATCH_ERR_DECOMPRESSING");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_DECOMPRESSING 解压补丁包失败");
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_DECOMPRESSING;
        }

        // 解析补丁文件列表
        QString xmlFilePath = patchDir + "patch.xml";
        UpdateXml xmlFile(xmlFilePath);
        if(!xmlFile.LoadFile())
        {
            Base::PathUtil::DeleteDir(patchDir);
            message = QStringLiteral("ERROR: PATCH_ERR_PARSE_INFO");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_PARSE_INFO 解析补丁包文件列表失败，File：%1").arg(xmlFilePath);
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_PARSE_INFO;
        }

        // 提取补丁文件信息
        HeadInfo headInfo;
        xmlFile.GetXmlHead(headInfo);
        QMap<QString, UpdateFileInfo> patchFiles;
        xmlFile.GetFiles(patchFiles);
        if(headInfo.fileNum != patchFiles.size())
        {
            Base::PathUtil::DeleteDir(patchDir);
            message = QStringLiteral("ERROR: PATCH_ERR_PARSE_INFO");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_PARSE_INFO 文件数量不匹配，File：%1").arg(xmlFilePath);
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_PARSE_INFO;
        }

        // 准备待升级文件
        QString oldDir = PrepareFiles(info);
        if(oldDir.isEmpty())
        {
            message = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE 准备待升级文件失败，oldDir：%1，version：%2").arg(oldDir).arg(info.version);
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_COPY_FILE;
        }

        // 提取待升级文件信息
        QMap<QString, FileInfo> oldFiles;
        FindFiles(oldDir, oldDir, oldFiles);

        // 开始升级文件
        PatchError code = PatchFiles(oldDir, patchDir, oldFiles, patchFiles, message);

        // 更新成功
        if(code == PatchError::PATCH_ERR_SUCCESS)
        {
            // 删除补丁信息文件
            QString patchInfoFile = QFileInfo(patchFile).absoluteFilePath() + ".patchinfo";
            QFile::remove(patchInfoFile);

            // 删除补丁文件
            QFile::remove(patchFile);

            // 更新设置
            UpdateSettings(info);

            // 更新版本号
            auto& version = Core::GlobalData::Get()->GetConfig().version.string = info.version;
        }

        return code;
    }

    QString PackageManager::PrepareFiles(const PatchInfo& info) const
    {
        QString appPath = Base::PathUtil::GetApplicationDir();
        QDir dir(appPath);
        if(dir.cdUp())
        {
            QString fileDir = QDir::toNativeSeparators(dir.absolutePath() + QDir::separator() + info.version + QDir::separator());
            if(!Base::PathUtil::CreateDir(fileDir).isEmpty())
            {
                if(Base::PathUtil::CopyDir(appPath, fileDir))
                    return fileDir;
            }
        }

        return "";
    }

    void PackageManager::FindFiles(const QString& root, const QString& parent, QMap<QString, FileInfo>& infoList) const
    {
        QDir dir(parent);
        if(!dir.exists())
            return;

        QDir rootDir(root);
        FileInfo info;
        for(QFileInfo& fi : dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot))
        {
            QString s = fi.filePath();
            if(fi.isDir())
            {
                info.name = fi.fileName();
                info.md5 = "";
                info.type = 1;
                info.size = 0;
                info.path = QDir::toNativeSeparators(rootDir.relativeFilePath(fi.absoluteFilePath()) + QDir::separator());
                infoList[info.path + info.name] = info;
                FindFiles(root, fi.filePath(), infoList);
            }
            else
            {
                info.name = fi.fileName();
                info.md5 = "";
                info.type = 0;
                info.size = fi.size();

                if(rootDir == fi.absoluteDir())
                    info.path = "";
                else
                    info.path = QDir::toNativeSeparators(rootDir.relativeFilePath(fi.absolutePath()) + QDir::separator());

                infoList[info.path + info.name] = info;
            }
        }
    }

    PackageManager::PatchError PackageManager::PatchFiles(
        const QString& oldDir,
        const QString& patchDir,
        const QMap<QString, FileInfo>& oldFiles,
        const QMap<QString, UpdateFileInfo>& updateFiles, QString& message) const
    {
        // 补丁目录
        QString updateDir = patchDir + "updatefile/";

        // 回滚目录
        QString rollbackDir = patchDir + "rollback/";

        // 生成的新文件目录
        QString saveDir = patchDir + "patch/";

        // 日志消息
        QString logmsg;

        message.clear();

        QDir dir;
        if(!dir.mkpath(saveDir))
        {
            message = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY 创建patch目录失败");
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_CREATE_DIRECTORY;
        }

        if(!dir.mkpath(rollbackDir))
        {
            message = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY");
            logmsg = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY 创建rollback目录失败");
            LOG_ERROR(logmsg.toStdString());
            return PatchError::PATCH_ERR_CREATE_DIRECTORY;
        }

        // 生成文件
        CourgetteImpl courgette;
        PatchError code = PatchError::PATCH_ERR_SUCCESS;
        QString oldFile, newFile, patchFile, rollbackFile;
        for(auto it = updateFiles.begin(); it != updateFiles.end(); ++it)
        {
            // 有补丁
            if(it.value().bPatch)
            {
                // 验证补丁文件正确性
                patchFile = updateDir + it.value().patchFile.name;
                QFileInfo info(patchFile);

                // 大小不匹配
                if(info.size() != it.value().patchFile.size)
                {
                    Q_ASSERT(0);
                    code = PatchError::PATCH_ERR_CHECKSUM;
                    message = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM");
                    logmsg = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM 差量补丁文件大小不匹配，patchFile：%1").arg(patchFile);
                    LOG_ERROR(logmsg.toStdString());
                    break;
                }

                // MD5不匹配
                if(Base::MathUtil::GetFileMD5(patchFile) != it.value().patchFile.md5)
                {
                    Q_ASSERT(0);
                    code = PatchError::PATCH_ERR_CHECKSUM;
                    message = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM");
                    logmsg = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM 差量补丁文件md5不匹配，patchFile：%1").arg(patchFile);
                    LOG_ERROR(logmsg.toStdString());
                    break;
                }
            }

            newFile = saveDir + it.key();

            // 处理目录
            if(!it.value().path.isEmpty())
            {
                QDir dir;
                if(!dir.mkpath(saveDir + it.value().path))
                {
                    Q_ASSERT(0);
                    code = PatchError::PATCH_ERR_CREATE_DIRECTORY;
                    message = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY");
                    logmsg = QStringLiteral("ERROR: PATCH_ERR_CREATE_DIRECTORY 生成文件目录失败，dir：%1").arg(saveDir + it.value().path);
                    LOG_ERROR(logmsg.toStdString());
                    break;
                }
            }

            // 文件查找
            auto findIt = oldFiles.find(it.key());
            if(findIt != oldFiles.end())
            {
                // 文件已存在
                oldFile = oldDir + it.key();

                if(it.value().bPatch)
                {
                    // 差量文件处理
                    if(Base::MathUtil::GetFileMD5(oldFile) != it.value().oldMd5)
                    {
                        Q_ASSERT(0);
                        code = PatchError::PATCH_ERR_CHECKSUM;
                        message = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM");
                        logmsg = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM 原始文件md5与差量文件oldmd5不匹配，oldFile：%1").arg(oldFile);
                        LOG_ERROR(logmsg.toStdString());
                        break;
                    }

                    patchFile = updateDir + it.value().patchFile.name;
                    if(!courgette.MakeBSDFileToPatch(oldFile, patchFile, newFile))
                    {
                        Q_ASSERT(0);
                        code = PatchError::PATCH_ERR_APPLY_TO_FILE;
                        message = QStringLiteral("ERROR: PATCH_ERR_APPLY_TO_FILE");
                        logmsg = QStringLiteral("ERROR: PATCH_ERR_APPLY_TO_FILE 差量文件生成新文件失败，oldFile：%1，patchFile：%2，newFile：%3")
                            .arg(oldFile).arg(patchFile).arg(newFile);
                        LOG_ERROR(logmsg.toStdString());
                        break;
                    }

                    // 生成的文件不对
                    if(Base::MathUtil::GetFileMD5(newFile) != it.value().md5)
                    {
                        Q_ASSERT(0);
                        code = PatchError::PATCH_ERR_CHECKSUM;
                        message = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM");
                        logmsg = QStringLiteral("ERROR: PATCH_ERR_CHECKSUM 差量文件生成的新文件md5不匹配，newFile：%1").arg(newFile);
                        LOG_ERROR(logmsg.toStdString());
                        break;
                    }
                }
                else
                {
                    // 全量文件处理
                    if(it.value().type == 0)
                    {
                        patchFile = updateDir + it.key();
                        if(!Base::PathUtil::CopyFileToPath(patchFile, newFile, true))
                        {
                            Q_ASSERT(0);
                            code = PatchError::PATCH_ERR_COPY_FILE;
                            message = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE");
                            logmsg = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE 拷贝文件失败，from：%1，to：%2")
                                .arg(patchFile).arg(newFile);
                            LOG_ERROR(logmsg.toStdString());
                            break;
                        }
                    }
                }

                // 拷贝老文件到回滚目录
                if(it.value().type == 0)
                {
                    rollbackFile = rollbackDir + findIt.key();
                    if(!Base::PathUtil::CopyFileToPath(oldFile, rollbackFile, true))
                    {
                        Q_ASSERT(0);
                        code = PatchError::PATCH_ERR_COPY_FILE;
                        message = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE");
                        logmsg = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE 拷贝文件失败，[DIR]：%1 => [DIR]：%2")
                            .arg(oldFile).arg(rollbackFile);
                        LOG_ERROR(logmsg.toStdString());
                        break;
                    }
                }
            }
            else
            {
                // 全新的新文件
                if(it.value().type == 0)
                {
                    patchFile = updateDir + it.key();
                    if(!Base::PathUtil::CopyFileToPath(patchFile, newFile, true) != 0)
                    {
                        Q_ASSERT(0);
                        code = PatchError::PATCH_ERR_COPY_FILE;
                        message = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE");
                        logmsg = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE 拷贝文件失败，[FILE]：%1 => [FILE]：%2")
                            .arg(patchFile).arg(newFile);
                        LOG_ERROR(logmsg.toStdString());
                        break;
                    }
                }
            }
        }

        if(code == PatchError::PATCH_ERR_SUCCESS)
        {
            // 复制新文件
            if(!Base::PathUtil::CopyDir(saveDir, oldDir))
            {
                message = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE");
                logmsg = QStringLiteral("ERROR: PATCH_ERR_COPY_FILE 拷贝文件失败，[DIR]：%1 => [DIR]：%2")
                    .arg(saveDir, oldDir);
                LOG_ERROR(logmsg.toStdString());
                code = PatchError::PATCH_ERR_COPY_FILE;
            }
            else
            {
                LOG_INFO(QStringLiteral("升级完成").toStdString());
            }
        }
        else
        {
            // 回滚
            if(!Base::PathUtil::CopyDir(rollbackDir, oldDir))
            {
                LOG_ERROR(QStringLiteral("回滚失败，[DIR]：%1 => [DIR]：%2").arg(rollbackDir).arg(oldDir).toStdString());
            }
        }

        // 删除解压的目录
        LOG_INFO(QStringLiteral("删除解压目录...").toStdString());
        Base::PathUtil::DeleteDir(patchDir);

        // 分割线
        LOG_INFO(QStringLiteral("###################################################\n").toStdString());

        return code;
    }

    void PackageManager::UpdateSettings(const PatchInfo& info) const
    {
        QString appPath = Base::PathUtil::GetApplicationDir();
        QDir dir(appPath);
        if(!dir.cdUp())
            return;

        // 控制面板程序项
        QString rootDir = QDir::toNativeSeparators(dir.absolutePath() + QDir::separator());
        QString versionDir = QDir::toNativeSeparators(dir.absolutePath() + QDir::separator() + info.version + QDir::separator());
        QString key = QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\多点智慧收银系统\\");
        QSettings registry(key, QSettings::NativeFormat);
        registry.setValue("DisplayName", "DMALL OS POS SERVER");
        registry.setValue("DisplayVersion", info.version);
        registry.setValue("DisplayIcon", versionDir + "Pos.exe");
        registry.setValue("UninstallString", versionDir + "Uninst.exe");

        // 读Launcher.exe配置
        QStringList allVersions;
        QFile inFile(rootDir + "Manifest.json");
        if(inFile.open(QIODevice::ReadOnly))
        {
            QByteArray data = inFile.readAll();
            inFile.close();
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
            if(parseError.error == QJsonParseError::NoError)
            {
                QJsonObject root = doc.object();
                allVersions.prepend(info.version);
                for(const auto& value : root["version"].toArray())
                {
                    allVersions.append(value.toString());
                }

                // 只保留2个版本，最老的版本目录将被删除
                if(allVersions.size() > 2)
                {
                    QString oldVersion = allVersions.takeLast();

                    // 确保不是同名目录覆盖
                    if(oldVersion != info.version)
                        Base::PathUtil::DeleteDir(rootDir + oldVersion);
                }
            }
        }

        // 写Launcher.exe配置
        QFile outFile(rootDir + "Manifest.json");
        if(outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QJsonDocument doc;
            QJsonObject root;
            root["version"] = QJsonArray::fromStringList(allVersions);
            doc.setObject(root);
            outFile.write(doc.toJson(QJsonDocument::Compact));
            outFile.flush();
            outFile.close();
        }
    }

    void PackageManager::onCheckUpdate()
    {
        // 版本查询请求 
        bool found = false;
        QString message;
        PatchInfo info;
        if(!QueryVersion(found, info, message))
        {
            LOG_ERROR(message.toStdString());
            emit signalCheckUpdateError(message);
            return;
        }

        emit signalCheckUpdateSuccess(found, info);
    }

    void PackageManager::onDownloadPackage(const PatchInfo& info)
    {
        QString patchFile = Base::PathUtil::GetAppPatchDir() + Base::MathUtil::GetMD5(info.url) + ".zip";
        QString patchInfoFile = Base::PathUtil::GetAppPatchDir() + Base::MathUtil::GetMD5(info.url) + ".zip.patchinfo";

        // 检查包是否预下载
        if(QFile::exists(patchFile))
        {
            if(Base::MathUtil::GetFileMD5(patchFile) == info.md5)
            {
                LOG_INFO(QStringLiteral("补丁包已下载").toStdString());
                emit signalDownloadPackageSuccess(info, patchFile);
                RESET_FROZEN_FLAG();
                return;
            }
            else
            {
                // 包损坏
                QFile::remove(patchFile);
                QFile::remove(patchInfoFile);
            }
        }

        // 下载包
        auto request = QSharedPointer<Http::Request>::create();
        auto response = QSharedPointer<Http::Response>::create();

        request->SetUrl(info.url);
        request->SetTimeout(10 * 60 * 1000);
        request->SetDownloadFilePath(patchFile);
        request->SetCompleteCallback([this, info](QSharedPointer<Http::Request> req, QSharedPointer<Http::Response> rsp) {
                QMetaObject::invokeMethod(this, "OnDownloadResult", 
                Q_ARG(PatchInfo, info), Q_ARG(QSharedPointer<Http::Request>, req), Q_ARG(QSharedPointer<Http::Response>, rsp));
        });
        request->SetProgressCallback([this, info](QSharedPointer<Http::Request> request, QSharedPointer<Http::Response> response, 
            qint64 current, qint64 total, bool upload) {
                QMetaObject::invokeMethod(this, "OnDownloadProgress", 
                Q_ARG(PatchInfo, info), Q_ARG(qint64, current), Q_ARG(qint64, total));
        });

        Http::Client::Get()->Download(request, response, true);
    }

    void PackageManager::OnDownloadResult(const PatchInfo& info, QSharedPointer<Http::Request> request, QSharedPointer<Http::Response> response)
    {
        if(response->IsSuccess())
        {
            QString patchFile = Base::PathUtil::GetAppPatchDir() + Base::MathUtil::GetMD5(info.url) + ".zip";
            QString patchInfoFile = Base::PathUtil::GetAppPatchDir() + Base::MathUtil::GetMD5(info.url) + ".zip.patchinfo";
            // 生成.patchinfo
            QSettings iniFile(patchInfoFile, QSettings::IniFormat);
            iniFile.setValue("info/version", info.version);
            iniFile.setValue("info/url", info.url);
            iniFile.setValue("info/md5", info.md5);
            iniFile.setValue("info/isPatched", info.isPatched);
            iniFile.setValue("info/isForced", info.isForced);
            iniFile.sync();

            emit signalDownloadPackageSuccess(info, patchFile);
            RESET_FROZEN_FLAG();
        }
        else
        {
            emit signalDownloadPackageError(info, response->GetErrorMessage());
            RESET_FROZEN_FLAG();
        }
    }

    void PackageManager::OnDownloadProgress(const PatchInfo& info, qint64 bytesReceived, qint64 bytesTotal)
    {
        emit signalDownloadPackageProgress(info, bytesReceived, bytesTotal);
    }

    void PackageManager::onUpdatePackage(const PatchInfo& info, const QString& fileName)
    {
        // 校验md5
        QString message = "Verify file MD5";
        LOG_INFO(message.toStdString());
        emit signalUpdatePackageProgress(info, message);

        if(Base::MathUtil::GetFileMD5(fileName) != info.md5)
        {
            QFile::remove(fileName);
            message = "File MD5 verification failed";
            LOG_ERROR(message.toStdString());
            emit signalUpdatePackageError(info, PatchError::PATCH_ERR_CHECKSUM, message);
            RESET_FROZEN_FLAG();
            return;
        }

        // 补丁更新
        message = "Update file";
        LOG_INFO(message.toStdString());
        emit signalUpdatePackageProgress(info, message);

        PatchError code = ApplyPatch(info, fileName, message);
        if(code == PatchError::PATCH_ERR_SUCCESS)
            emit signalUpdatePackageSuccess(info);
        else
            emit signalUpdatePackageError(info, code, message);

        RESET_FROZEN_FLAG();
    }

    void PackageManager::onReportVersion()
    {
        const auto& version = Core::GlobalData::Get()->GetConfig().version;
        auto urlParam = Base::GetService<UrlManager>()->GetUrl(UrlManager::GALAXY_REPORT_VERSION);
        auto request = QSharedPointer<Http::GalaxyReportVersionRequest>::create();
        auto response = QSharedPointer<Http::PosResponse>::create();

        request->SetUrl(urlParam.url);
        request->SetTimeout(urlParam.timeout);
        request->SetVersionName(version.string);
        request->SetVersionCode(version.code);
        request->SetPackageName(Core::GlobalData::Get()->GetAppName());
        request->SetAppId(Core::GlobalData::Get()->GetAppId());

        Http::Client::Get()->ProcessRequest(request, response);

        if(response->IsSuccess())
        {
            LOG_INFO(QStringLiteral("POS Server上报版本成功，版本号：%1").arg(version.string).toStdString());
        }
        else
        {
            LOG_ERROR(response->GetErrorMessage().toStdString());
        }
    }

    void PackageManager::onTimeout()
    {
        LOG_INFO("timer check update...");
        // 暂停定时器
        m_timer->stop();

        do
        {
            bool found = false;
            QString message;
            PatchInfo info;

            // 检查更新
            if(!QueryVersion(found, info, message))
            {
                LOG_ERROR(message.toStdString());
                break;
            }

            if(!found)
            {
                LOG_INFO(QStringLiteral("未查询到可用的补丁包").toStdString());
                break;
            }

            // 下载补丁包
            QString patchFile = Base::PathUtil::GetAppPatchDir() + Base::MathUtil::GetMD5(info.url) + ".zip";
            QString patchInfoFile = Base::PathUtil::GetAppPatchDir() + Base::MathUtil::GetMD5(info.url) + ".zip.patchinfo";

            // 检查包是否预下载
            if(QFile::exists(patchFile))
            {
                if(Base::MathUtil::GetFileMD5(patchFile) == info.md5)
                {
                    LOG_INFO(QStringLiteral("包已存在，文件：%1").arg(patchFile).toStdString());
                    break;
                }
                else
                {
                    // 包损坏
                    LOG_ERROR(QStringLiteral("包已损坏，准备删除，文件：%1").arg(patchFile).toStdString());
                    QFile::remove(patchFile);
                    QFile::remove(patchInfoFile);
                }
            }

            // 下载包
            auto request = QSharedPointer<Http::Request>::create();
            auto response = QSharedPointer<Http::Response>::create();

            request->SetUrl(info.url);
            request->SetTimeout(10 * 60 * 1000);
            request->SetDownloadFilePath(patchFile);
            Http::Client::Get()->Download(request, response);

            if(response->IsSuccess())
            {
                LOG_INFO(QStringLiteral("静默下载补丁包完成，文件：%1").arg(patchFile).toStdString());

                // 生成.patchinfo
                QSettings iniFile(patchInfoFile, QSettings::IniFormat);
                iniFile.setValue("info/version", info.version);
                iniFile.setValue("info/url", info.url);
                iniFile.setValue("info/md5", info.md5);
                iniFile.setValue("info/isPatched", info.isPatched);
                iniFile.setValue("info/isForced", info.isForced);
                iniFile.sync();
            }
            else
            {
                LOG_ERROR(QStringLiteral("下载补丁包失败，错误：%1").arg(response->GetErrorMessage()).toStdString());
            }
        } while(false);

        // 重新启动定时器
        m_timer->start();
    }
}