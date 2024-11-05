#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include "PathUtil.h"


namespace Base
{
    QString PathUtil::CreateDir(const QString& dirPath)
    {
        QDir dir;
        if(dir.mkpath(dirPath))
            return dirPath;
        else
            return QString();
    }

    bool PathUtil::DeleteDir(const QString& dirPath)
    {
        if(dirPath.isEmpty())
            return false;

        QDir dir(dirPath);
        if(!dir.exists())
            return true;

        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        QFileInfoList fileList = dir.entryInfoList();
        for(QFileInfo& fi : fileList)
        {
            if(fi.isFile())
                fi.dir().remove(fi.fileName());
            else
                DeleteDir(fi.absoluteFilePath());
        }
        return dir.rmpath(dir.absolutePath());
    }

    bool PathUtil::CopyDir(QString const& from, QString const& to, bool force/* = true*/)
    {
        QDir fromDir(from);
        if(!fromDir.exists())
            return false;

        if(!fromDir.mkpath(to))
            return false;

        QDir toDir(to);
        QFileInfoList entries = fromDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        for(QFileInfo& fi : entries)
        {
            QString s = fi.fileName();
            if(fi.isDir())
            {
                if(!CopyDir(fi.filePath(), toDir.filePath(fi.fileName())))
                    return false;
            }
            else
            {
                if(force && toDir.exists(fi.fileName()))
                    toDir.remove(fi.fileName());

                if(!QFile::copy(fi.filePath(), toDir.filePath(fi.fileName())))
                    return false;
            }
        }
        return true;
    }

    bool PathUtil::CopyFileToPath(const QString& srcFile, const QString& destPath, bool force)
    {
        if(!QFile::exists(srcFile))
            return false;

        QDir dir;
        QFileInfo fi(destPath);
        if(!dir.mkpath(fi.absolutePath()))
            return false;

        QString path = destPath;
        if(fi.isDir())
            path = QDir::cleanPath(fi.absolutePath() + QDir::separator() + QFileInfo(srcFile).fileName());

        if(QFile::exists(path) && !force)
            return true;

        QFile::remove(path);
        return QFile::copy(srcFile, path);
    }

    QString PathUtil::GetDatabaseDir()
    {
        static QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/StockServer/Database/";
        return CreateDir(dirPath);
    }

    QString PathUtil::GetConfigCacheDir()
    {
        static QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/StockServer/ConfigCache/";
        return CreateDir(dirPath);
    }

    QString PathUtil::GetApplicationDir()
    {
        QString path = QCoreApplication::applicationDirPath() + "/";
        return path;
    }

    QString PathUtil::GetDumpDir()
    {
        static QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/StockServer/CrashDump/";
        return CreateDir(dirPath);
    }

    QString PathUtil::GetLogDir()
    {
        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();
        QString dirPath = dir.path() + "/Logs/";
        return CreateDir(dirPath);
    }

    QString PathUtil::GetDataDir()
    {
        static QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/StockServer/";
        return CreateDir(dirPath);
    }

    QString PathUtil::GetTempDir()
    {
        return QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/";
    }

    QString PathUtil::GetTempPosServerDir()
    {
        QString tempPosDir = GetTempDir() + "StockServer/";
        return CreateDir(tempPosDir);
    }

    QString PathUtil::GetFileStationDir()
    {
        static QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/StockServer/FileStation/";
        return CreateDir(dirPath);
    }

    QString PathUtil::GetOfflinePacketDir()
    {
        QString dirPath = GetFileStationDir() + "OfflinePacket/";
        return CreateDir(dirPath);
    }

    QString PathUtil::GetAppPatchDir()
    {
        static QString tempPosDir = GetTempDir() + "Patch/App/";
        return CreateDir(tempPosDir);
    }
}
