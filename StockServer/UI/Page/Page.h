// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include <QQuickItem>
#include <QHash>
#include <QList>
#include <QVariantMap>

namespace UI
{
    /** Page基类实现 */
    struct PageMeta;
    class PageController;
    class Page : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString name MEMBER m_name CONSTANT)
        Q_PROPERTY(QString qmlFile MEMBER m_qmlFile CONSTANT)
        Q_PROPERTY(QVariant userData MEMBER m_userData CONSTANT)
        Q_PROPERTY(QQuickItem* item MEMBER m_item CONSTANT)
        Q_PROPERTY(PageController* controller MEMBER m_controller CONSTANT)
    public:
        friend class MainWindow;
        Q_INVOKABLE Page(QObject* parent = nullptr);

        virtual ~Page();

        /** 设置页面名称 */
        void SetName(const QString& name);

        /** 设置用户数据 */
        void SetUserData(const QVariant& data);

        /** 设置页面QML上下文参数 */
        void SetContextProperty(const QString& name, const QVariant& data);

        /** 设置页面QML上下文参数 */
        void SetContextProperty(const QString& name, QObject* data);

        /** 设置内容控件 */
        void SetContentItem(const QString& qmlFile);

        /** 设置是否自动销毁当页面切换隐藏时 */
        void SetAutoUnload(bool enabled);

        /** 设置页面控制器 */
        void SetController(PageController* controller);

        /** 通过对象类名查找子控件 */
        QQuickItem* FindItemByName(const QString& objectName) const;

        /** 获取页面名称 */
        const QString& GetName() const;

        /** 获取用户数据 */
        const QVariant& GetUserData() const;

        /** 获取QML上下文 */
        QQmlContext* GetContext() const;

        /** 获取内容控件 */
        QQuickItem* GetContentItem() const;

        /** 是否开启页面自动销毁 */
        bool IsAutoUnload() const;

        /** 页面是否显示 */
        bool IsVisible() const;

        /** 获取页面元信息 */
        static PageMeta* GetMeta();

        /** 获取页面控制器 */
        PageController* GetController() const;

    protected:
        /** 页面加载时回调 */
        void OnLoad();

        /** 页面显示前回调 */
        void OnShow();

        /** 页面隐藏前回调 */
        void OnHide();

        /** 页面销毁前回调 */
        void OnUnload();

    protected:
        // QML上下文
        QQmlContext* m_context = nullptr;

        // 页面名称
        QString m_name;

        // 页面QML文件
        QString m_qmlFile;

        // 用户数据
        QVariant m_userData;

        // 内容控件
        QQuickItem* m_item = nullptr;

        // 退出时销毁标记
        bool m_enableAutoUnload = false;

        // 页面控制器
        PageController* m_controller = nullptr;
    };

    /** 页面元信息，提供动态创建功能 */
    struct PageMeta
    {
        QString name;
        const QMetaObject* meta = nullptr;
        QHash<QString, PageMeta*> metaTable;

        PageMeta(const QString& name, const QMetaObject* meta)
        {
            this->name = name;
            this->meta = meta;
            if(meta)
                Page::GetMeta()->metaTable[name] = this;
        }

        const QMetaObject* FindClass(const QString& name) const
        {
            if(!Page::GetMeta()->metaTable.contains(name))
                return nullptr;
            return Page::GetMeta()->metaTable[name]->meta;
        }
    };
}

/** 声明UI页绑定 */
#define DECLARE_PAGE()          \
public:                         \
    static UI::PageMeta s_meta; \
    static UI::PageMeta* GetMeta();

/** 实现UI页绑定 */
#define IMPLEMENT_PAGE(N, T)                         \
    UI::PageMeta T::s_meta(N, &T::staticMetaObject); \
    UI::PageMeta* T::GetMeta()                       \
    {                                                \
        return &s_meta;                              \
    }
