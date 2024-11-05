import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
import Other 1.0

Rectangle {
    id: root

    property real cellWidth: root.width/5

    property var posModel

    property var pvtModel

    TabView {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16

        Tab {
            title: "POS"
            sourceComponent: terminalCom

            property var tabModel: root.posModel
        }

        Tab {
            title: "PVT"
            sourceComponent: terminalCom

            property var tabModel: root.pvtModel
        }

        style: TabViewStyle {
            tabBar: Rectangle {
                implicitWidth: root.width
                implicitHeight: 48
            }

            tab: Rectangle {
                implicitWidth: 54
                implicitHeight: 48

                Rectangle{
                    anchors.centerIn: parent
                    implicitWidth: 54
                    implicitHeight: 32
                    color: "#F6F8FB"
                    radius: 4

                    Rectangle {
                        anchors.centerIn: parent
                        implicitWidth: 50
                        implicitHeight: 28
                        radius: 4
                        color: styleData.selected?"#FFFFFF":"transparent"

                        Text {
                            anchors.centerIn: parent
                            color: styleData.selected?"#315EFB":"#1E293B"
                            font.weight: Font.Normal
                            font.pixelSize: 14
                            font.family: UIConfig.fontFamily
                            text: styleData.title
                        }
                    }
                }
            }

            tabsAlignment: Qt.AlignHCenter
        }
    }

    Component {
        id: terminalCom

        Rectangle {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.bottomMargin: 16
                spacing: 0

                // 列表头
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#F6F8FB"

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: "ID"
                                color: "#374151"
                                font.weight: Font.Medium
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }

                            Rectangle {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: 1
                                height: 16
                                color: "#E2E5EB"
                            }
                        }

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: "Terminal No."
                                color: "#374151"
                                font.weight: Font.Medium
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }

                            Rectangle {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: 1
                                height: 16
                                color: "#E2E5EB"
                            }
                        }

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: "Device Code"
                                color: "#374151"
                                font.weight: Font.Medium
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }

                            Rectangle {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: 1
                                height: 16
                                color: "#E2E5EB"
                            }
                        }
                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: "Unsync Task"
                                color: "#374151"
                                font.weight: Font.Medium
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }

                            Rectangle {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                width: 1
                                height: 16
                                color: "#E2E5EB"
                            }
                        }
                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: "Status"
                                color: "#374151"
                                font.weight: Font.Medium
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }
                        }
                    }

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: 1
                        color: "#E2E5EB"
                    }
                }

                // 列表
                ListView{
                    id: listview

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: tabModel
                    delegate: listDelegate
                    clip: true
                }
            }

            Component {
                id: listDelegate

                Rectangle {
                    width: listview.width
                    height: 32
                    color: index%2===0?"#FFFFFF":"#F6F8FB"

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: id
                                color: "#374151"
                                font.weight: Font.Normal
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }
                        }

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: deskNo
                                color: "#374151"
                                font.weight: Font.Normal
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }
                        }

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: deviceCode
                                color: "#374151"
                                font.weight: Font.Normal
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }
                        }

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                text: unsynctask.length==0?"0/0":unsynctask
                                color: unsynctask.length==0?"#374151":"#FF6666"
                                font.weight: Font.Normal
                                font.pixelSize: 14
                                font.family: UIConfig.fontFamily
                            }
                        }

                        Item {
                            Layout.preferredWidth: cellWidth
                            Layout.fillHeight: true

                            Image {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 8
                                source: connected ? "qrc:/Resources/Images/connected.png" : "qrc:/Resources/Images/disconnected.png"
                            }
                        }
                    }
                }
            }
        }
    }
}
