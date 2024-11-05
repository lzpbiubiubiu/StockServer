#include <QDir>

#include "ModuleBase/Common/System.h"
#include "ModuleBase/Http/Client/Client.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Util/JsonUtil.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleBase/Util/ZipUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleProtocol/Client/ApiIotReportEquipmentCommandResultRequest.h"
#include "ModuleProtocol/Client/ApiIotReportEquipmentCommandResultResponse.h"
#include "ModuleProtocol/Client/HeartRequest.h"
#include "ModuleProtocol/Client/HeartResponse.h"
#include "RemoteManager.h"

/** 上传日志 */
static const char* KEY_UPLOAD_LOG = "thing.service.SYS_UploadLog";

/** 上传dump  */
static const char* KEY_UPLOAD_DUMP = "thing.service.upload_dmp";

/** 上传pos 数据库  */
static const char* KEY_UPLOAD_POS_DB = "thing.service.upload_pos_db";

/** 删除指定文件 */
static const char* KEY_DELETE_FILE = "thing.service.delete_file";

/** 上传指定文件 */
static const char* KEY_UPLOAD_FILE = "thing.service.upload_appoint_file";

/** 上传的token key */
static const char* UPLOAD_TOKEN_KEY_TABLE[] =
{
    // DEV
    "_test_tenant_token_",

    // UAT
    "_uat_tenant_token_",

    // PRO
    "_tenant_token_"
};

/** 上传的token */
static const char* UPLOAD_TOKEN = "68d54fa0-2cd0-dd2b-616e-65c62472b9a0";

namespace Extra
{
    RemoteManager::RemoteManager(QObject* parent/* = nullptr*/)
        : Base::AsyncService(parent)
    {
        qRegisterMetaType<Command>("Command");
    }

    RemoteManager::~RemoteManager()
    {}

    void RemoteManager::UploadDumpFile()
    {
        QMetaObject::invokeMethod(this, "onUploadDumpFile");
    }

    void RemoteManager::StartHeartbeat()
    {
        QMetaObject::invokeMethod(this, "onStartHeartbeat");
    }

    bool RemoteManager::OnStart()
    {
        m_timer = new QTimer(this);
        m_um = Base::GetService<UrlManager>();
        m_cm = Base::GetService<ConnectivityManager>();

        connect(m_timer, &QTimer::timeout, this, &RemoteManager::onTimeout);
        connect(m_cm, &ConnectivityManager::signalOnlineServiceConnected, this, &RemoteManager::onOnlineServiceConnected);

        return true;
    }

    void RemoteManager::OnStop()
    {
        m_timer->disconnect();
        m_timer->stop();
    }

    void RemoteManager::OnAfterStart()
    {}

    void RemoteManager::OnBeforeStop()
    {}

    void RemoteManager::onUploadDumpFile()
    {
        Command command;
        command.uuid = Base::MathUtil::GetUUID();
        command.key = KEY_UPLOAD_DUMP;

        OnCommandUploadDump(command);
    }

    void RemoteManager::onStartHeartbeat()
    {
        m_timer->start(60 * 1000);
        onTimeout();
    }

    void RemoteManager::onTimeout()
    {
        auto urlParam = m_um->GetUrl(UrlManager::HEART);
        auto request = QSharedPointer<Http::HeartRequest>::create();
        auto response = QSharedPointer<Http::HeartResponse>::create();

        request->SetUrl(urlParam.url);
        request->SetTimeout(urlParam.timeout);
        request->SetTimestamp(Base::System::GetCurrentTimeMillis());
        request->SetProductKey(Core::GlobalData::Get()->GetProductKey());

        if(m_sendDeviceOnline)
            request->SetOperateType(Http::HeartRequest::ONLION);

        Http::Client::Get()->ProcessRequest(request, response);

        // 心跳失败不做处理
        if(!response->IsSuccess())
            return;

        m_sendDeviceOnline = false;

        const auto& result = response->GetResult();
        QList<Command> commands;

        for(auto commandStr : result.commandList)
            commands << ParseCommand(commandStr);

        if(!commands.isEmpty())
        {
            m_commands << commands;
            QTimer::singleShot(0, this, &RemoteManager::ExecCommand);
        }
    }

    void RemoteManager::onOnlineServiceConnected()
    {
        m_sendDeviceOnline = true;
    }

    RemoteManager::Command RemoteManager::ParseCommand(const QString& commandStr) const
    {
        Command command;

        auto commandObj = Base::JsonUtil::ToJsonObject(commandStr);
        command.uuid = Base::MathUtil::GetUUID();
        command.id = commandObj["id"].toString();
        command.key = commandObj["method"].toString();
        command.params = commandObj["params"].toObject();
        command.needReply = true;

        return command;
    }

    void RemoteManager::ExecCommand()
    {
        while(!m_commands.isEmpty())
        {
            auto command = m_commands.takeFirst();
            LOG_INFO(QString("exec command, id: %1, key: %2, parmas: %3")
                .arg(command.id).arg(command.key).arg(Base::JsonUtil::ToString(command.params)).toStdString());

            if(command.key == KEY_UPLOAD_LOG)
            {
                // 上传日志
                OnCommandUploadLog(command);
            }
            else if(command.key == KEY_UPLOAD_DUMP)
            {
                // 上传dump
                OnCommandUploadDump(command);
            }
            else if(command.key == KEY_UPLOAD_POS_DB)
            {
                // 上传Db
                OnCommandUploadDb(command);
            }
            else if(command.key == KEY_UPLOAD_FILE)
            {
                // 上传指定文件
                OnCommandUploadFile(command);
            }
            else if(command.key == KEY_DELETE_FILE)
            {
                // 删除指定文件
                OnCommandDeleteFile(command);
            }
            else
            {
                LOG_WARN(QString("unknow command, id: %1, key: %2").arg(command.id).arg(command.key).toStdString());
            }
        }
    }

    void RemoteManager::OnCommandUploadLog(const Command& command)
    {
        if(!command.params.contains("querydate"))
        {
            LOG_ERROR(QString("unknow params, key:%1, params:%2")
                .arg(command.key).arg(Base::JsonUtil::ToString(command.params)).toStdString());
            return;
        }

        QString queryDate = command.params["querydate"].toString();
        QString logBackDir = Base::PathUtil::GetTempPosServerDir().append(command.uuid).append("/");

        // 删除以前备份目录
        QDir backDir(logBackDir);
        backDir.removeRecursively();

        // 创建备份目录
        backDir.mkdir(logBackDir);

        // 备份需要打包日志
        QDir logDir(Base::PathUtil::GetLogDir());
        logDir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList fileList = logDir.entryInfoList();
        for(auto& item : fileList)
        {
            QDateTime modifyTime = item.lastModified();
            if(queryDate == modifyTime.toString("yyyy-MM-dd"))
            {
                QFile::copy(item.absoluteFilePath(), logBackDir + item.fileName());
            }
        }

        auto device = Core::GlobalData::Get()->GetConfig().device;
        // 打包日志文件名称
        QString logZipPath = logBackDir
            + QString("%1_%2_%3_%4_%5_%6_%7")
            .arg(device.venderId) //商家id
            .arg(device.venderName) //商家名称
            .arg(device.storeId) //门店id
            .arg(device.storeName) //门店名称
            .arg(device.deviceId) //设备号
            .arg(Base::System::GetCurrentDateTime().toString("yyyyMMddHHmmss"))
            .arg("logs.zip");

        // 压缩备份文件
        backDir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList backFileList = backDir.entryInfoList();

        QStringList backPathList;
        for(auto item : backFileList)
            backPathList.append(item.absoluteFilePath());

        if(!Base::ZipUtil::CompressFiles(logZipPath, backPathList))
        {
            LOG_ERROR(QString("upload log zip fille error, %1").arg(logZipPath).toStdString());
            return;
        }

        auto uploadDir = QString("posServer/%1/%2/logs").arg(device.venderId).arg(device.storeId);
        LOG_INFO(QString("update log path=%1").arg(logZipPath).toStdString());
        UploadFile(command, logZipPath, uploadDir);
    }

    void RemoteManager::OnCommandUploadDump(const Command& command)
    {
        QString dumpBackDir = Base::PathUtil::GetTempPosServerDir().append(command.uuid).append("/");

        // 删除以前备份目录
        QDir backDir(dumpBackDir);
        backDir.removeRecursively();

        // 创建备份目录
        backDir.mkdir(dumpBackDir);

        // 备份需要打包Dump
        QDir dumpDir(Base::PathUtil::GetDumpDir());
        dumpDir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList fileList = dumpDir.entryInfoList();
        for(auto& item : fileList)
        {
            if(item.fileName().contains(".dmp"))
            {
                QFile::copy(item.absoluteFilePath(), dumpBackDir + item.fileName());
            }
        }

        auto device = Core::GlobalData::Get()->GetConfig().device;
        // Dump文件名称
        QString dumpZipPath = dumpBackDir
            + QString("%1_%2_%3_%4_%5_%6_%7")
            .arg(device.venderId) //商家id
            .arg(device.venderName) //商家名称
            .arg(device.storeId) //门店id
            .arg(device.storeName) //门店名称
            .arg(device.deviceId) //设备号
            .arg(Base::System::GetCurrentDateTime().toString("yyyyMMddHHmmss"))
            .arg("dumps.zip");

        // 压缩备份文件
        backDir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList backFileList = backDir.entryInfoList();
        if(backFileList.isEmpty())
        {
            backDir.removeRecursively();
            return;
        }

        QStringList backPathList;
        for(auto item : backFileList)
        {
            backPathList.append(item.absoluteFilePath());
        }

        if(!Base::ZipUtil::CompressFiles(dumpZipPath, backPathList))
        {
            LOG_ERROR(QString("upload dump zip file error, %1").arg(dumpZipPath).toStdString());
            return;
        }

        auto uploadDir = QString("posServer/%1/%2/dumps").arg(device.venderId).arg(device.storeId);
        LOG_INFO(QString("update dump file  path=%1").arg(dumpZipPath).toStdString());
        UploadFile(command, dumpZipPath, uploadDir);
    }

    void RemoteManager::OnCommandUploadDb(const Command& command)
    {
        if(!command.params.contains("date"))
        {
            LOG_ERROR(QString("unknow params, key:%1, params:%2").arg(command.key).arg(Base::JsonUtil::ToString(command.params)).toStdString());
            return;
        }

        QString date = command.params["date"].toString();
        QString dbBackDir = Base::PathUtil::GetTempPosServerDir().append(command.uuid).append("/");

        // 删除以前备份目录
        QDir backDir(dbBackDir);
        backDir.removeRecursively();

        // 创建备份目录
        backDir.mkdir(dbBackDir);

        // 备份需要打包数据库
        QDir dbDir(Base::PathUtil::GetDatabaseDir());
        dbDir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList fileList = dbDir.entryInfoList();
        for(auto& item : fileList)
        {
            QString dateDb = QString("%1.db").arg(date);
            if(date.isEmpty() || item.fileName().contains(dateDb))
                QFile::copy(item.absoluteFilePath(), dbBackDir + item.fileName());
        }

        auto device = Core::GlobalData::Get()->GetConfig().device;
        // 打包日志文件名称
        QString dbZipPath = dbBackDir
            + QString("%1_%2_%3_%4_%5_%6_%7")
            .arg(device.venderId) //商家id
            .arg(device.venderName) //商家名称
            .arg(device.storeId) //门店id
            .arg(device.storeName) //门店名称
            .arg(device.deviceId) //设备号
            .arg(Base::System::GetCurrentDateTime().toString("yyyyMMddHHmmss"))
            .arg("dbs.zip");

        // 压缩备份文件
        backDir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList backFileList = backDir.entryInfoList();

        QStringList backPathList;
        for(auto item : backFileList)
        {
            QString dateDb = QString("%1.db").arg(date);
            if(date.isEmpty() || item.fileName().contains(dateDb))
            {
                backPathList.append(item.absoluteFilePath());
            }
        }

        if(!Base::ZipUtil::CompressFiles(dbZipPath, backPathList))
        {
            LOG_ERROR(QString("upload db zip fille error, %1").arg(dbZipPath).toStdString());
            return;
        }

        auto uploadDir = QString("posServer/%1/%2/dbs").arg(device.venderId).arg(device.storeId);
        LOG_INFO(QString("update db file  path=%1").arg(dbZipPath).toStdString());
        UploadFile(command, dbZipPath, uploadDir);
    }

    void RemoteManager::OnCommandUploadFile(const Command& command)
    {
        if(!command.params.contains("filePath"))
        {
            LOG_ERROR(QString("unknow params, key:%1, params:%2").arg(command.key).arg(Base::JsonUtil::ToString(command.params)).toStdString());
            return;
        }
        QString filePath = command.params["filePath"].toString();

        if(filePath.isEmpty())
        {
            return;
        }

        int nPos = filePath.lastIndexOf("/");
        QString qsPath = filePath.left(nPos);
        QDir dir(qsPath);
        if(!dir.exists()) // 判断是否有盘符 若没有盘符 选择当前目录
        {
            filePath = Base::PathUtil::GetApplicationDir() + filePath;
        }

        if(!QFile::exists(filePath))
            return;

        QString fileBackDir = Base::PathUtil::GetTempPosServerDir().append(command.uuid).append("/");
        // 删除以前备份目录
        QDir backDir(fileBackDir);
        backDir.removeRecursively();

        // 创建备份目录
        backDir.mkdir(fileBackDir);

        QFileInfo fileInfo(filePath);
        QFile::copy(filePath, fileBackDir + fileInfo.fileName());

        auto device = Core::GlobalData::Get()->GetConfig().device;
        // 打包日志文件名称
        QString fileZipPath = fileBackDir
            + QString("%1_%2_%3_%4_%5_%6_%7")
            .arg(device.venderId) //商家id
            .arg(device.venderName) //商家名称
            .arg(device.storeId) //门店id
            .arg(device.storeName) //门店名称
            .arg(device.deviceId) //设备号
            .arg(Base::System::GetCurrentDateTime().toString("yyyyMMddHHmmss"))
            .arg("file.zip");

        QStringList list;
        list << fileBackDir + fileInfo.fileName();

        if(!Base::ZipUtil::CompressFiles(fileZipPath, list))
        {
            LOG_ERROR(QStringLiteral("上传指定文件 zip fille error, %1").arg(fileZipPath).toStdString());
            return;
        }

        auto uploadDir = QString("posServer/%1/%2/appointFile").arg(device.venderId).arg(device.storeId);
        LOG_INFO(QStringLiteral("上传指定文件 path= %1").arg(fileZipPath).toStdString());
        UploadFile(command, fileZipPath, uploadDir);
    }

    void RemoteManager::OnCommandDeleteFile(const Command& command)
    {
        if(!command.params.contains("fileName"))
        {
            LOG_ERROR(QString("unknow params, key:%1, params:%2").arg(command.key).arg(Base::JsonUtil::ToString(command.params)).toStdString());
            return;
        }

        QString fileName = command.params["fileName"].toString();
        QString path = Base::PathUtil::GetApplicationDir() + fileName;
        bool result = QFile::remove(path);
        LOG_INFO(QString("delete file  path=%1 success=%2").arg(path).arg(result).toStdString());

        QJsonObject payloadObj, payloadDataObj;
        payloadObj["id"] = command.id;
        payloadObj["code"] = "0000";
        payloadObj["message"] = "success";
        payloadObj["method"] = QString("%1.reply").arg(command.key);

        payloadObj["data"] = payloadDataObj;

        // 回执指令
        ReplyCommand(payloadObj);
    }

    void RemoteManager::UploadFile(const Command& command, const QString& path, const QString& uploadDir)
    {
        auto request = QSharedPointer<Http::Request>::create();
        auto response = QSharedPointer<Http::Response>::create();
        auto urlParam = m_um->GetUrl(UrlManager::FILE_UPLOAD);

        request->SetUrl(urlParam.url);
        request->SetTimeout(urlParam.timeout);
        request->SetUploadDir(uploadDir);
        request->SetUploadFilePath(path);
        request->SetHeader(QStringLiteral("Cookie"), QString("%1=%2")
            .arg(UPLOAD_TOKEN_KEY_TABLE[Core::GlobalData::Get()->GetConfig().app.env]).arg(UPLOAD_TOKEN));
        request->SetHeader(QStringLiteral("token"), UPLOAD_TOKEN);
        request->SetHeader("loginType", "4");
        request->SetHeader("dir", uploadDir.toUtf8().toPercentEncoding());
        request->SetCompleteCallback([this, command](QSharedPointer<Http::Request> req, QSharedPointer<Http::Response> rsp) {
            QMetaObject::invokeMethod(this, "OnUploadResult",
            Q_ARG(Command, command), Q_ARG(QSharedPointer<Http::Request>, req), Q_ARG(QSharedPointer<Http::Response>, rsp));
        });

        Http::Client::Get()->Upload(request, response, true);
    }

    void RemoteManager::OnUploadResult(const Command& command, QSharedPointer<Http::Request> request, QSharedPointer<Http::Response> response)
    {
        // 删除备份文件夹
        QString logBackDir = Base::PathUtil::GetTempPosServerDir().append(command.uuid).append("/");
        QDir backDir(logBackDir);
        backDir.removeRecursively();

        QJsonObject rspObj = Base::JsonUtil::ToJsonObject(response->GetData());
        QJsonObject payloadObj, payloadDataObj;
        QString code = rspObj["code"].toString();
        QString message = rspObj["message"].toString();

        // 解析上次结果信息
        if(code == "0000")
        {
            QString downloadUrl;
            QJsonObject dataObj = rspObj["data"].toObject();
            QJsonArray fileItems = dataObj["items"].toArray();

            if(!fileItems.isEmpty())
                downloadUrl = fileItems[0].toObject()["resourceUrl"].toString();

            payloadObj["id"] = command.id;
            payloadObj["code"] = "0000";
            payloadObj["message"] = "success";
            payloadObj["method"] = QString("%1.reply").arg(command.key);

            payloadDataObj["url"] = downloadUrl;
            payloadObj["data"] = payloadDataObj;
        }
        else
        {
            payloadObj["id"] = command.id;
            payloadObj["code"] = code;
            payloadObj["message"] = message;
            payloadObj["method"] = QString("%1.reply").arg(command.key);

            payloadObj["data"] = payloadDataObj;
        }
    
        // 回执指令
        if(command.needReply)
            ReplyCommand(payloadObj);
    }

    void RemoteManager::ReplyCommand(const QJsonObject& payload)
    {
        auto response = QSharedPointer<Http::ApiIotReportEquipmentCommandResultResponse>::create();
        auto request = QSharedPointer<Http::ApiIotReportEquipmentCommandResultRequest>::create();
        auto urlParam = m_um->GetUrl(UrlManager::API_IOT_REPORT_EQUIPMENT_COMMAND_RESULT);

        request->SetExternalCode(Core::GlobalData::Get()->GetConfig().device.deviceCode);
        request->SetProductKey(Core::GlobalData::Get()->GetProductKey());
        request->SetPayload(Base::JsonUtil::ToString(payload));
        request->SetUrl(urlParam.url);
        request->SetTimeout(urlParam.timeout);
        
        //不关心指令回执结果
        Http::Client::Get()->ProcessRequest(request, response, true);
    }
}// namespace Extra