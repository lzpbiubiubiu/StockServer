#include <QJsonObject>
#include "Controller/StartupPageController.h"
#include "StartupPage.h"

namespace UI
{
    IMPLEMENT_PAGE("startupPage", StartupPage)

    StartupPage::StartupPage(QObject* parent/* = nullptr*/)
        : Page(parent)
    {
        SetController(new StartupPageController(this));
        SetContentItem("qrc:/Qml/Page/StartupPage.qml");
    }

    StartupPage::~StartupPage()
    {
    }

    QJsonArray StartupPage::GetInfos() const
    {
        return m_infos;
    }

    void StartupPage::AddInfo(const QString& key, const QString& value)
    {
        QJsonObject item;
        item["key"] = key;
        item["value"] = value;
        m_infos.append(item);
        emit signalInfosChanged();
    }

    void StartupPage::UpdateInfo(const QString& key, const QString& value)
    {
        for(auto i : m_infos)
        {
            if(i.isObject())
            {
                if(i.toObject().value("key").toString() == key)
                {
                    QJsonObject obj;
                    obj["key"] = key;
                    obj["value"] = value;
                    i = obj;
                    emit signalInfosChanged();
                    break;
                }
            }
        }
    }

    void StartupPage::ResetInfos()
    {
        m_infos = QJsonArray();
        emit signalInfosChanged();
    }

    void StartupPage::RemoveInfo(const QString& key)
    {
        for(int i = 0; i < m_infos.size(); ++i)
        {
            auto obj = m_infos[i];
            if(obj.isObject() && obj.toObject()["key"] == key)
            {
                m_infos.removeAt(i);
                emit signalInfosChanged();
                break;
            }
        }
    }

    int StartupPage::GetButtons() const
    {
        return m_buttons;
    }

    void StartupPage::SetButtons(Buttons buttons)
    {
        if(m_buttons != buttons)
        {
            m_buttons = buttons;
            emit signalButtonsChanged();
        }
    }

} // namespace UI