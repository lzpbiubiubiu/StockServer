import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import Component 1.0
import Other 1.0

Page {
    id: root

    // 按钮组
    enum Buttons {
        // 没有按钮
        BUTTON_NONE = 0,

        // 启动按钮
        BUTTON_START = 1,

        // 重试按钮
        BUTTON_RETRY = 2,

        // 跳过升级按钮
        BUTTON_IGNORE_UPDATE = 4,

        // 退出按钮
        BUTTON_EXIT = 8
    }

    Image {
        anchors.fill: parent
        source: "qrc:/Resources/Images/launcher_bg.png"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 38
        anchors.topMargin: 48
        anchors.bottomMargin: 40
        anchors.rightMargin: 40
        spacing: 48

        Image {
            source: "qrc:/Resources/Images/logo.svg"
        }

        ListView {
            id: listView

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20
            model: page.infos
            delegate: listDelegate
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            visible: (page.buttons & (StartupPage.Buttons.BUTTON_START|StartupPage.Buttons.BUTTON_RETRY|StartupPage.Buttons.BUTTON_INIT)) != 0

            RowLayout {
                anchors.fill: parent
                spacing: 16

                // 占位符
                Item {
                    Layout.fillWidth: true
                }

                UIButton {
                    id: quitButton
                    Layout.preferredWidth: 144
                    Layout.fillHeight: true
                    text: "Exit"
                    fontSize: 20
                    visible: (page.buttons & (StartupPage.Buttons.BUTTON_START|StartupPage.Buttons.BUTTON_RETRY|StartupPage.Buttons.BUTTON_INIT)) != 0
                    enabled: quitButton.visible

                    onClicked: page.controller.quit()
                }

                UIButton {
                    id: retryButton
                    Layout.preferredWidth: 144
                    Layout.fillHeight: true
                    style: UIButton.Style.STYLE_ONE
                    text: "Retry"
                    fontSize: 20
                    visible: (page.buttons & StartupPage.Buttons.BUTTON_RETRY) != 0
                    enabled: retryButton.visible

                    onClicked: page.controller.retry()
                }

                UIButton {
                    id: ignoreButton
                    Layout.preferredWidth: 144
                    Layout.fillHeight: true
                    style: UIButton.Style.STYLE_ONE
                    text: "Skip Upgrade"
                    fontSize: 20
                    visible: (page.buttons & StartupPage.Buttons.BUTTON_IGNORE_UPDATE) != 0
                    enabled: ignoreButton.visible

                    onClicked: page.controller.ignoreUpdate()
                }

                UIButton {
                    id: startButton
                    Layout.preferredWidth: 144
                    Layout.fillHeight: true
                    style: UIButton.Style.STYLE_ONE
                    text: "Start"
                    fontSize: 20
                    visible: (page.buttons & StartupPage.Buttons.BUTTON_START) != 0
                    enabled: startButton.visible

                    onClicked: page.controller.start()
                }
            }
        }
    }

    Component {
        id: listDelegate

        Item {
            width: listView.width
            height: 24

            RowLayout {
                anchors.fill: parent
                spacing: 60

                Text {
                    Layout.preferredWidth: 204
                    Layout.fillHeight: true
                    text: modelData.key
                    font.weight: Font.Normal
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignTop
                    color: "#CED3DB"
                }

                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: modelData.value
                    font.weight: Font.Bold
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                    color: "#FFFFFF"
                }
            }
        }
    }
}
