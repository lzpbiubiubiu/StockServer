#include <QFileInfo>
#include <QQmlContext>
#include "Application.h"
#include "Controller/PageController.h"
#include "Page.h"

namespace UI
{
    Page::Page(QObject* parent)
        : QObject(parent)
    {
        auto rootContext = Application::Get()->GetQmlEngine()->rootContext();
        m_context = new QQmlContext(rootContext);
        m_context->setContextProperty("page", this);
    }

    Page::~Page()
    {
        if (m_context) {
            m_context->setParent(0);
            delete m_context;
            m_context = 0;
        }
    }

    void Page::SetName(const QString& name)
    {
        m_name = name;
    }

    void Page::SetUserData(const QVariant& data)
    {
        m_userData = data;
    }

    void Page::SetContextProperty(const QString& name, const QVariant& data)
    {
        m_context->setContextProperty(name, data);
    }

    void Page::SetContextProperty(const QString& name, QObject* object)
    {
        m_context->setContextProperty(name, object);
    }

    void Page::SetContentItem(const QString& qmlFile)
    {
        QFileInfo fi(qmlFile);
        m_qmlFile = fi.completeBaseName();
        QString qmlError;
        QQmlComponent component(Application::Get()->GetQmlEngine().data(), QUrl(qmlFile));
        m_item = qobject_cast<QQuickItem*>(component.create(m_context));
        qmlError = component.errorString();
        Q_ASSERT_X(m_item != nullptr, "QML syntax error", qmlError.toUtf8());
        m_item->setParent(this);
    }

    void Page::SetAutoUnload(bool enabled)
    {
        m_enableAutoUnload = enabled;
    }

    void Page::SetController(PageController* controller)
    {
        Q_ASSERT(controller != nullptr);
        m_controller = controller;
    }

    QQuickItem* Page::FindItemByName(const QString& objectName) const
    {
        QQuickItem* item = nullptr;
        if(m_item != nullptr)
            item = m_item->findChild<QQuickItem*>(objectName);
        return item;
    }

    const QString& Page::GetName() const
    {
        return m_name;
    }

    const QVariant& Page::GetUserData() const
    {
        return m_userData;
    }

    QQmlContext* Page::GetContext() const
    {
        return m_context;
    }

    QQuickItem* Page::GetContentItem() const
    {
        return m_item;
    }

    bool Page::IsAutoUnload() const
    {
        return m_enableAutoUnload;
    }

    bool Page::IsVisible() const
    {
        if(m_item)
            return m_item->isVisible();
        return false;
    }

    PageMeta* Page::GetMeta()
    {
        static PageMeta meta("ba625cda-8839-466d-bb57-b69e5de1cb1a", nullptr);
        return &meta;
    }

    PageController* Page::GetController() const
    {
        return m_controller;
    }

    void Page::OnLoad()
    {
        if(m_controller != nullptr)
            m_controller->OnPageLoad();
    }

    void Page::OnShow()
    {
        if(m_controller != nullptr)
            m_controller->OnPageShow();
    }

    void Page::OnHide()
    {
        if(m_controller != nullptr)
            m_controller->OnPageHide();
    }

    void Page::OnUnload()
    {
        if(m_controller != nullptr)
            m_controller->OnPageUnload();
    }
}