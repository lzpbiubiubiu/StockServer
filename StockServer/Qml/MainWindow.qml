import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import Qt.labs.platform 1.1
import Page 1.0

ApplicationWindow {
    id: root

    /** 加载动画 */
    property Item __loading: null

    /** toast队列 */
    property variant __toasts: []

    /** 当前toast */
    property variant __currentToast: null

    /** 缓存toast组件 */
    property var __toastComponent : Qt.createComponent("qrc:/Qml/Component/UIToast.qml")

    x: 0
    y: 0
    width: mainWindow.fullScreen ? Screen.width : 800
    height: mainWindow.fullScreen ? Screen.height + 1 : 600
    title: "Stock Server"
    flags: mainWindow.fullScreen ? (Qt.FramelessWindowHint | Qt.WindowSystemMenuHint
           | Qt.WindowMinimizeButtonHint | Qt.Window) : Qt.Window

    onWindowStateChanged: {
        if(windowState === Qt.WindowMinimized)
            root.hide()
    }

    StackView {
        id: stackView

        anchors.fill: parent
        focus: true
        popEnter: Transition {}
        popExit: Transition {}
        pushEnter: Transition {}
        pushExit: Transition {}
        replaceEnter: Transition {}
        replaceExit: Transition {}
    }

    SystemTrayIcon {
        visible: mainWindow.trayVisible
        icon.source: "qrc:/Resources/Images/stock_server.ico"
        tooltip: "Stock Server"

        menu: Menu {

            MenuItem {
                text: "退出"
                onTriggered: app.quit()
            }

            MenuItem {
                text: "最小化"
                onTriggered: mainWindow.tray()
            }
        }

        onActivated: {
            root.requestActivate()
            if(reason === SystemTrayIcon.DoubleClick) {
                root.show()
                root.raise()
                root.requestActivate()
            }
        }
    }

    /** 加载页面 */
    function loadPage(page) {
        stackView.replace(page.item)
    }

    /** 显示loading弹窗 */
    function showLoading(text, delayShow) {
        if(__loading !== null) {
            __loading.text = text
        } else {
            var component = Qt.createComponent("qrc:/Qml/Component/UILoading.qml")
            if (component.status === Component.Ready) {
                __loading = component.createObject(root, { "text": text, "delayShow": delayShow })
                console.log("UILoading Window.activeFocusItem", Window.activeFocusItem)
                __loading.closeFocusItem = Window.activeFocusItem
                __loading.forceActiveFocus()
            } else {
                console.error(component.errorString())
            }
        }
        mainWindow.loadingRefCount++
        console.log("==========> show() __loadingRefCount ", mainWindow.loadingRefCount)
    }

    /** 关闭loading弹窗 */
    function closeLoading() {
        if(--mainWindow.loadingRefCount < 0)
            mainWindow.loadingRefCount = 0

        if(__loading !== null && mainWindow.loadingRefCount === 0) {
            console.log("UILoading closeFocusItem", __loading.closeFocusItem)
            if(__loading.closeFocusItem)
                __loading.closeFocusItem.forceActiveFocus()
            __loading.visible = false
            __loading.destroy()
            __loading = null
        }
        console.log("==========> close() __loadingRefCount ", mainWindow.loadingRefCount)
    }

    /** 显示消息对话框 */
    function showMessageBox(json) {
        console.log("showMessageBox", json.message)

        json.width = root.width
        json.height = root.height
        var component = Qt.createComponent("qrc:/Qml/Component/UIMessageBox.qml")
        if (component.status === Component.Ready) {
            var dialog = component.createObject(root, json)
            dialog.show()
        } else {
            console.error(component.errorString())
        }
    }

    /** 显示提示框 */
    function showToast(text, duration) {
        // 如果前一个toast仍在显示且内容不一致则复用
        console.log("showToast:", text)
        if(__currentToast !== null) {
            if(__currentToast.text !== text)
            {
                __currentToast.text = text
                __currentToast.duration = duration
                __currentToast.reset()
            }
            return
        }

        if (__toastComponent.status === Component.Ready) {
            __currentToast = __toastComponent.createObject(root, { "text": text, "duration": duration })
            __currentToast.Component.onDestruction.connect(function() {
                __currentToast = null
            })
            __currentToast.show()
        }
    }
}
