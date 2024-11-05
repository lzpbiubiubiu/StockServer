#pragma once

#include <QJsonArray>
#include "Page.h"

namespace UI
{
    class StartupPage : public Page
    {
        Q_OBJECT
        DECLARE_PAGE()
        Q_PROPERTY(QJsonArray infos READ GetInfos NOTIFY signalInfosChanged)
        Q_PROPERTY(int buttons READ GetButtons NOTIFY signalButtonsChanged)

    public:
        enum Button
        {
            /** 没有按钮 */
            BUTTON_NONE = 0,

            /** 启动 */
            BUTTON_START = 1,

            /** 重试 */
            BUTTON_RETRY = 1 << 1,

            /** 跳过升级 */
            BUTTON_IGNORE_UPDATE = 1 << 2,

            /** 退出 */
            BUTTON_EXIT = 1 << 3,
        };
        Q_ENUM(Button)
        Q_DECLARE_FLAGS(Buttons, Button)

        Q_INVOKABLE StartupPage(QObject* parent = nullptr);

        virtual ~StartupPage();

        QJsonArray GetInfos() const;
        void AddInfo(const QString& key, const QString& value);
        void UpdateInfo(const QString& key, const QString& value);
        void ResetInfos();
        void RemoveInfo(const QString& key);

        int GetButtons() const;
        void SetButtons(Buttons buttons);

    Q_SIGNALS:
        void signalInfosChanged();
        void signalButtonsChanged();

    private:
        // 列表数据
        QJsonArray m_infos;

        // 按钮组
        Buttons m_buttons = Button::BUTTON_NONE;
    };
} // namespace UI