#include <QUuid>
#include <QTime>
#include <QtMath>
#include <QCryptographicHash>
#include <QFile>
#include "MathUtil.h"

namespace Base
{
    QString MathUtil::GetUUID()
    {
        QUuid id = QUuid::createUuid();
        QString uuid = id.toString();
        uuid.remove("{").remove("}").remove("-");
        return uuid.toLower();
    }

    QString MathUtil::GetNonce()
    {
        return GetUUID().mid(0, 16);
    }

    QString MathUtil::GetMD5(const QString& content)
    {
        QByteArray md5 = QCryptographicHash::hash(content.toUtf8(), QCryptographicHash::Md5).toHex();
        return md5.toLower();
    }

    QString MathUtil::GetFileMD5(const QString& fileName)
    {
        if(!QFile::exists(fileName))
            return "";

        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
            return "";

        QCryptographicHash hash(QCryptographicHash::Md5);
        while(!file.atEnd())
        {
            // 64K
            QByteArray data = file.read(64 * 1024);
            hash.addData(data);
        }

        file.close();
        return QString(hash.result().toHex()).toLower();
    }
}