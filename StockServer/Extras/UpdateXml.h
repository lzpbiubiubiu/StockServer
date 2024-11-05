#pragma once

#include <QString>
#include <QMap>
#include <QDomDocument>
#include "UpdateStruct.h"

namespace Extra
{
    class UpdateXml
    {
    public:
        UpdateXml(QString const& filePath = QString());
        ~UpdateXml(void);
        void Init(QString const& file);
        bool LoadFile();
        bool SaveFile();
        bool GetXmlHead(HeadInfo& headInfo);
        bool GetFiles(QMap<QString, UpdateFileInfo>& fileInfos);

        static UpdateXml LoadFromBuffer(const QByteArray& data);

    private:
        void AddElement(QDomElement& parent, QString const& key, QString const& value);

    private:
        QString m_filePath;
        int m_fileNum = 0;
        QDomDocument m_domDocument;
    };

}
