#include "PageController.h"

namespace UI
{
    PageController::PageController(Page* page)
        : QObject(page)
        , m_page(page)
    {
        qRegisterMetaType<PageController*>("PageController*");
    }

    PageController::~PageController()
    {}

    void PageController::OnPageLoad()
    {}

    void PageController::OnPageShow()
    {}

    void PageController::OnPageHide()
    {}

    void PageController::OnPageUnload()
    {}
}