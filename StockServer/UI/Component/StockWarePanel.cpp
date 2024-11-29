#include "StockWarePanel.h"
#include <QDebug>
#include <QtMath>
#include "ModuleBase/Service/ServiceManager.h"
#include "ModuleBase/Util/PathUtil.h"
#include "ModuleBase/Util/PriceUtil.h"
#include "Extras/ConfigManager.h"

namespace UI
{
    StockWarePanelModel::StockWarePanelModel(QObject* parent)
        : QAbstractListModel(parent)
    {}

    StockWarePanelModel::~StockWarePanelModel()
    {}

    void StockWarePanelModel::SetItems(const QList<StockWareItem>& items)
    {
        beginResetModel();
        m_items = items;
        endResetModel();
    }

    StockWarePanelModel::StockWareItem StockWarePanelModel::GetItem(int index) const
    {
        StockWareItem item;
        if(index >= 0 && index < m_items.size())
            item = m_items[index];
        return item;
    }


    void StockWarePanelModel::UpdateItem(int index, Core::ConfigWarePtr item)
    {
        if(index >= 0 && index < m_items.size())
        {
            auto& it = m_items[index];
            it.stock = QString::number(item->stock);
            it.retailPrice = Base::PriceUtil::FenToString(item->retailPrice);
            it.wholesalePrice = Base::PriceUtil::FenToString(item->wholesalePrice);
            it.imageFimeName = item->imageFileName;
            it.name = item->name;
            auto modelIndex = QAbstractListModel::index(index);
            emit dataChanged(modelIndex, modelIndex);
        }
    }

    void StockWarePanelModel::AddItem(const StockWareItem& item)
    {
        // 插入
        beginInsertRows(QModelIndex(), 0, 0);
        m_items.prepend(item);
        endInsertRows();
    }

    int StockWarePanelModel::GetIndex(const QString& code)
    {
        int index = -1;
        for(int i = 0; i < m_items.size(); i++)
        {
            if(m_items[i].code == code)
            {
                index = i;
                break;
            }
        }

        return index;
    }

    int StockWarePanelModel::rowCount(const QModelIndex& parent) const
    {
        return m_items.size();
    }

    QVariant StockWarePanelModel::data(const QModelIndex& index, int role) const
    {
        auto& itemData = m_items[index.row()];
        QVariant value;
        switch(role)
        {
        case ItemRole::CODE_ROLE:
            value = itemData.code;
            break;
        case ItemRole::NAME_ROLE:
            value = itemData.name;
            break;
        case ItemRole::STACK_ROLE:
            value = itemData.stock;
            break;
        case ItemRole::WHOLESALE_PRICE_ROLE:
            value = itemData.wholesalePrice;
            break;
        case ItemRole::RETAIL_PRICE_ROLE:
            value = itemData.retailPrice;
            break;
        case ItemRole::IMAGE_FIME_NAME_ROLE:
            value = itemData.imageFimeName;
            break;
        default:
            break;
        }
        return value;
    }

    QHash<int, QByteArray> StockWarePanelModel::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[ItemRole::CODE_ROLE] = "code";
        roles[ItemRole::NAME_ROLE] = "name";
        roles[ItemRole::STACK_ROLE] = "stock";
        roles[ItemRole::WHOLESALE_PRICE_ROLE] = "wholesalePrice";
        roles[ItemRole::RETAIL_PRICE_ROLE] = "retailPrice";
        roles[ItemRole::IMAGE_FIME_NAME_ROLE] = "imageFimeName";
        return roles;
    }

    StockWarePanel::StockWarePanel(QObject* parent)
        : QObject(parent)
    {
        m_modelData = new StockWarePanelModel(this);
    }

    StockWarePanel::~StockWarePanel()
    {

    }

    StockWarePanelModel* StockWarePanel::GetModel() const
    {
        return m_modelData;
    }

    StockWarePanelModel::StockWareItem StockWarePanel::GetPanelItem(Core::ConfigWarePtr item) const
    {
        StockWarePanelModel::StockWareItem wareItem;
        wareItem.name = item->name;
        wareItem.code = item->code;
        wareItem.stock = QString::number(item->stock);
        wareItem.retailPrice = Base::PriceUtil::FenToString(item->retailPrice);
        wareItem.wholesalePrice = Base::PriceUtil::FenToString(item->wholesalePrice);
        wareItem.imageFimeName = item->imageFileName;
        return wareItem;
    }

    void StockWarePanel::InitWares()
    {
        QList<StockWarePanelModel::StockWareItem> wareItemList;
        for(const auto& item : Base::GetService<Extra::ConfigManager>()->GetConfigWares())
        {
            wareItemList << GetPanelItem(item);
        }
        m_modelData->SetItems(wareItemList);
    }

    QString StockWarePanel::LoadLocalWareImage(const QString& imageFileName)
    {
        QString fileSource = "qrc:/Resources/Images/default_ware.svg";
        if(imageFileName.isEmpty())
            return fileSource;

        QString path = Base::PathUtil::GetFileStationDir();
        if(QFile::exists(path + imageFileName))
        {
            fileSource = "file:///" + path + imageFileName;
        }

        return fileSource;
    }

    void StockWarePanel::OnUpdateItem(int index, const Core::ConfigWarePtr item)
    {
        m_modelData->UpdateItem(index, item);
    }
    void StockWarePanel::OnAddItem(const Core::ConfigWarePtr item)
    {
        m_modelData->AddItem(GetPanelItem(item));
    }
}
