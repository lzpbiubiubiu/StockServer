import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import Other 1.0

// 通用消息提示框
UIDialog {
    id: root

    /** 标题 */
    property string title:""

    /** 消息内容 */
    property string message: ""

    /** 消息内容字体颜色 */
    property string messageTextColor: "#616E80"

    /** 消息code */
    property string code: ""

    /** 按钮1文本 */
    property string rejectButtonText: ""

    /** 按钮2文本 */
    property string acceptButtonText: ""

    /** 确认回调 */
    property var accept: null

    /** 取消回调 */
    property var reject: null

    contentItem: messageBox

    Component {
        id: messageBox

        Rectangle {
            id: msgbk
            width: 360
            height: codeText.visible ? 174 : 158
            radius: 4
            clip: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // 错误码内容
                Text {
                    id: codeText
                    Layout.topMargin: 8
                    Layout.leftMargin: 8
                    Layout.fillWidth: true
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 12
                    font.weight: Font.Bold
                    elide: Text.ElideRight
                    color: "#616E80"
                    text: root.code
                    visible: root.code !== ""
                }

                // 标题
                Text {
                    id: titleText
                    Layout.topMargin: codeText.visible ? 14 : 24
                    Layout.leftMargin: 24
                    Layout.rightMargin: 24
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 20
                    color: "#0F172A"
                    horizontalAlignment: Text.AlignHCenter
                    text: root.title
                }

                // 消息内容
                Text {
                    id: messageText
                    Layout.topMargin: 8
                    Layout.leftMargin: 24
                    Layout.rightMargin: 24
                    Layout.fillWidth: true
                    Layout.preferredHeight: {
                        var maxheight = root.height*0.4-106-titleText.lineCount*30
                        if(codeText.visible)
                            maxheight-=16
                        console.log("===================>Layout.preferredHeight",maxheight,implicitHeight,lineCount)
                        implicitHeight>maxheight?maxheight:implicitHeight
                    }
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.Bold
                    wrapMode: Text.WordWrap
                    //elide: Text.ElideRight
                    color: root.messageTextColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: root.message
                    visible: root.message !== ""
                }

                Item {
                    Layout.fillHeight: true
                }

                // 水平分割线
                Rectangle {
                    color: "#E5E8EE"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                }

                Row {
                    id: buttonGroup
                    spacing: 0
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50

                    // 取消按钮
                    UIButton {
                        id: rejectButton
                        width: acceptButton.visible ? (parent.width / 2) : parent.width
                        height: parent.height
                        radius: 4
                        text: rejectButtonText
                        textColor: "#000000"
                        textHoverColor: "#315EFB"
                        textPressColor: "#315EFB"
                        textdisabledColor: "#9CA3AF"
                        fontWeight: Font.Medium
                        fontSize: 20
                        color: "#FFFFFF"
                        hoverColor: "#FFFFFF"
                        pressColor: "#FFFFFF"
                        disabledColor: "#E2E5EB"
                        visible: rejectButtonText !== ""

                        onClicked: {
                            console.log("rejectButton Clicked")
                            root.close()
                            if(root.reject && typeof root.reject === 'function')
                                root.reject()
                        }

                        KeyNavigation.up: acceptButton
                        KeyNavigation.down: acceptButton
                        KeyNavigation.left: acceptButton
                        KeyNavigation.right: acceptButton
                        KeyNavigation.tab: acceptButton
                    }

                    // 垂直分割线
                    Rectangle {
                        color: "#E5E8EE"
                        width: 1
                        height: parent.height
                        visible: rejectButton.visible && acceptButton.visible
                    }

                    // 确认按钮
                    UIButton {
                        id: acceptButton
                        width: rejectButton.visible ? (parent.width / 2) : parent.width
                        height: parent.height
                        radius: 4
                        text: acceptButtonText
                        textColor: "#000000"
                        textHoverColor: "#315EFB"
                        textPressColor: "#315EFB"
                        textdisabledColor: "#9CA3AF"
                        fontWeight: Font.Medium
                        fontSize: 20
                        color: "#FFFFFF"
                        hoverColor: "#FFFFFF"
                        pressColor: "#FFFFFF"
                        disabledColor: "#E2E5EB"

                        onClicked: {
                            console.log("acceptButton Clicked")
                            root.close()
                            if(root.accept && typeof root.accept === 'function')
                                root.accept()
                        }

                        KeyNavigation.up: rejectButton
                        KeyNavigation.down: rejectButton
                        KeyNavigation.left: rejectButton
                        KeyNavigation.right: rejectButton
                        KeyNavigation.tab: rejectButton
                        //Keys.onEscapePressed: root.close()
                    }
                }
            }

            Connections {
                target: root
                onSignalShow: acceptButton.forceActiveFocus()
            }


            Connections {
                target: messageText

                onLineCountChanged: {
                    var maxHeight = root.height*0.4+5
                    var impHeight = 106+messageText.lineCount*22+titleText.lineCount*30+5
                    if(codeText.visible)
                        impHeight += 16
                    console.log("messageText===================>onLineCountChanged",maxHeight,impHeight)
                    msgbk.height = maxHeight > impHeight ? impHeight : maxHeight
                }
            }

            Connections {
                target: titleText

                onLineCountChanged: {
                    var maxHeight = root.height*0.4+5
                    var impHeight = 106+messageText.lineCount*22+titleText.lineCount*30+5
                    if(codeText.visible)
                        impHeight += 16
                    console.log("titleText===================>onLineCountChanged",maxHeight,impHeight)
                    msgbk.height = maxHeight > impHeight ? impHeight : maxHeight
                }
            }

            Connections {
                target: codeText

                onVisibleChanged: {
                    var maxHeight = root.height*0.4+5
                    var impHeight = 106+messageText.lineCount*22+titleText.lineCount*30+5
                    if(codeText.visible)
                        impHeight += 16
                    console.log("codeText===================>onVisibleChanged",maxHeight,impHeight)
                    msgbk.height = maxHeight > impHeight ? impHeight : maxHeight
                }
            }

        }
    }
}
