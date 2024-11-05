import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.4
import Other 1.0

// 通用文本按钮
UIFocusArea {

    id: root

    /** 图片宽度 */
    property alias imageWidth: icon.sourceSize.width

    /** 图片高度 */
    property alias imageHeight: icon.sourceSize.height

    /** 图片资源 */
    property alias source: icon.source

    /** 文字 */
    property alias text: label.text

    /** 字体大小 */
    property alias fontSize: label.font.pixelSize

    /** 字重 */
    property alias fontWeight: label.font.weight

    /** 字体颜色 */
    property alias textColor: label.color

    /** 字体垂直布局 */
    property alias textHorizontalAlignment: label.horizontalAlignment

    /** 图片与文字间距*/
    property int spacing: 8

    /** 内间距 (仅用于左右布局)*/
    property int leftMargin: 0

    /** 内间距 (仅用于左右布局)*/
    property int rightMargin: 0

    /** 图片位置 (left,top,right,bottom) */
    property string position: "left"

    /** 背景色 */
    property alias color: rectangle.color

    /** 边框 */
    property alias border: rectangle.border

    /** 圆角 */
    property alias radius: rectangle.radius

    property var clickTime: 0

    /** 点击按钮 */
    signal signalClicked()

    Rectangle {
        id: rectangle
        width: parent.width
        height: parent.height
        color: "transparent"
        clip: true
        focus: true

        // 图标
        Image {
            id: icon
            clip: true
            cache: false
        }

        // 文字
        Text {
           id: label
           width: {
               if(position === "left" || position === "right"){
                   parent.width - 16 - icon.width - leftMargin - rightMargin
               }else{
                   parent.width - 16
               }
           }

           height: implicitHeight
           font.family: UIConfig.fontFamily
           font.weight: Font.Normal
           font.pixelSize: 14
           color: "#000000"
           verticalAlignment: Text.AlignVCenter
           horizontalAlignment: Text.AlignHCenter
           wrapMode: Text.WordWrap
           maximumLineCount: 2
           elide: Text.ElideRight
        }

        Component.onCompleted: {
            var xMargin = (rectangle.width - (icon.width + spacing + label.width)) / 2.0
            var yMargin = (rectangle.height - (icon.height + spacing + label.height)) / 2.0

            //图片在左边
            if(root.position === "left") {

                icon.anchors.left = rectangle.left
                icon.anchors.leftMargin = leftMargin > 0 ? leftMargin : xMargin
                icon.anchors.verticalCenter = rectangle.verticalCenter
                label.anchors.left = icon.right
                label.anchors.leftMargin = spacing
                label.anchors.verticalCenter = rectangle.verticalCenter

                root.implicitWidth = icon.width + label.width + spacing
                root.implicitHeight = Math.max(icon.height, label.height)
            }
            //图片在右边
            else if (root.position === "right") {
                label.anchors.left = rectangle.left
                label.anchors.leftMargin = rightMargin > 0 ? rightMargin : xMargin
                label.anchors.verticalCenter = rectangle.verticalCenter
                icon.anchors.left = label.right
                icon.anchors.leftMargin = spacing
                icon.anchors.verticalCenter = rectangle.verticalCenter

                root.implicitWidth = icon.width + label.width + spacing
                root.implicitHeight = Math.max(icon.height, label.height)
            }
            // 图片在上边
            else if(root.position === "top") {
                icon.anchors.top = rectangle.top
                icon.anchors.topMargin = yMargin
                icon.anchors.horizontalCenter = rectangle.horizontalCenter
                label.anchors.top = icon.bottom
                label.anchors.topMargin = spacing
                label.anchors.horizontalCenter = rectangle.horizontalCenter

                root.implicitWidth = Math.max(icon.width, label.width)
                root.implicitHeight = icon.height + label.height + spacing
            }
            // 图片在下边
            else if(root.position === "bottom") {
                label.anchors.top = rectangle.top
                label.anchors.topMargin = yMargin
                label.anchors.horizontalCenter = rectangle.horizontalCenter
                icon.anchors.top = label.bottom
                icon.anchors.topMargin = spacing
                icon.anchors.horizontalCenter = rectangle.horizontalCenter

                root.implicitWidth = Math.max(icon.width, label.width)
                root.implicitHeight = icon.height + label.height + spacing
            }
        }

        function click(){
          if(Date.now()-clickTime < 300 )
          {
              console.log('--------------重复点击-------------')
              return
          }
          clickTime = Date.now()
          root.signalClicked()
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rectangle.click()
        }

        Keys.onEnterPressed: click()
        Keys.onReturnPressed: click()

    }
}
