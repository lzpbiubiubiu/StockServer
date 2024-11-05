import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.4
import Other 1.0

// 按钮控件
UIFocusArea {
    id: root

    // 按钮样式
    enum Style {
        /** 默认按钮样式 */
        STYLE_DEFAULT = 0,

        /** 按钮样式1 */
        STYLE_ONE = 1,

        /** 错误按钮样式 */
        STYLE_ERROR = 2,

        /** 默认选择按钮样式 */
        STYLE_SELECT = 3
    }
    property int style: UIButton.Style.STYLE_DEFAULT

    // 背景渐变方向属性
    property int orientation: Gradient.Horizontal

    // 按钮背景色
    property color color: "#315EFB"

    // 按钮背景色2（用于渐变效果）
    property color color2: color

    // 按钮移入背景色
    property color hoverColor: color

    // 按钮移入背景色2（用于渐变效果）
    property color hoverColor2: hoverColor

    // 按钮点击背景色
    property color pressColor: "#1E40AF"

    // 按钮点击背景色2（用于渐变效果）
    property color pressColor2: pressColor

    // 按钮不可点击背景色
    property color disabledColor: "#E2E5EB"

    // 按钮不可点击背景色2（用于渐变效果）
    property color disabledColor2: disabledColor

    // 文本背景色
    property color textColor: "#FFFFFF"

    // 文本移入背景色
    property color textHoverColor: textColor

    // 文本点击背景色
    property color textPressColor: textColor

    // 文本不可点击背景色
    property color textdisabledColor: "#9CA3AF"

    // 文本文案
    property string text: "Confirm"

    // 边框宽度
    property int borderWidth: 0

    // 边框背景色
    property color borderColor: "#0F62FE"

    // 边框移入背景色
    property color borderHoverColor: borderColor

    // 边框点击背景色
    property color borderPressColor: borderColor

    // 边框不可点击背景色
    property color borderDisabledColor: "#5C94FE"

    // 圆角弧度
    property int radius: 2

    // 焦点策略
    property alias focusPolicy: focusItem.focusPolicy

    // 字体大小
    property int fontSize: 16

    // 字重
    property int fontWeight: Font.Bold

    // 按钮按下时字重
    property int pressedFontWeight: Font.Bold

    // 是否单选
    property bool selected: false

    // 文案最多显示行数
    property int maxLineCount: 2

    // 焦点Item
    property alias focusItem: focusItem

    // 点击时间
    property var clickTime: 0

    width: focusItem.width
    height: focusItem.height

    areaRadius: radius

    signal clicked()

    onStyleChanged: {
        switch(style) {
        case UIButton.Style.STYLE_DEFAULT:
        {
            color = "#315EFB"
            hoverColor = color
            pressColor = "#1E40AF"
            disabledColor = "#E2E5EB"
            textColor = "#FFFFFF"
            textHoverColor = textColor
            textPressColor = textColor
            textdisabledColor = "#9CA3AF"
            break
        }
        case UIButton.Style.STYLE_ONE:
        {
            color = "#EFF6FF"
            hoverColor = color
            pressColor = "#BFDBFE"
            disabledColor = "#E2E5EB"
            textColor = "#315EFB"
            textHoverColor = textColor
            textPressColor = textColor
            textdisabledColor = "#9CA3AF"
            break
        }
        case UIButton.Style.STYLE_ERROR:
        {
            color = "#FEF2F2"
            hoverColor = color
            pressColor = "#FECACA"
            disabledColor = "#E2E5EB"
            textColor = "#DC2626"
            textHoverColor = textColor
            textPressColor = textColor
            textdisabledColor = "#9CA3AF"
            break
        }
        case UIButton.Style.STYLE_SELECT:
        {
            color = "#EFF6FF"
            hoverColor = color
            pressColor = "#1E40AF"
            disabledColor = "#E2E5EB"
            textColor = "#0F172A"
            textHoverColor = textColor
            textPressColor = "#315EFB"
            textdisabledColor = "#9CA3AF"
            borderColor = "#DBEAFE"
            borderHoverColor = borderColor
            borderPressColor = "#BFDBFE"
            borderDisabledColor = "transparent"
            borderWidth = 1
            fontSize = 12
            break
        }
        default:
        {
            color = "#315EFB"
            hoverColor = color
            pressColor = "#1E40AF"
            disabledColor = "#E2E5EB"
            textColor = "#FFFFFF"
            textHoverColor = textColor
            textPressColor = textColor
            textdisabledColor = "#9CA3AF"
            break
        }
        }
    }

    Button {
        id: focusItem
        enabled: root.enabled
        focus: true

        contentItem: Text {
            anchors.fill: parent
            text: root.text
            elide: Text.ElideRight
            leftPadding: 4
            rightPadding: 4
            font.pixelSize: root.fontSize
            lineHeightMode: Text.ProportionalHeight
            lineHeight: 0.7
            font.family: UIConfig.fontFamily
            font.weight: {
                if(focusItem.pressed || root.selected)
                {
                     return root.pressedFontWeight
                }
                return root.fontWeight
            }
            color: {
                if (!focusItem.enabled) {
                    return root.textdisabledColor
                } else if (focusItem.pressed || root.selected) {
                    return root.textPressColor
                } else {
                    return root.textColor
                }
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            maximumLineCount: root.maxLineCount
        }

        background: Rectangle {
            border.width: root.borderWidth
            border.color: {
                if (!focusItem.enabled) {
                    return root.borderDisabledColor
                } else if (focusItem.pressed || root.selected) {
                    return root.borderPressColor
                } else {
                    return root.borderColor
                }
            }
            radius: root.radius
            implicitHeight: 35
            gradient: Gradient {
                orientation: root.orientation    //该属性设置渐变的方向
                GradientStop {
                    position: 0.0
                    color: {
                        if (!focusItem.enabled) {
                            return root.disabledColor
                        } else if (focusItem.pressed || root.selected) {
                            return root.pressColor
                        } else {
                            return root.color
                        }
                    }
                }
                GradientStop {
                    position: 1.0
                    color: {
                        if (!focusItem.enabled) {
                            return root.disabledColor2
                        } else if (focusItem.pressed || root.selected) {
                            return root.pressColor2
                        } else {
                            return root.color2
                        }
                    }
                }
            }
        }

        Keys.forwardTo: [root]
        Keys.onEnterPressed: click()
        Keys.onReturnPressed: click()
        onClicked: click()
		Keys.onSpacePressed: {}
        function click(){
          if(Date.now()- clickTime < 300 )
          {
              console.log('--------------重复点击-------------')
              return
          }
          clickTime = Date.now()
          root.clicked()
        }
    }
}

