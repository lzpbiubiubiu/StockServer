#include <QApplication>
#include <QQmlComponent>
#include <QQmlContext>
#include <QJsonObject>
#include "ModuleBase/Log/Log.h"
#include "MainWindow.h"

namespace UI
{
    MainWindow::MainWindow(QSharedPointer<QQmlEngine> engine)
        : QObject(engine.data())
    {
        engine->rootContext()->setContextProperty("mainWindow", this);

        QQmlComponent component(engine.data(), QUrl(m_qmlFile));
        m_window = qobject_cast<QQuickWindow*>(component.create());
        Q_ASSERT_X(m_window != nullptr, "MainWindow", component.errorString().toUtf8());

        m_window->setScreen(QApplication::primaryScreen());

        m_statusBar = new StatusBar(this);
        
#ifndef _DEBUG
        SetFullScreen(true);
#endif // !DEBUG
    }

    MainWindow::~MainWindow()
    {
        if(m_window != nullptr)
        {
            m_window->close();
            m_window->deleteLater();
        }
    }

    void MainWindow::Show()
    {
        m_window->show();
    }

    void MainWindow::Tray()
    {
        m_window->hide();
    }

    void MainWindow::LoadPage(const QString& name, const QVariant& data/* = QVariant()*/)
    {
        // 页面已加载
        if(m_currentPageName == name)
            return;

        // 页面未注册
        auto metaClass = UI::Page::GetMeta()->FindClass(name);
        if(!metaClass)
            return;

        // 退出上一页
        if(m_pages.contains(m_currentPageName))
        {
            auto prev = m_pages[m_currentPageName];
            prev->OnHide();
            if(prev->IsAutoUnload())
            {
                prev->OnUnload();
                prev->deleteLater();
                m_pages.remove(m_currentPageName);
            }
        }

        // 加载当前页
        m_currentPageName = name;
        if(!m_pages.contains(m_currentPageName))
        {
            auto next = qobject_cast<Page*>(metaClass->newInstance(Q_ARG(QObject*, this)));
            next->SetName(name);
            next->SetUserData(data);
            next->OnLoad();
            m_pages[m_currentPageName] = next;
        }

        // 显示当前页
        if(m_window != nullptr && m_pages[m_currentPageName]->GetContentItem() != nullptr)
        {
            m_pages[m_currentPageName]->SetUserData(data);
            bool success = QMetaObject::invokeMethod(m_window, "loadPage", Q_ARG(QVariant, QVariant::fromValue(m_pages[m_currentPageName])));
            m_pages[m_currentPageName]->OnShow();
        }
    }

    void MainWindow::ShowLoading(const QString& text/* = QString()*/, bool delayShow/* = false*/)
    {
        QMetaObject::invokeMethod(m_window, "showLoading", Q_ARG(QVariant, text), Q_ARG(QVariant, delayShow));
    }

    void MainWindow::CloseLoading()
    {
        QMetaObject::invokeMethod(m_window, "closeLoading");
    }

    void MainWindow::ShowToast(const QString text, int duration)
    {
        QMetaObject::invokeMethod(m_window, "showToast", Q_ARG(QVariant, text), Q_ARG(QVariant, duration));
    }

    void MainWindow::ShowMessageBox(const QString& title, const QString& text, const QString& code/* = QString()*/)
    {
        QJsonObject obj;
        obj["title"] = title;
        obj["message"] = text;
        obj["code"] = code;
        obj["acceptButtonText"] = QStringLiteral("Confirm");
        QMetaObject::invokeMethod(m_window, "showMessageBox", Q_ARG(QVariant, obj));
    }

    bool MainWindow::IsFullScreen() const
    {
        return m_fullScreen;
    }

    void MainWindow::SetFullScreen(bool fullScreen)
    {
        if(m_fullScreen != fullScreen)
        {
            m_fullScreen = fullScreen;
            emit signalFullScreenChanged();
        }
    }

    bool MainWindow::IsTrayVisible() const
    {
        return m_trayVisible;
    }

    void MainWindow::SetTrayVisible(bool visible)
    {
        if(m_trayVisible != visible)
        {
            m_trayVisible = visible;
            emit signalTrayVisibleChanged();
        }
    }

    int MainWindow::GetLoadingRefCount() const
    {
        return m_loadingRefCount;
    }

    void MainWindow::SetLoadingRefCount(int count)
    {
        if(m_loadingRefCount != count)
        {
            m_loadingRefCount = count;
            emit signalLoadingRefCountChanged();
        }
    }

    void MainWindow::addPageMaskRefCount()
    {
        ++m_pageMaskRefCount[m_currentPageName];
        LOG_INFO(QStringLiteral("increase, currentPage is %1 mask count is %2").arg(m_currentPageName).arg(m_pageMaskRefCount[m_currentPageName]).toStdString());
    }

    void MainWindow::releasePageMaskRefCount()
    {
        if(m_pageMaskRefCount[m_currentPageName] <= 0)
        {
            LOG_WARN(QStringLiteral("decrease currentPage is %1 mask count is %2 less than zero").arg(m_currentPageName).arg(m_pageMaskRefCount[m_currentPageName]).toStdString());
            return;
        }

        --m_pageMaskRefCount[m_currentPageName];
        LOG_INFO(QStringLiteral("decrease, currentPage is %1 mask count is %2").arg(m_currentPageName).arg(m_pageMaskRefCount[m_currentPageName]).toStdString());
    }

    void MainWindow::tray()
    {
        Tray();
    }
} // namespace UI