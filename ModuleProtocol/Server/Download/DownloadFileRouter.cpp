#include "DownloadFileRouter.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleBase/Log/Log.h"
#include <fstream>
#include <algorithm>

namespace Http
{
    const QString DownloadFileRouter::GetName() const
    {
        return "/instore/download/(.*)";
    }

    const QString DownloadFileRouter::GetDescription() const
    {
        return "download";
    }

    const RoutePolicy DownloadFileRouter::GetPolicy() const
    {
        return RoutePolicy::ROUTE_GET;
    }

    const bool DownloadFileRouter::IsCheckHeader() const
    {
        return false;
    }

    bool DownloadFileRouter::CheckRequiredParams(const httplib::Request& req, QString& error)
    {
        return true;
    }

    void DownloadFileRouter::OnRouteHandler(const httplib::Request& req, httplib::Response& res,
            const HttpServerHeader& header, HttpServerResponsePtr& reponse)
    {
        std::string file_name = req.matches[1];
        file_name = Base::PathUtil::GetFileStationDir().toLocal8Bit().constData() + file_name;
        std::ifstream file(file_name, std::ios::binary);
        if(!file.is_open())
        {
            LOG_ERROR(QStringLiteral("文件打开失败,当前文件:%1").arg(QString::fromStdString(file_name)).toStdString());
            res.status = 404;
            res.set_content("File not found", "text/plain");
            return;
        }

        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);
        file.close();

        size_t content_length = 0;
        size_t offset = 0;
        if(req.has_header("Range"))
        {
            auto range = req.get_header_value("Range");
            auto pos1 = range.find("=");
            auto pos2 = range.find("-");
            offset = stoi(range.substr(pos1 + 1, pos2 - pos1 - 1));
            if(range.substr(pos2 + 1) != "")
                content_length = stoi(range.substr(pos2 + 1)) - offset + 1;
            else 
                content_length = file_size - offset;

            res.status = 206;
            res.set_header("Content-Range", "bytes " + std::to_string(offset) + "-" +
                std::to_string(offset + content_length - 1) + "/" + std::to_string(file_size));
        }
        else 
        {
            content_length = file_size;
            res.status = 200;
        }

        res.set_content_provider(file_size, "application/octet-stream",
            [file_name](size_t offset, size_t length, httplib::DataSink& sink) -> bool {
                std::ifstream file(file_name, std::ios::binary);
                if(!file.is_open())
                {
                    return false;
                }

                file.seekg(offset, std::ios::beg);
                char buffer[4096];
                while(!file.eof())
                {
                    memset(buffer, 0, 4096);
                    file.read(buffer, std::min(length, sizeof(buffer)));
                    auto gcount = file.gcount();
                    if(gcount > 0)
                        sink.write(buffer, gcount);
                }

                file.close();
                return true;
            });

        return;
    }
}