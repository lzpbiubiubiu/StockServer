#include "ModuleBase/Log/Log.h"
#include "TerminalPanel.h"

namespace UI
{
    TerminalPanelModel::TerminalPanelModel(QObject* parent/* = nullptr*/)
        : QAbstractListModel(parent)
    {
    }

    TerminalPanelModel::~TerminalPanelModel()
    {}

    void TerminalPanelModel::SetItems(const QList<TerminalItem>& items)
    {
        beginResetModel();
        m_items = items;
        endResetModel();
    }

    void TerminalPanelModel::UpdateItem(const TerminalItem& item)
    {
        for(int i = 0; i < m_items.size(); ++i)
        {
            if(m_items[i].id == item.id)
            {
                m_items[i] = item;
                emit dataChanged(index(i), index(i));
                break;
            }
        }
    }

    int TerminalPanelModel::rowCount(const QModelIndex& parent/* = QModelIndex()*/) const
    {
        return m_items.size();
    }

    QVariant TerminalPanelModel::data(const QModelIndex& index, int role) const
    {
        const auto& item = m_items[index.row()];
        QVariant value;
        switch(role)
        {
            case ItemRole::CONNECTED_ROLE:
                value = item.connected;
                break;
            case ItemRole::DESKNO_ROLE:
                value = item.deskNo;
                break;
            case ItemRole::DEVICECODE_ROLE:
                value = item.deviceCode;
                break;
            case ItemRole::UNSYNC_TASK:
                value = item.unsyncTask;
                break;
            case ItemRole::ID_ROLE:
                value = item.id;
                break;
            default:
                break;
        }

        return value;
    }

    QHash<int, QByteArray> TerminalPanelModel::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[ID_ROLE] = "id";
        roles[DESKNO_ROLE] = "deskNo";
        roles[DEVICECODE_ROLE] = "deviceCode";
        roles[UNSYNC_TASK] = "unsynctask";
        roles[CONNECTED_ROLE] = "connected";
        return roles;
    }

    TerminalPanel::TerminalPanel(QObject* parent)
        : QObject(parent)
    {
        m_posModelData = new TerminalPanelModel(this);
        m_pvtModelData = new TerminalPanelModel(this);
    }

    TerminalPanel::~TerminalPanel()
    {}

    TerminalPanelModel::TerminalItem TerminalPanel::GetPanelItem(const Extra::TerminalManager::Terminal& terminal) const
    {
        TerminalPanelModel::TerminalItem item;
        item.id = terminal.id;
        item.deskNo = terminal.deskNo;
        item.deviceCode = terminal.code;
        item.connected = terminal.connected;
        if(terminal.notUpLoadInStoreServer > 0 || terminal.notUploadCloudServer > 0)
        {
            QString notUpload = QString::number(terminal.notUpLoadInStoreServer);
            notUpload += "/";
            notUpload += QString::number(terminal.notUploadCloudServer);
            item.unsyncTask = notUpload;
        }
        else
        {
            item.unsyncTask = "";
        }
        return item;
    }

    void TerminalPanel::InitPos(const QList<Extra::TerminalManager::Terminal>& posTerminals)
    {
        QList<TerminalPanelModel::TerminalItem> posItems;
        for(auto& terminal : posTerminals)
        {
            posItems << GetPanelItem(terminal);
        }

        auto sortFunc = [](const TerminalPanelModel::TerminalItem& a, const TerminalPanelModel::TerminalItem& b)
            {
                if(a.connected == b.connected)
                {
                    return a.deskNo.toInt() < b.deskNo.toInt();
                }

                return !a.connected;
            };
        qSort(posItems.begin(), posItems.end(), sortFunc);
        m_posModelData->SetItems(posItems);
    }

    void TerminalPanel::InitPvt(const QList<Extra::TerminalManager::Terminal>& pvtTerminals)
    {
        QList<TerminalPanelModel::TerminalItem> pvtItems;

        for(auto& terminal : pvtTerminals)
        {
            pvtItems << GetPanelItem(terminal);
        }

        auto sortFunc = [](const TerminalPanelModel::TerminalItem& a, const TerminalPanelModel::TerminalItem& b)
            {
                if(a.connected == b.connected)
                {
                    return a.deskNo.toInt() < b.deskNo.toInt();
                }

                return !a.connected;
            };
        qSort(pvtItems.begin(), pvtItems.end(), sortFunc);

        m_pvtModelData->SetItems(pvtItems);
    }

    bool TerminalPanel::UpdatePosTerminalItem(const Extra::TerminalManager::Terminal& terminal)
    {
        m_posModelData->UpdateItem(GetPanelItem(terminal));

        return true;
    }

    bool TerminalPanel::UpdatePvtTerminalItem(const Extra::TerminalManager::Terminal& terminal)
    {
        m_pvtModelData->UpdateItem(GetPanelItem(terminal));

        return true;
    }
}