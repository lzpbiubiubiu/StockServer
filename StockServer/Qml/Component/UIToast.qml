import QtQuick 2.12
import QtQuick.Layouts 1.12
import Other 1.0

// 浮动消息提示控件
Rectangle {
    id: root
    property alias text: content.text
    property alias duration: timer.interval

    implicitWidth: content.implicitWidth > parent.width * 0.7 ? parent.width * 0.7 : content.implicitWidth
    implicitHeight: content.implicitHeight
    color: "#CC000000"
    radius: 4
    visible: false
    anchors.centerIn: parent

    Text {
        id: content
        anchors.fill: parent
        color: "white"
        leftPadding: 30
        rightPadding: 30
        topPadding: 35
        bottomPadding: 35
        verticalAlignment: Qt.AlignVCenter
        wrapMode: Text.WordWrap
        font.family: UIConfig.fontFamily
        font.pixelSize: 20
    }

    PropertyAnimation
    {
        id: anim
        target: root
        duration: 300
        property: "opacity"
        from: 1.0
        to: 0.0
        easing.type: Easing.Linear
        onStopped: root.destroy()
    }

    Timer {
        id: timer
        onTriggered: anim.running = true
    }

    /** 显示UI */
    function show() {
        root.visible = true
        timer.running = true
    }

    /** 重置自动析构定时器 */
    function reset() {
        timer.restart()
    }
}
