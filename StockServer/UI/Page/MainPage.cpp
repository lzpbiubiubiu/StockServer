#include "Controller/MainPageController.h"
#include "MainPage.h"

namespace UI
{
    IMPLEMENT_PAGE("mainPage", MainPage)

    MainPage::MainPage(QObject* parent/* = nullptr*/)
        : Page(parent)
    {

        SetController(new MainPageController(this));
        SetContentItem("qrc:/Qml/Page/MainPage.qml");
    }

    MainPage::~MainPage()
    {
    }


} // namespace UI