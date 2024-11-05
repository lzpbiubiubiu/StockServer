import QtQuick 2.12
import QtQuick.Layouts 1.12
import Other 1.0

// 加载动画层控件
Rectangle {
    /** 文本提示 */
    property alias text: msg.text

    /** 是否延迟显示 */
    property bool delayShow: false

    property Item closeFocusItem: null

    id: root
    anchors.fill: parent
    color: Qt.rgba(0,0,0,0.4)
    opacity: root.delayShow ? 0 : 1.0

    Rectangle {
        anchors.centerIn: parent
        width: col.implicitWidth
        height: 132
        color: "#FFFFFF"
        radius: 8

        Column {
            id: col
            padding: 16
            anchors.centerIn: parent
            spacing: 10

            // 图标
            AnimatedImage {
                id: icon
                width: 80
                height: 80
                source: "qrc:/Resources/Images/loading.gif"
                smooth: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // 提示
            Text {
                id: msg
                visible: text !== ""
                width: implicitWidth
                height: implicitHeight
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: UIConfig.fontFamily
                font.pixelSize: 16
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "#191919"
            }
        }
    }


    // 延迟显示定时器
    Timer {
        id: delayTimer
        interval: 500
        repeat: false
        triggeredOnStart: false
        onTriggered: root.opacity = 1.0
    }


    MouseArea {
        anchors.fill: parent
        hoverEnabled: true //屏蔽下层按钮的移入效果
        onClicked: {}
        onPressed: {}
        onReleased: {}
        onWheel: {}
        onPressAndHold: {}
    }

    // 屏蔽键盘事件
    Keys.onPressed: {
        event.accepted = true
    }

    Component.onCompleted: {
        root.opacity = root.delayShow ? 0.0 : 1.0
        delayTimer.running = root.delayShow
    }
}
