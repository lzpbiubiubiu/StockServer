#pragma once

#include "Page.h"
#include "UI/Component/StockWarePanel.h"
namespace UI
{
    class MainPage : public Page
    {
        Q_OBJECT
        Q_PROPERTY(QObject* stockWarePanel MEMBER m_stockWarePanel CONSTANT)
        Q_PROPERTY(QJsonObject currentWareInfo READ GetCurrentWareInfo NOTIFY signalCurrentWareInfoChanged)
        DECLARE_PAGE()

    public:
        Q_INVOKABLE MainPage(QObject* parent = nullptr);
        virtual ~MainPage();

        /** 设置|获取当前的商品信息 */
        void SetCurrentWareInfo(int index);
        QJsonObject GetCurrentWareInfo() const;

        /** 获取商品加购面板 */
        StockWarePanel* GetStockWarePanel() const;

    Q_SIGNALS:
        void signalCurrentWareInfoChanged();

    private:

        // 商品列表面板
        StockWarePanel* m_stockWarePanel = nullptr;

        // 当前选择的商品信息
        QJsonObject m_currentWareInfo;
    };
} // namespace UI