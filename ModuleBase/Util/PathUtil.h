#pragma once

#include "ModuleBase/ModuleBaseApi.h"

namespace Base
{
    /** 路径工具类，提供目录创建、目录路径查询等功能 */
    class MODULE_BASE_API PathUtil
    {
    public:
        /** 创建目录 */
        static QString CreateDir(const QString& dirPath);

        /** 删除目录 */
        static bool DeleteDir(const QString& dirPath);

        /** 拷贝目录 */
        static bool CopyDir(QString const& srcDir, QString const& destDir, bool force = true);

        /** 拷贝文件 */
        static bool CopyFileToPath(const QString& srcFile, const QString& destPath, bool force = true);

        /** 获取程序根目录 */
        static QString GetApplicationDir();

        /** 获取数据目录路径 C:/Users/用户名/Documents/PosServer/ */
        static QString GetDataDir();

        /** 获取数据目录路径 C:/Users/用户名/Documents/PosServer/Database/ */
        static QString GetDatabaseDir();

        /** 获取配置缓存目录路径 C:/Users/用户/Documents/PosServer/ConfigCache/ */
        static QString GetConfigCacheDir();

        /** 获取崩溃转储目录路径 C:/Users/用户/Documents/PosServer/CrashDump/ */
        static QString GetDumpDir();

        /** 获取日志目录路径 AppDir/Logs/ */
        static QString GetLogDir();

        /** 获取临时目录路径 C:/Users/用户/AppData/Local/Temp/ */
        static QString GetTempDir();

        /** 获取程序临时路径 C:/Users/用户/AppData/Local/Temp/PosServer/ */
        static QString GetTempPosServerDir();

        /** 获取文件站目录 C:/Users/用户名/Documents/PosServer/FileStation/ */
        static QString GetFileStationDir();

        /** 获取补丁下载路径 C:/Users/用户/AppData/Local/Temp/Patch/App/ */
        static QString GetAppPatchDir();
    };
};