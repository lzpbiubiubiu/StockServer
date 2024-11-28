#include "Controller/MainPageController.h"
#include "MainPage.h"

namespace UI
{
    IMPLEMENT_PAGE("mainPage", MainPage)

    MainPage::MainPage(QObject* parent/* = nullptr*/)
        : Page(parent)
    {
        m_stockWarePanel = new StockWarePanel(this);
        SetController(new MainPageController(this));
        SetContentItem("qrc:/Qml/Page/MainPage.qml");
    }

    MainPage::~MainPage()
    {
    }

    StockWarePanel* MainPage::GetStockWarePanel() const
    {
        return m_stockWarePanel;
    }

    void MainPage::SetCurrentWareInfo(int index)
    {
        auto wareItem = m_stockWarePanel->GetModel()->GetItem(index);
        QJsonObject wareInfo;
        wareInfo["index"] = index;
        wareInfo["name"] = wareItem.name;
        wareInfo["code"] = wareItem.code;
        wareInfo["stock"] = wareItem.stock;
        wareInfo["retailPrice"] = wareItem.retailPrice;
        wareInfo["wholesalePrice"] = wareItem.wholesalePrice;
        wareInfo["imgPath"] = m_stockWarePanel->LoadLocalWareImage(wareItem.imageFimeName);
        m_currentWareInfo = wareInfo;
        emit signalCurrentWareInfoChanged();
    }

    QJsonObject MainPage::GetCurrentWareInfo() const
    {
        return m_currentWareInfo;
    }

} // namespace UI