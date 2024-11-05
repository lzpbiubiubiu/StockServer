#pragma once

#include "External/7zip/Module7ZipApi.h"
#include <QString>

namespace zip
{
    //压缩工具类
    class MODULE_7ZIP_API ZipTool
    {
    public:
        /** 压缩文件 */
        static bool CompressFiles(const QList<QString>& zipFiles, const QString& fileDest);

        /** 压缩目录 */
        static bool CompressDir(const QString& zipFile, const QString& dirDest);

        /** 解压文件 */
        static bool ExtractFile(const QString& zipFile, const QString& fileName, QByteArray& buffer);

        /** 解压目录 */
        static bool ExtractDir(const QString& zipFile, const QString& path);

    private:
        /** 获取目录所有文件 */
        static QStringList GetDirFiles(const QString& dirPath);

        /** total callback */
        static void TotalSize(int size);

        /** progress callback */
        static void ProgressState(int pro);

        /** file callback */
        static void FileNameCB(std::wstring filename);
    };
}