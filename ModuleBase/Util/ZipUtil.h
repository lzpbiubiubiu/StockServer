#pragma once

#include "ModuleBase/ModuleBaseApi.h"
#include <QString>

namespace Base
{
    //压缩工具类
    class MODULE_BASE_API ZipUtil
    {
    public:
        /** 压缩文件 */
        static bool CompressFile(const QString& zipFile, const QString& fileDest);

        /** 压缩多个文件 */
        static bool CompressFiles(const QString& zipFile, const QStringList& files);

        /** 压缩目录 */
        static bool CompressDir(const QString& zipFile, const QString& dirDest);

        /** 解压文件 */
        static bool ExtractFile(const QString& zipFile, const QString& fileName, QByteArray& buffer);

        /** 解压目录 */
        static bool ExtractDir(const QString& zipFile, const QString& path);

        /** 解压目录并返回文件列表 */
        static bool ExtractDir(const QString& zipFile, const QString& path, QStringList& files);

    private:
        /** 获取目录所有文件 */
        static QStringList GetDirFiles(const QString& dirPath);
    };
}