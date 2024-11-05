import QtQuick 2.12
import QtQuick.Layouts 1.12
import Component 1.0
import Other 1.0

// 状态栏
Rectangle {
    id: root

    implicitWidth: 1024
    implicitHeight: 24
    color: "#1E3A8A"

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 16

        Text {
            id: storeName

            Layout.alignment: Qt.AlignVCenter
            color: "#FFFFFF"
            font.weight: Font.Normal
            font.pixelSize: 11
            font.family: UIConfig.fontFamily
            elide: Text.ElideRight
            text: mainWindow.statusBar.storeName
        }

        Text {
            id: deviceId

            Layout.alignment: Qt.AlignVCenter
            color: "#FFFFFF"
            font.weight: Font.Normal
            font.pixelSize: 11
            font.family: UIConfig.fontFamily
            text: mainWindow.statusBar.deviceId
        }

        Text {
            id: version

            Layout.alignment: Qt.AlignVCenter
            color: "#FFFFFF"
            font.weight: Font.Normal
            font.pixelSize: 11
            font.family: UIConfig.fontFamily
            text: "V"+ mainWindow.statusBar.version
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Image {
            id: network

            Layout.alignment: Qt.AlignVCenter
            source: mainWindow.statusBar.netState ? "qrc:/Resources/Images/network_ok.svg" :
                                         "qrc:/Resources/Images/network_error.svg"
        }

        Text {
            id: time

            Layout.alignment: Qt.AlignVCenter
            color: "#FFFFFF"
            font.weight: Font.Normal
            font.pixelSize: 11
            font.family: UIConfig.fontFamily
            text: mainWindow.statusBar.time
        }

        Rectangle{
            Layout.preferredWidth: 1
            Layout.preferredHeight: 12
            Layout.alignment: Qt.AlignVCenter
            color: "#FAFAFA"
            opacity: 0.2
        }

        UILabelButton {
            id: trayButton

            Layout.preferredWidth: 54
            Layout.preferredHeight: 16
            Layout.alignment: Qt.AlignVCenter
            source: "qrc:/Resources/Images/tray.svg"
            imageWidth: 16
            imageHeight: 16
            text: "Tray"
            fontSize: 11
            spacing: 4
            fontWeight: Font.Normal
            textColor: "#FFFFFF"

            onSignalClicked: mainWindow.tray()
        }

        UILabelButton {
            id: exitButton

            Layout.preferredWidth: 52
            Layout.preferredHeight: 16
            Layout.alignment: Qt.AlignVCenter
            source: "qrc:/Resources/Images/quit.svg"
            imageWidth: 16
            imageHeight: 16
            text: "Exit"
            fontSize: 11
            spacing: 4
            fontWeight: Font.Normal
            textColor: "#FFFFFF"

            onSignalClicked: {
                var json = {
                    "title":"Message",
                    "message":"Confirm exit ?",
                    "acceptButtonText":"Confirm",
                    "rejectButtonText":"Cancel",
                    "accept": app.quit
                }

                mainWindow.item.showMessageBox(json)
            }
        }
    }
}
