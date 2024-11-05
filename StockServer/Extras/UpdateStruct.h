#pragma once

#include <QString>

namespace Extra
{
    struct UpdateParam
    {
        QString version;
        QString patchVersion;
        QString url;
        QString patchUrl;
        QString md5;
        QString patchMd5;
        QString filePath;
        bool force = false;
    };

    struct HeadInfo
    {
        QString title;
        QString newVer;
        QString oldVer;
        QString updateTime;
        int fileNum = 0;
    };

    struct FileInfo
    {
        QString name;
        QString md5;
        QString path;
        quint32 size = 0;
        int type = 0;
    };

    struct PatchFileInfo
    {
        QString name;
        QString md5;
        int size = 0;
    };

    struct UpdateFileInfo
    {
        QString name;
        int     type = 0;         //0 表示文件，1表示目录
        QString path;
        int     size = 0;
        QString md5;
        QString oldMd5;
        bool    bPatch = false;    //是否有补丁
        PatchFileInfo patchFile;
    };

    struct UpdateXmlFileInfo
    {
        QString title;          //应用名称
        QString location;       //主程序启动程序名
        QString version;        //版本号
        QString updateTime;     //升级时间
        QString updateFile;     //升级文件
        int     result = 1;     //升级结果，1：成功   2：安装补丁失败  3：下载失败 4，解压失败
    };


    enum UpdateResultType
    {
        URT_SUCC,
        URT_UNZIP_FAILE,        //解压失败
        URT_LOAD_XML_FAILE,     //加载xml失败
        URT_FILE_NUM_FAILE,     //解析xml后文件数量和head数量不同
        URT_MAKE_PATCH_FAILE,   //生成补丁失败
        URT_COPY_FILE_FAILE,    //复制文件失败
    };
}