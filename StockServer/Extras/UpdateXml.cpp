#include "UpdateXml.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace Extra
{
    UpdateXml::UpdateXml(QString const& filePath)
        : m_filePath(filePath)
        , m_fileNum(0)
    {

    }

    UpdateXml::~UpdateXml(void)
    {
    }

    void UpdateXml::Init(QString const& file)
    {
        m_filePath = file;
    }

    bool UpdateXml::LoadFile()
    {
        bool res = false;
        QFile file(m_filePath); //相对路径、绝对路径、资源路径都可以
        if(!file.open(QFile::ReadWrite))
            return false;

        res = m_domDocument.setContent(&file);
        file.close();

        return res;
    }
    bool UpdateXml::SaveFile()
    {
        QDomElement head = m_domDocument.firstChildElement("head");
        if(!head.isNull())
        {
            qDebug() << "head element is null";
            Q_ASSERT(false);
            //把文件数加上
            AddElement(head, "filenum", QString::number(m_fileNum));
        }


        QFile file(m_filePath); //相对路径、绝对路径、资源路径都可以
        if(!file.open(QFile::WriteOnly | QFile::Truncate)) //可以用QIODevice，Truncate表示清空原来的内容
        {
            return false;
        }

        //输出到文件
        QTextStream outStream(&file);
        m_domDocument.save(outStream, 4); //缩进4格
        file.close();

        return true;
    }

    bool UpdateXml::GetXmlHead(HeadInfo& headInfo)
    {
        QDomNodeList head = m_domDocument.elementsByTagName("head");
        if(head.size() < 1 || head.size() > 1)
        {
            qDebug() << "head element error";
            return false;
        }

        QDomNode node = head.item(0);

        headInfo.title = node.firstChildElement("title").text();
        headInfo.newVer = node.firstChildElement("newVer").text();
        headInfo.oldVer = node.firstChildElement("oldVer").text();

        headInfo.updateTime = node.firstChildElement("updatetime").text();
        headInfo.fileNum = node.firstChildElement("filenum").text().toInt();

        return true;
    }

    bool UpdateXml::GetFiles(QMap<QString, UpdateFileInfo>& file_infos)
    {
        QDomNodeList file_nodes = m_domDocument.elementsByTagName("file");
        if(file_nodes.isEmpty())
        {
            qDebug() << "file element is empty";
            return false;
        }

        for(int i = 0; i < file_nodes.size(); ++i)
        {
            UpdateFileInfo updateFileInfo;
            QDomNode node = file_nodes.item(i);

            updateFileInfo.name = node.firstChildElement("name").text();
            updateFileInfo.type = node.firstChildElement("type").text().toInt();
            updateFileInfo.path = node.firstChildElement("path").text();
            updateFileInfo.size = node.firstChildElement("size").text().toInt();
            updateFileInfo.md5 = node.firstChildElement("md5").text();
            updateFileInfo.oldMd5 = node.firstChildElement("oldmd5").text();
            updateFileInfo.bPatch = node.firstChildElement("havepatch").text().toInt() == 1;
            if(updateFileInfo.bPatch)
            {
                updateFileInfo.patchFile.name = node.firstChildElement("patch").firstChildElement("name").text();
                updateFileInfo.patchFile.size = node.firstChildElement("patch").firstChildElement("size").text().toInt();
                updateFileInfo.patchFile.md5 = node.firstChildElement("patch").firstChildElement("md5").text();
            }

            file_infos[updateFileInfo.path + updateFileInfo.name] = updateFileInfo;
        }

        return true;
    }


    void UpdateXml::AddElement(QDomElement& parent, QString const& key, QString const& value)
    {
        QDomElement ele = m_domDocument.createElement(key);
        QDomText ele_text = m_domDocument.createTextNode(value);
        ele.appendChild(ele_text);
        parent.appendChild(ele);
    }

    UpdateXml UpdateXml::LoadFromBuffer(const QByteArray& data)
    {
        UpdateXml xml;
        xml.m_domDocument.setContent(data);
        return xml;
    }
}