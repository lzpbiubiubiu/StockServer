#include "7ZipTool.h"
#include <QDir>
#include <QScopedPointer>
#include <shared_mutex>
#include <string>
#include "bit7zlibrary.hpp"
#include "bitcompressor.hpp"
#include "bitexception.hpp"
#include "bitextractor.hpp"
#include "ModuleBase/Log/Log.h"

using namespace std;
using namespace bit7z;

namespace zip
{
    bool ZipTool::CompressFiles(const QList<QString>& zipFiles, const QString& fileDest)
    {
        shared_ptr<BitCompressor> compressor = nullptr;

        bit7z::Bit7zLibrary lib(L"7z.dll");

        if(fileDest.endsWith("zip"))
        {
            compressor = make_shared<BitCompressor>(lib, BitFormat::Zip);
        }
        else if(fileDest.endsWith("tar"))
        {
            compressor = make_shared<BitCompressor>(lib, BitFormat::Tar);
        }
        else if(fileDest.endsWith("gz"))
        {
            compressor = make_shared<BitCompressor>(lib, BitFormat::GZip);
        }
        else if(fileDest.endsWith("bz2"))
        {
            compressor = make_shared<BitCompressor>(lib, BitFormat::BZip2);
        }
        else
        {
            compressor = make_shared<BitCompressor>(lib, BitFormat::SevenZip);
        }
        bit7z::TotalCallback TotalCB = TotalSize;
        bit7z::ProgressCallback ProCB = ProgressState;
        bit7z::FileCallback fc = FileNameCB;
        try {
            compressor->setTotalCallback(TotalSize);
            compressor->setProgressCallback(ProgressState);
            compressor->setFileCallback(FileNameCB);
            compressor->setPassword(L"2KABFX");

            std::vector<std::wstring> files;
            for(int i = 0; i < zipFiles.size(); ++i)
            {
                files.push_back(zipFiles[i].toStdWString());
            }
            compressor->compressFiles(files, fileDest.toStdWString());
            compressor->setUpdateMode(true);

            return true;
        }
        catch(const BitException& ex) {
            QString err = QString(ex.what());
            LOG_ERROR("compress files error: {}",err.toStdString());
            return false;
        }
    }

    bool ZipTool::CompressDir(const QString& zipFile, const QString& dirDest)
    {
        // todo
        return true;
    }

    bool ZipTool::ExtractFile(const QString& zipFile, const QString& fileName, QByteArray& buffer)
    {
        // todo 
        return false;
    }

    bool ZipTool::ExtractDir(const QString& zipFile, const QString& path)
    {
        // todo
        return false;
    }

    QStringList ZipTool::GetDirFiles(const QString& dirPath)
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

    void ZipTool::TotalSize(int size)
    {
        int totalSize = size;
    }

    void ZipTool::ProgressState(int pro)
    {
        // to do 
    }

    void ZipTool::FileNameCB(std::wstring filename)
    {
        // to do
        QString tempFile = QString::fromStdWString(filename);
    }
}