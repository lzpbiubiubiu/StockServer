#include <QDir>
#include "FetchOfflineDataRouter.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleBase/Util/MathUtil.h"
#include "ModuleData/Core/GlobalData.h"
#include "ModuleBase/Log/Log.h"

namespace Http
{
    const QString FetchOfflineDataRouter::GetName() const
    {
        return "/instore/terminal/fetchOfflineData";
    }

    const QString FetchOfflineDataRouter::GetDescription() const
    {
        return "fetch offline data";
    }

    const RoutePolicy FetchOfflineDataRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_POST;
    }

    const bool FetchOfflineDataRouter::IsCheckHeader() const
    {
        return false;
    }

    bool FetchOfflineDataRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        QJsonObject object = GetThreadLocal().object;
        do
        {
            if(!CHECK_REQUIRED_PARAM(object, "appKey"))
            {
                error = "appKey is required";
                break;
            }
        } while(0);

        return error.isEmpty();
    }

    void FetchOfflineDataRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        QJsonObject object = GetThreadLocal().object;

        QString appKey;
        ASSIGN_POST_PARAM_TO_STRING(object, "appKey", appKey);

        QString folderPath = Base::PathUtil::GetOfflinePacketDir();
        QDir dir(folderPath);
        if(!dir.exists())
        {
            reponse = HttpServerResponsePtr::create();
            reponse->code = HTTP_SERVER_OFFLINE_PACKET_FOLDER_ERROR;
            reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_OFFLINE_PACKET_FOLDER_ERROR;
            reponse->msg = "offline packet folder not exists";
            reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_OFFLINE_PACKET_FOLDER_ERROR];
            reponse->traceId = header.traceId;
            reponse->success = false;
            return;
        }

        QString findFile, findVersion;
        QStringList zipFiles = dir.entryList(QStringList() << "*.zip", QDir::Files);
        foreach(const QString& file, zipFiles)
        {
            QStringList nameList = file.split("__");
            if(nameList.size() == 2 && nameList[0] == appKey)
            {
                findFile = file;
                QString second = nameList[1];
                findVersion = second.remove(second.length() - 4, 4);
                break;
            }
        }

        QJsonObject data;
        if(findFile.isEmpty())
        {
            LOG_INFO(QStringLiteral("文件站中没有找到该文件,当前文件夹:%1,当前文件名:%2").arg(folderPath).arg(appKey).toStdString());
        }
        else
        {
            data["md5"] = Base::MathUtil::GetFileMD5(folderPath + findFile);
            data["version"] = findVersion;
            data["type"] = 1;
            const auto& device = Core::GlobalData::Get()->GetConfig().device;
            QString url = QString("http://%1:%2/instore/download/%3/%4")
                .arg(device.ip).arg(device.port)
                .arg(dir.dirName()).arg(findFile);
            data["url"] = url;
        }

        reponse = HttpServerResponsePtr::create();
        reponse->code = HTTP_SERVER_SUCCESS;
        reponse->displayCode = HTTP_SERVER_ERROR_CODE_PREFIX + HTTP_SERVER_SUCCESS;
        reponse->msg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->displayMsg = HTTP_SERVER_ERROR_MESSAGES[HTTP_SERVER_SUCCESS];
        reponse->traceId = header.traceId;
        reponse->data = data;
    }
}