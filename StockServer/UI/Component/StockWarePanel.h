
#pragma once

#include <QAbstractListModel>
#include "ModuleData/Core/GlobalStructData.h"

namespace UI
{
    /** 首页商品面板数据 */
    class StockWarePanelModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        StockWarePanelModel(QObject* parent);
        ~StockWarePanelModel();

        enum ItemRole
        {
            /** 商品码 */
            CODE_ROLE = Qt::UserRole + 1, 

            /** 名称 */
            NAME_ROLE,

            /** 库存 */
            STACK_ROLE,

            /** 批发价格 */
            WHOLESALE_PRICE_ROLE,

            /** 零售价格 */
            RETAIL_PRICE_ROLE,

            /** 图片文件名 */
            IMAGE_FIME_NAME_ROLE
        };

        /** 商品列表项 */
        struct StockWareItem
        {
            /** 商品编码 */
            QString code;

            /** 商品名称 */
            QString name;

            /** 单品零售价格 */
            QString retailPrice;

            /** 单品批发价格 */
            QString wholesalePrice;

            /** 库存 */
            QString stock;

            /** 图片文件名 */
            QString imageFimeName;
        };

        /** 设置列表数据 */
        void SetItems(const QList<StockWareItem>& items);

        /** 获取商品 */
        StockWareItem GetItem(int index) const;

        /** 更新子项 */
        void UpdateItem(int index, Core::ConfigWarePtr item);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        virtual QVariant data(const QModelIndex& index, int role) const override;

        virtual QHash<int, QByteArray> roleNames() const override;


    private:
        // 商品列表
        QList<StockWareItem> m_items;

        // 当前修改商品的索引
        int m_currentIndex = 0;
    };

    /** 购物面板 StockWarePanel */
    class StockWarePanel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QObject* model MEMBER m_modelData CONSTANT)
    public:
        StockWarePanel(QObject* parent = nullptr);
        ~StockWarePanel();

        /** 获取UI商品数据 */
        StockWarePanelModel::StockWareItem GetPanelItem(Core::ConfigWarePtr item) const;

        /** 获取数据模型 */
        StockWarePanelModel* GetModel() const;

        /** 初始化商品 */
        void InitWares();

        /** 根据图片文件名获取本地商品图片 */
        QString LoadLocalWareImage(const QString &imageFileName);

        /** 修改商品信息 */
        void OnUpdateItem(int index, const Core::ConfigWarePtr item);
    Q_SIGNALS:

    private:

        // 商品列表数据模型
        StockWarePanelModel* m_modelData = nullptr;
    };
}
