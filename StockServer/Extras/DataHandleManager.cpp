#include <QFile>
#include <QTextCodec>
#include <QTimer>
#include <QJsonDocument>
#include "ModuleBase/Util/ZipUtil.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleData/Core/Config.h"
#include "ModuleBase/Log/Log.h"
#include "ModuleData/Core/DatabaseManager.h"
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleProtocol/Client/OfflineCheckAccountsRequest.h"
#include "ModuleProtocol/Client/PosResponse.h"
#include "ConfigManager.h"
#include "ModuleBase/Http/Client/Client.h"
#include "DataHandleManager.h"
#include "UrlManager.h"
#include "External/7zip/src/7ZipTool.h"

Extras::DataHandleManager::DataHandleManager(QObject* parent/* = nullptr*/)
    : AsyncService(parent)
{
}

Extras::DataHandleManager::~DataHandleManager()
{
}

void Extras::DataHandleManager::Init()
{
}

bool Extras::DataHandleManager::OnStart()
{
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &DataHandleManager::CheckTimeout);
    m_checkTimer->start(1000);
    return true;
}

void Extras::DataHandleManager::OnStop()
{
    m_checkTimer->disconnect();
    m_checkTimer->stop();
}

void Extras::DataHandleManager::ExportTradeData(const QString& dateTime, const QString& path)
{
    CHECK_AND_SET_FROZEN_FLAG();
    //QMetaObject::invokeMethod(this, "onExportTradeData", Q_ARG(QString, dateTime), Q_ARG(QString, path));
    QMetaObject::invokeMethod(this, "onExportTradeDataNew", Q_ARG(QString, dateTime), Q_ARG(QString, path));
}

void Extras::DataHandleManager::ImportTradeData(const QString& path)
{
    CHECK_AND_SET_FROZEN_FLAG();
    QMetaObject::invokeMethod(this, "onImportTradeData", Q_ARG(QString, path));
}

void Extras::DataHandleManager::onExportTradeData(const QString& dateTime, const QString& path)
{
    QDate queryDate = QDate::fromString(dateTime, "yyyy-MM-dd");
    QString fileName = Base::PathUtil::GetDatabaseDir() + RECORC_DATABASE_NAME + queryDate.toString("yyyy-MM-dd") + ".db";
    if(!QFile::exists(fileName))
    {
        RESET_FROZEN_FLAG();
        emit signalExportOfflineDataSuccess(false);
        return;
    }

    // todo 获取数据库数据
    auto dbm = Base::GetService<Extra::DatabaseManager>();
    auto db = dbm->GetRecordDatabase(queryDate);
    m_pushDml.SetDatabase(db);
    Core::OfflineDataPtrList offlineDataList;
    QString sqlError;
    bool queryState = true;
    if(!m_pushDml.GetAllJobGroups(offlineDataList, sqlError))
    {
        Q_ASSERT(0);
        queryState = false;
        LOG_ERROR(QStringLiteral("sql error: {%1}").arg(sqlError).toStdString());
    }

    m_recordDml.SetDatabase(db);
    Core::RecordDataPtrList recordDataList;
    if(!m_recordDml.GetAllRecords(recordDataList, sqlError))
    {
        Q_ASSERT(0);
        queryState = false;
        LOG_ERROR(QStringLiteral("sql error: {%1}").arg(sqlError).toStdString());
    }

    if(!queryState)
    {
        RESET_FROZEN_FLAG();
        LOG_ERROR(QStringLiteral("Record Data and offline Data is empty!").toStdString());
        emit signalExportOfflineDataSuccess(false);
        return;
    }

    // 组装数据
    QList<QStringList> dataList = JoinData(offlineDataList, recordDataList, queryDate.toString("yyyy-MM-dd"),true);
    // 写入文件
    bool writeState = WriteCsv(path, dataList);
    if(!writeState)
    { 
        LOG_ERROR(QStringLiteral("write file fail!").toStdString());
        emit signalExportOfflineDataSuccess(false);
    }
    else
    {
        emit signalExportOfflineDataSuccess(true);
    }
    RESET_FROZEN_FLAG();
}

void Extras::DataHandleManager::onExportTradeDataNew(const QString& dateTime, const QString& path)
{
    LOG_INFO(QStringLiteral("========>Start Export Data.... ").toStdString());
    QDate queryDate = QDate::fromString(dateTime, "yyyy-MM-dd");
    QString fileName = Base::PathUtil::GetDatabaseDir() + RECORC_DATABASE_NAME + queryDate.toString("yyyy-MM-dd") + ".db";
    if(!QFile::exists(fileName))
    {
        RESET_FROZEN_FLAG();
        emit signalExportOfflineDataSuccess(false, QString("No transaction data for the date"));
        return;
    }
    QList<QString> fileLists = WriteData(path, queryDate);
    // 压缩文件
    bool queryState = false;
    if(fileLists.isEmpty())
    {
        emit signalExportOfflineDataSuccess(false,QString("No transaction data for the date"));
        LOG_ERROR(QStringLiteral("Record Data or offline Data is empty!").toStdString());
        return;
    }
    else
    {
        queryState = CompressFilesToDirectory(fileLists,path);
    }

    // 删除文件
    for(auto& delPath : fileLists)
    {
        QFile::remove(delPath);
    }
    if(!queryState)
    {
        LOG_ERROR(QStringLiteral("Record Data or offline Data is error!").toStdString());
        emit signalExportOfflineDataSuccess(false);
    }
    else
    {
        emit signalExportOfflineDataSuccess(true);
    }
    RESET_FROZEN_FLAG();
    LOG_INFO(QStringLiteral("========>End Export Data.... ").toStdString());
}

void Extras::DataHandleManager::onImportTradeData(const QString& path)
{
    QString folderPath = Base::PathUtil::GetOfflinePacketDir();
    bool state = CopyFileToDirectory(path, folderPath);
    emit signalImportOfflineDataSuccess(state);
    RESET_FROZEN_FLAG();
}

void Extras::DataHandleManager::CheckTimeout()
{
    if(IsTimePoint())
    {
        QJsonArray jsonArray;
        QDate queryDate = QDate::currentDate();

        // 获取前1天的数据
        jsonArray.append(GetCheckAccountsData(queryDate.addDays(-1)));

        // 获取前2天的数据
        jsonArray.append(GetCheckAccountsData(queryDate.addDays(-2)));

        // 获取前3天的数据
        jsonArray.append(GetCheckAccountsData(queryDate.addDays(-3)));

        // 数据请求
        if(!jsonArray.isEmpty())
            OfflineCheckAccounts(jsonArray);
    }
}

void Extras::DataHandleManager::OfflineCheckAccounts(const QJsonArray& data)
{
    auto urlParam = Base::GetService<Extra::UrlManager>()->GetUrl(Extra::UrlManager::OFFLINE_CHECK_ACCOUNTS);
    auto request = QSharedPointer<Http::OfflineCheckAccountsRequest>::create();
    request->SetOfflineCheckInfo(data);
    auto response = QSharedPointer<Http::PosResponse>::create();

    request->SetUrl(urlParam.url);
    request->SetTimeout(urlParam.timeout);
    Http::Client::Get()->ProcessRequest(request, response);
    if(!response->IsSuccess())
    {
        LOG_INFO(QStringLiteral("自动对账失败").toStdString());
    }
    else
    {
        LOG_INFO(QStringLiteral("自动对账成功").toStdString());
    }
}

bool Extras::DataHandleManager::ReadCsvData(QString filePath, QList<QStringList>& data)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");   // 使用UTF-8编码
    QByteArray content = file.readAll();
    QString text = codec->toUnicode(content);
    QStringList lines = text.split('\n');

    for(int i = 0; i < lines.size(); i++)
    {
        if(!lines.at(i).isEmpty())
        {
            QStringList row = lines.at(i).split(',');
            data.append(row);
        }
    }

    file.close();
    return true;
}

bool Extras::DataHandleManager::WriteCsv(QString filePath, QList<QStringList>& data)
{
    QString fileName = filePath;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        LOG_INFO(QStringLiteral("文件打开失败：%1").arg(fileName).toStdString());
        return false;
    }

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");   // 使用UTF-8编码

    for(int i = 0; i < data.size(); i++)
    {
        QStringList row = data.at(i);
        QString line = row.join(",");
        line += "\n";
        QByteArray encodedLine = codec->fromUnicode(line);
        file.write(encodedLine);
    }

    file.close();
    return true;
}

QList<QStringList> Extras::DataHandleManager::JoinData(Core::OfflineDataPtrList& offlineDataList, Core::RecordDataPtrList& recordDataList, const QString& dateTime, bool header)
{
    QList<QStringList> exportLists;
    // 默认表头
    QStringList headerData;
    if(header)
    {
        headerData << "DataType" << "DataContext" << "CheckDate";
        exportLists.append(headerData);
    }

    for(auto& offline: offlineDataList)
    {
        QStringList exportData;
        exportData << QString::number(Extras::DataHandleManager::OFFLINE_TASK);
        QByteArray byteArray = offline->pushData.toUtf8();
        exportData << byteArray.toBase64();
        exportData << dateTime.toUtf8().toBase64();
        exportLists.append(exportData);
    }

    for(auto& record : recordDataList)
    {
        QStringList exportData;
        exportData << QString::number(Extras::DataHandleManager::RECORD_TASK);
        QByteArray byteArray = record->pushData.toUtf8();
        exportData << byteArray.toBase64();
        exportData << dateTime.toUtf8().toBase64();
        exportLists.append(exportData);
    }
    return exportLists;
}

bool Extras::DataHandleManager::IsTimePoint()
{
    // 获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();

    // 检查时间是否等于指定的时间点
    QString time = currentTime.time().toString("hh:mm:ss");
    if(time == "00:10:00")
        return true;
    if(time == "01:10:00")
        return true;
    if(time == "02:10:00")
        return true;
    return false;
}

QJsonObject Extras::DataHandleManager::GetCheckAccountsData(const QDate& date)
{
    auto dbm = Base::GetService<Extra::DatabaseManager>();
    auto db = dbm->GetRecordDatabase(date);
    m_pushDml.SetDatabase(db);
    QString sqlError;

    qint64 saleNum = m_pushDml.GetJobGroupsQuantity(
        QVariantList() << QVariant::fromValue(1) << QVariant::fromValue(3), sqlError);
    qint64 saleAmount = m_pushDml.GetJobGroupsAmount(
        QVariantList() << QVariant::fromValue(1) << QVariant::fromValue(3), sqlError);
    qint64 returnNum = m_pushDml.GetJobGroupsQuantity(QVariantList() << QVariant::fromValue(2), sqlError);
    qint64 returnAmount = m_pushDml.GetJobGroupsAmount(QVariantList() << QVariant::fromValue(2), sqlError);

    QJsonObject jsonObject;
    jsonObject["checkAccountsDate"] = date.toString("yyyy-MM-dd");
    jsonObject["clientSaleTotalAmount"] = saleAmount;
    jsonObject["clientSaleTotalCount"] = saleNum;
    jsonObject["clientAfterSaleTotalAmount"] = returnAmount;
    jsonObject["clientAfterSaleTotalCount"] = returnNum;
    return jsonObject;
}

bool Extras::DataHandleManager::CopyFileToDirectory(const QString& sourceFile, const QString& destDirectory)
{
    // 检查源文件是否存在
    QFile sourceFileObj(sourceFile);
    if(!sourceFileObj.exists())
    {
        LOG_INFO(QStringLiteral("Source file does not exist:%1").arg(sourceFile).toStdString());
        return false;
    }

    // 删除以前的目标目录
    QDir destDir(destDirectory);
    if(!destDir.removeRecursively())
    {
        LOG_INFO(QStringLiteral("Failed to clean dest dir:%1").arg(destDirectory).toStdString());
        return false;
    }

    // 创建新的目标目录
    if(!destDir.mkdir(destDirectory))
    {
        LOG_INFO(QStringLiteral("Failed to create dest dir:%1").arg(destDirectory).toStdString());
        return false;
    }

    // 解压文件到当前目录
    bool success = Base::ZipUtil::ExtractDir(sourceFile, destDirectory);
    if(success)
    {
        LOG_INFO(QStringLiteral("unzip file successfully from:%1 to %2").arg(sourceFile).arg(destDirectory).toStdString());

        QDir dir(destDirectory);
        if(!dir.exists() || dir.isEmpty())
        {
            LOG_INFO(QStringLiteral("unzip successfully but dir is not exist or empty").toStdString());
            success = false;
        }
    }
    else
    {
        LOG_INFO(QStringLiteral("Failed to unzip file from:%1 to %2").arg(sourceFile).arg(destDirectory).toStdString());
    }

    return success;
}

bool Extras::DataHandleManager::CompressFilesToDirectory(const QList<QString>& fileList, const QString& sourceFile, const QString& destDirectory)
{
    QString tempDirectory = sourceFile;
    if(destDirectory.isEmpty())
    {
        // 默认当前目录
        if(sourceFile.endsWith("csv"))
        {
            tempDirectory = sourceFile.left(sourceFile.length() - 3) + "zip";
        }
        else
        {
            tempDirectory.append(".zip");
        }
    }
    bool result = zip::ZipTool::CompressFiles(fileList, tempDirectory);
    return result;
}

QList<QString> Extras::DataHandleManager::WriteData(const QString& fileName, const QDate& dateTime)
{
    // 文件写入临时区域
    QFileInfo fileInfo(fileName);
    QString wirtFileName = fileInfo.fileName();
    QString tempWriteFile = Base::PathUtil::GetDataDir() + wirtFileName;

    QList<QString> fileLists;
    int fileMaxSize = 1000;
    int fixedQuantity = 200;
    int startRow = 0;

    bool joinHeader = true;
    bool queryState = true;
    QString sqlError;

    // todo 获取数据库数据
    auto dbm = Base::GetService<Extra::DatabaseManager>();
    auto db = dbm->GetRecordDatabase(dateTime);
    m_pushDml.SetDatabase(db);
    int offlineQuantity = m_pushDml.GetJobGroupsQuantity();
    
    // OfflineData分片
    int fileNum = 1;
    if(offlineQuantity > fileMaxSize)
    {
        fileNum = (offlineQuantity % fileMaxSize) != 0 ? (offlineQuantity / fileMaxSize) + 1 : offlineQuantity / fileMaxSize;
    }
    else
    {
        fileNum = 1;
        fileMaxSize = offlineQuantity;
    }

    for(int i = 0; i < fileNum; i++)
    {
        QString tempFileName = tempWriteFile.left(tempWriteFile.length()-4)+ "_"+QString::number(i)+ tempWriteFile.right(4);
        // 打开文件
        QFile file(tempFileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            LOG_ERROR(QStringLiteral("文件打开失败：%1").arg(tempFileName).toStdString());
            return fileLists;
        }
        fileLists.append(tempFileName);
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");   // 使用UTF-8编码
        int fileSize = fileMaxSize*i + fileMaxSize;
        joinHeader = true;
        while(startRow < fileSize)
        {
            Core::OfflineDataPtrList offlineDataList;
            if(!m_pushDml.GetFixQuantityJobGroups(offlineDataList, sqlError, fixedQuantity, startRow))
            {
                Q_ASSERT(0);
                queryState = false;
                LOG_ERROR(QStringLiteral("sql error: {%1}").arg(sqlError).toStdString());
                break;
            }
            else
            {
                Core::RecordDataPtrList recordDataList;
                // 组装数据
                QList<QStringList> dataList = JoinData(offlineDataList, recordDataList, dateTime.toString("yyyy-MM-dd"), joinHeader);
                if(joinHeader)
                    joinHeader = false;
                for(int i = 0; i < dataList.size(); i++)
                {
                    QStringList row = dataList.at(i);
                    QString line = row.join(",");
                    line += "\n";
                    QByteArray encodedLine = codec->fromUnicode(line);
                    file.write(encodedLine);
                }
                startRow += fixedQuantity;
            }
        }

        file.close();
    }
    
    // recordData分片
    fileMaxSize =2000;
    startRow = 0;
    m_recordDml.SetDatabase(db);
    int recordQuantity = m_recordDml.GetRecordsQuantity();

    int recordFileNum = 1;
    if(recordQuantity > fileMaxSize)
    {
        recordFileNum = (recordQuantity % fileMaxSize) != 0 ? (recordQuantity / fileMaxSize) + 1 : recordQuantity / fileMaxSize;
    }
    else
    {
        recordFileNum = 1;
        fileMaxSize = recordQuantity;
    }
    
    for(int i = 0; i < recordFileNum; i++)
    {
        QString tempFileName = tempWriteFile.left(tempWriteFile.length() - 4) + "_" + QString::number(i+ fileNum) + tempWriteFile.right(4);
        // 打开文件
        QFile file(tempFileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            LOG_ERROR(QStringLiteral("文件打开失败：%1").arg(tempFileName).toStdString());
            return fileLists;
        }
        fileLists.append(tempFileName);
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");   // 使用UTF-8编码
        int fileSize = fileMaxSize * i + fileMaxSize;
        joinHeader = true;
        while(startRow < fileSize)
        {
            Core::RecordDataPtrList recordDataList;
            if(!m_recordDml.GetFixQuantityRecords(recordDataList, sqlError, fixedQuantity, startRow))
            {
                Q_ASSERT(0);
                queryState = false;
                LOG_ERROR(QStringLiteral("sql error: {%1}").arg(sqlError).toStdString());
                break;
            }
            else
            {
                Core::OfflineDataPtrList offlineDataList;
                // 组装数据
                QList<QStringList> dataList = JoinData(offlineDataList, recordDataList, dateTime.toString("yyyy-MM-dd"), joinHeader);
                if(joinHeader)
                    joinHeader = false;
                for(int i = 0; i < dataList.size(); i++)
                {
                    QStringList row = dataList.at(i);
                    QString line = row.join(",");
                    line += "\n";
                    QByteArray encodedLine = codec->fromUnicode(line);
                    file.write(encodedLine);
                }
                startRow += fixedQuantity;
            }
        }
        file.close();
    }

    return fileLists;
}
