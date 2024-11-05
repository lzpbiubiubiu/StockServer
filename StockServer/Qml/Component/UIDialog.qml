import QtQuick 2.12
import QtQuick.Window 2.12

// 通用对话框控件
Rectangle {
    id: root

    /** 导入的Component */
    property Component contentItem: Rectangle{ width: 360; height: 200; }

    /** 用户自定义数据 */
    property var userData

    /** 关闭后给焦点控件 */
    property Item closeFocusItem

    /** 关闭时是否摧毁控件 */
    property bool destoryOnClose: true

    property bool closeOnEscPressed: false

    /** 是否显示蒙层 */
    property bool showMask: true

    /** 抽屉位置 0完全关闭 1完全打开 0-1 抽屉动画中 */
    property real position: 0

    /** Dialog完全展示信号 */
    signal signalShowed()

    /** Dialog展示信号 */
    signal signalShow()

    /** Dialog完全关闭信号 */
    signal signalClosed()

    /** Dialog关闭信号 */
    signal signalClose()

    /** 关闭标识 */
    property bool closed: false

    /** 显示弹窗 */
    function show() {
        if(position < 1 && animation.to === 0) {
            closed = false
            loader.enabled = true
            mainWindow.addPageMaskRefCount()
            animation.stop()
            console.log("UIDialog Window.activeFocusItem", Window.activeFocusItem)
            closeFocusItem = Window.activeFocusItem
            root.forceActiveFocus()
            root.visible = true
            mouseArea.visible = true
            animation.from = 0.01
            animation.to = 1

            if(showMask) {
                animation.start()
                root.signalShow()
            } else {
                position = 1
                root.color = Qt.rgba(0,0,0,0)
                root.signalShow()
                root.signalShowed()
            }
        }
    }

    /** 关闭弹窗 */
    function close() {
        if(position > 0 && animation.to === 1) {
            closed = true
            loader.enabled = false
            mainWindow.releasePageMaskRefCount()
            animation.stop()
            console.log("UIDialog closeFocusItem", closeFocusItem)
            if(closeFocusItem)
                closeFocusItem.forceActiveFocus()
            animation.from = 0.99
            animation.to = 0

            if(showMask) {
                animation.start()
                root.visible = false
                root.signalClose()
            } else {
                position = 0
                root.visible = false
                root.signalClose()
                root.signalClosed()
                if(destoryOnClose) {
                    console.log("===============> UIDialog destroyed")
                    root.destroy()
                }
            }
        }
    }

    color: Qt.rgba(0,0,0,0.4 * position)
    visible: false

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        // 屏蔽空白区域鼠标事件
        hoverEnabled: true

        //屏蔽下层按钮的移入效果
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

    // esc退出
    Keys.onEscapePressed:
    {
        if(closeOnEscPressed)
            close()
    }

    Loader {
        id: loader
        anchors.centerIn: parent
        sourceComponent: contentItem
    }

    // 弹窗动画
    NumberAnimation {
        id: animation
        target: root
        property: "position"
        easing.type: Easing.InOutQuad
        duration: 300

        onStopped: {
            if(position === 0) {
                mouseArea.visible = false
                root.visible = false
                root.signalClosed()
                if(destoryOnClose) {
                    console.log("===============> UIDialog destroyed")
                    root.destroy()
                }
            } else if(position === 1) {
                root.signalShowed()
            }
        }
    }
}
