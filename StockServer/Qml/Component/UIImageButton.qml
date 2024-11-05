import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.4
import Other 1.0

// 图片按钮控件
UIFocusArea {
    property string imageSource: ""
    property string imageHoverSource: imageSource
    property string imagePressSource: imageSource
    property int borderWidth: 0
    property string text: ""
    property int radius: 0
    property color textColor: "white"
    property int fontSize: 16
    property int fontWeight: Font.Bold
    property string fontFamily: UIConfig.fontFamily
    property color color: "transparent"
    property color borderColor: "transparent"
    property bool hovered: button.hovered
    property bool pressed: button.pressed
    property alias focusItem: button
    property var clickTime: 0
    property alias focusPolicy: button.focusPolicy
    property int imageWidth: image.implicitWidth
    property int imageHeight: image.implicitHeight

    signal signalClicked()

    id: root
    implicitWidth: Math.max(image.width, button.implicitWidth)
    implicitHeight: Math.max(image.height, button.implicitHeight)

    FocusScope {
        focus: true

        Image {
            id: image
            anchors.centerIn: parent
            source: button.pressed ? imagePressSource : imageSource
            width: imageWidth
            height: imageHeight
        }

        Button {
            id: button
            anchors.fill: parent
            activeFocusOnTab: false
            focus: true

            contentItem: Text {
                text: root.text
                font.pixelSize: root.fontSize
                font.family: root.fontFamily
                font.weight: root.fontWeight
                color: root.textColor
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            background: Rectangle {
                radius: root.radius
                border.width: root.borderWidth
                color: root.color
                border.color: root.borderColor
            }

            Keys.forwardTo: [root]
            Keys.onEnterPressed: click()
            Keys.onReturnPressed: click()
            onClicked: click()

            function click(){
              if(Date.now()-clickTime < 100 )
              {
                  console.log('--------------重复点击-------------')
                  return
              }
              clickTime = Date.now()
              root.signalClicked()
            }
        }
    }
}
