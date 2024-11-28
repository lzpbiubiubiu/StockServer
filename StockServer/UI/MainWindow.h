#pragma once

/**
* @file         MainWindow.h
* @brief        主窗口类
* @copyright    lizhongpu
*/

#include <QMap>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QSharedPointer>
#include <QString>
#include "UI/Page/Page.h"
#include "UI/Component/StatusBar.h"

namespace UI
{
    class MainWindow : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool fullScreen READ IsFullScreen WRITE SetFullScreen NOTIFY signalFullScreenChanged)
        Q_PROPERTY(int loadingRefCount READ GetLoadingRefCount WRITE SetLoadingRefCount NOTIFY signalLoadingRefCountChanged)
        Q_PROPERTY(bool trayVisible READ IsTrayVisible WRITE SetTrayVisible NOTIFY signalTrayVisibleChanged)
        Q_PROPERTY(QObject* item MEMBER m_window CONSTANT)
        Q_PROPERTY(QObject* statusBar MEMBER m_statusBar CONSTANT)

    public:
        MainWindow(QSharedPointer<QQmlEngine> engine);
        ~MainWindow();

        /** 显示窗口 */
        void Show();

        /** 最小化托盘 */
        void Tray();

        /** 加载页面 */
        void LoadPage(const QString& name, const QVariant& data = QVariant());

        /** 显示加载界面 */
        void ShowLoading(const QString& text = QString(), bool delayShow = false);

        /** 关闭加载界面 */
        void CloseLoading();

        /** 显示Toast弹窗 */
        void ShowToast(const QString text, int duration);

        /** 显示简单消息提示框 */
        void ShowMessageBox(const QString& title, const QString& text, const QString& code = QString());

        /** 是否全屏相关 */
        bool IsFullScreen() const;
        void SetFullScreen(bool fullScreen);

        /** 是否显示托盘相关 */
        bool IsTrayVisible() const;
        void SetTrayVisible(bool visible);

        /** loading计数相关 */
        int GetLoadingRefCount() const;
        void SetLoadingRefCount(int count);

    protected:
        /** 导出全局JS函数：当前界面蒙层计数器+1 */
        Q_INVOKABLE void addPageMaskRefCount();

        /** 导出全局JS函数：当前界面蒙层计数器-1 */
        Q_INVOKABLE void releasePageMaskRefCount();

        /** 最小化托盘 */
        Q_INVOKABLE void tray();

    Q_SIGNALS:
        void signalFullScreenChanged();
        void signalLoadingRefCountChanged();
        void signalTrayVisibleChanged();

    private:
        QQuickWindow* m_window = nullptr;

        QString m_qmlFile = QStringLiteral("qrc:/Qml/MainWindow.qml");

        // 所有页面
        QHash<QString, Page*> m_pages;

        // 当前页面
        QString m_currentPageName;

        // 界面蒙层计数器
        QMap<QString, int> m_pageMaskRefCount;

        // loading计数器
        int m_loadingRefCount = 0;

        // 是否全屏
        bool m_fullScreen = false;

        // 是否显示托盘
        bool m_trayVisible = true;

        // 状态栏
        StatusBar* m_statusBar = nullptr;
    };
} // namespace UI