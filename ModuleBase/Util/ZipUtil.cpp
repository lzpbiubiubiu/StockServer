#include "ZipUtil.h"
#include <QDir>
#include <QScopedPointer>
#include "QtGui/private/qzipreader_p.h"
#include "QtGui/private/qzipwriter_p.h"

namespace Base
{
    bool ZipUtil::CompressFile(const QString& zipFile, const QString& fileDest)
    {
        QFile file(fileDest);
        if(!file.exists())
            return false;

        QScopedPointer<QZipWriter> writer(new QZipWriter(zipFile));
        if(writer.isNull())
            return false;

        if(!file.open(QIODevice::ReadOnly))
            return false;

        QFileInfo fileInfo(fileDest);
        writer->addFile(fileInfo.fileName(), file.readAll());

        QZipWriter::Status status = writer->status();
        file.close();
        writer->close();

        return status == QZipWriter::NoError;
    }

    bool ZipUtil::CompressFiles(const QString& zipFile, const QStringList& files)
    {
        QScopedPointer<QZipWriter> writer(new QZipWriter(zipFile));
        if(writer.isNull())
            return false;

        for(const QString& filePath : files)
        {
            QFile file(filePath);
            if(!file.exists())
                continue;

            if(!file.open(QIODevice::Truncate | QIODevice::ReadOnly))
                continue;

            QFileInfo fileInfo(filePath);
            writer->addFile(fileInfo.fileName(), file.readAll());
            file.close();
        }

        QZipWriter::Status status = writer->status();
        writer->close();

        return status == QZipWriter::NoError;
    }

    bool ZipUtil::CompressDir(const QString& zipFile, const QString& dirDest)
    {
        QFileInfo fileInfo(dirDest);
        if(!fileInfo.isDir())
            return false;

        QScopedPointer<QZipWriter> writer(new QZipWriter(zipFile));
        if(writer.isNull())
            return false;

        //读取路径下所有文件路
        QStringList fileList = GetDirFiles(dirDest);
        for(QString& filePath : fileList)
        {
            QFile file(filePath);
            if(!file.exists())
                continue;

            if(!file.open(QIODevice::Truncate | QIODevice::ReadOnly))
                continue;

            QString path = filePath.remove(QDir::fromNativeSeparators(dirDest));
            path = path.mid(1, path.size());

            writer->addFile(path, file.readAll());
            file.close();
        }

        QZipWriter::Status status = writer->status();
        writer->close();

        return status == QZipWriter::NoError;
    }

    bool ZipUtil::ExtractFile(const QString& zipFile, const QString& fileName, QByteArray& buffer)
    {
        QZipReader reader(zipFile);
        buffer = reader.fileData(fileName);
        QZipReader::Status status = reader.status();
        reader.close();

        return status == QZipReader::NoError;
    }

    bool ZipUtil::ExtractDir(const QString& zipFile, const QString& path)
    {
        QDir tempDir;
        if(!tempDir.exists(path))
            tempDir.mkpath(path);

        QZipReader reader(zipFile);
        bool ret = reader.extractAll(path);
        reader.close();

        return ret;
    }

    bool ZipUtil::ExtractDir(const QString& zipFile, const QString& path, QStringList& files)
    {
        QDir tempDir;
        if(!tempDir.exists(path))
            tempDir.mkpath(path);

        QZipReader reader(zipFile);
        bool ret = reader.extractAll(path);
        if(ret)
        {
            for(QZipReader::FileInfo fileInfo : reader.fileInfoList())
            {
                if(fileInfo.isDir)
                    continue;
                files << fileInfo.filePath;
            }
        }
        reader.close();
        return ret;
    }


    QStringList ZipUtil::GetDirFiles(const QString& dirPath)
    {
        QStringList fileList;
        QDir dir(dirPath);
        if(!dir.exists())
        {
            return fileList;
        }

        for(QFileInfo& fileInfo : dir.entryInfoList())
        {
            if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
                continue;

            if(fileInfo.isDir())
                fileList.append(GetDirFiles(fileInfo.filePath()));
            else if(fileInfo.isFile())
                fileList.append(fileInfo.filePath());
        }

        return fileList;
    }
}