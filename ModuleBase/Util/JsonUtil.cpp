#include <QJsonDocument>
#include <QJsonParseError>

#include "JsonUtil.h"

namespace Base
{
    QJsonObject JsonUtil::ToJsonObject(const QByteArray& byteData)
    {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(byteData, &error);
        if(error.error != QJsonParseError::NoError)
            return {};

        return doc.object();
    }

    QJsonObject JsonUtil::ToJsonObject(const QString& str)
    {
        return ToJsonObject(str.toUtf8());
    }

    QString JsonUtil::ToString(const QJsonObject& jsonObj, bool indent /*= false*/)
    {
        QJsonDocument doc(jsonObj);
        return QString::fromUtf8(doc.toJson(indent ? QJsonDocument::Indented : QJsonDocument::Compact));
    }

} // namespace Base