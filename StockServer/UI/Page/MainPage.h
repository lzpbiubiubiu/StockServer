#pragma once

#include "Page.h"

namespace UI
{
    class MainPage : public Page
    {
        Q_OBJECT
        DECLARE_PAGE()

    public:
        Q_INVOKABLE MainPage(QObject* parent = nullptr);

        virtual ~MainPage();

    private:
    };
} // namespace UI