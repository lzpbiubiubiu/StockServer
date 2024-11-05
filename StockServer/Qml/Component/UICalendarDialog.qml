import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import Other 1.0

UIDialog {
    id: root

    property var accept: null
    property var reject: null
    property var date: null
    property var minDate: null
    property var maxDate: null
    property int pickMode: __dayMode
    property int contentWidth: pickMode === __dayMode ? 480 : 288
    property int contentHeight: pickMode === __dayMode ? 392 : 256

    readonly property int __dayMode: 1
    readonly property int __monthMode: 2

    contentItem: contentComponent

    Component {
        id: contentComponent

        Rectangle {
            width: root.contentWidth
            height: root.contentHeight
            radius: 4
            color: "#FFFFFF"

            Loader {
                id: pickerLoader
                x: 16
                y: 16
                width: parent.width - 32
                height: parent.height - 64
                sourceComponent: pickMode === __dayMode ? dayPicker : monthPicker
            }

            Row {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 8
                width: parent.width - 32
                height: 40
                x: 16
                spacing: 8

                UIButton {
                    width: (parent.width - 8) / 2
                    height: parent.height
                    text: "Cancel"
                    fontSize: 14
                    fontWeight: Font.Bold
                    textColor: "#315EFB"
                    color: "#EFF6FF"
                    pressColor: "#BFDBFE"
                    onClicked: {
                        root.close()
                        if(root.reject && typeof root.reject === 'function')
                            root.reject()
                    }
                }

                UIButton {
                    width: (parent.width - 8) / 2
                    height: parent.height
                    text: "Confirm"
                    fontSize: 14
                    fontWeight: Font.Bold
                    textColor: "#FFFFFF"
                    color: "#315EFB"
                    pressColor: "#1E40AF"
                    onClicked: {
                        root.close()
                        if(root.accept && typeof root.accept === 'function')
                            root.accept(pickerLoader.item.selectedDate)
                    }
                }

            }

        }
    }

    Component {
        id: dayPicker
        Calendar{
            id: calendar
            width: parent.width
            height: parent.height
            weekNumbersVisible: false
            selectedDate: root.date === null ? Date.fromLocaleDateString(Qt.locale(), (new Date()).toLocaleDateString(Qt.locale(), "yyyy-MM-dd"), "yyyy-MM-dd") : root.date
            minimumDate: root.minDate
            maximumDate: root.maxDate

            style: CalendarStyle {
                gridColor: "#FFFFFF"

                /** 导航栏风格设置 */
                navigationBar:Item {
                    height: 32
                    Rectangle{
                        anchors.fill: parent

                        Row {
                            height: parent.height
                            width: 80
                            spacing: 16
                            anchors.left: parent.left
                            Rectangle {
                                width: 32
                                height: parent.height
                                Image {
                                    anchors.fill: parent
                                    source: "qrc:/Resources/Images/pre_year.svg"
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: control.showPreviousYear()
                                    }
                                }
                            }
                            Rectangle {
                                width: 32
                                height: parent.height
                                Image {
                                    anchors.fill: parent
                                    source: "qrc:/Resources/Images/pre_month.svg"
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: control.showPreviousMonth()
                                    }
                                }
                            }
                        }

                        Text{
                            id: dateText
                            anchors.centerIn: parent
                            color: "#0F172A"
                            font.family: UIConfig.fontFamily
                            font.pixelSize: 18
                            font.weight: Font.Bold
                            text: (Date.fromLocaleDateString(Qt.locale(), control.visibleYear + "-" + __fillZero(control.visibleMonth + 1), "yyyy-MM")).toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
                        }

                        Row {
                            height: parent.height
                            width: 80
                            spacing: 16
                            anchors.right: parent.right
                            Rectangle {
                                width: 32
                                height: parent.height
                                Image {
                                    anchors.fill: parent
                                    source: "qrc:/Resources/Images/next_month.svg"
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: control.showNextMonth()
                                    }
                                }
                            }
                            Rectangle {
                                width: 32
                                height: parent.height
                                Image {
                                    width: 32
                                    height: 32
                                    source: "qrc:/Resources/Images/next_year.svg"
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: control.showNextYear()
                                    }
                                }
                            }
                        }
                    }
                }
                /** 星期标识头风格设置 */
                dayOfWeekDelegate:Item {
                    id: weekDelegate
                    height: 34
                    width: calendar.width / 7
                    Rectangle{
                        anchors.fill: parent

                        Text{
                            id: weekDay
                            anchors.centerIn: parent
                            anchors.horizontalCenter: parent.Center
                            anchors.bottom: parent.bottom
                            color: "#616E80"
                            font.family: UIConfig.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Normal
                            text: __getWeekName(styleData.index)
                        }
                    }
                }

                /** 天（day）风格设置 */
                dayDelegate: Item {
                    id: dayDelegate
                    readonly property color sameMonthDateTextColor: "#0F172A"
                    readonly property color selectedDateColor: "#315EFB"
                    readonly property color selectedDateTextColor: "#FFFFFF"
                    readonly property color differentMonthDateTextColor: "#9CA3AF"
                    readonly property color invalidDatecolor: "#9CA3AF"
                    width: calendar.width / 7
                    height: (calendar.height - 66) / 6

                    Rectangle {
                        anchors.fill: parent
                        color: selectedDateColor
                        visible: styleData.selected
                        radius: 4
                    }


                    Text {
                        id: dayDelegateText
                        text: styleData.date.getDate()
                        anchors.centerIn: parent
                        font.family: UIConfig.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.Bold
                        color:  {
                            var color = invalidDatecolor
                            if (styleData.valid) {
                                color = styleData.visibleMonth ? sameMonthDateTextColor : differentMonthDateTextColor
                                if (styleData.selected) {
                                    color = selectedDateTextColor
                                }
                            }
                            return color
                        }
                    }
                }
            }
        }
    }

    Component {
        id: monthPicker
        Rectangle {
            id: monthControl
            property var selectedDate: root.date === null ? new Date() : root.date
            width: parent.width
            height: parent.height

            Rectangle {
                id: monthPickerHeader
                width: parent.width
                height: 32

                Rectangle {
                    width: 32
                    height: parent.height
                    anchors.left: parent.left
                    Image {
                        anchors.fill: parent
                        source: "qrc:/Resources/Images/pre_year.svg"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                monthControl.selectedDate.setFullYear(monthControl.selectedDate.getFullYear() - 1)
                                yearText.text = monthControl.selectedDate.getFullYear()
                            }
                        }
                    }
                }

                Text{
                    id: yearText
                    anchors.centerIn: parent
                    color: "#0F172A"
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    text: (Date.fromLocaleDateString(Qt.locale(), monthControl.selectedDate.getFullYear(), "yyyy")).toLocaleDateString(Qt.locale(), dateFormatYear)
                }

                Rectangle {
                    width: 32
                    height: parent.height
                    anchors.right: parent.right
                    Image {
                        anchors.fill: parent
                        source: "qrc:/Resources/Images/next_year.svg"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                monthControl.selectedDate.setFullYear(monthControl.selectedDate.getFullYear() + 1)
                                yearText.text = monthControl.selectedDate.getFullYear()
                            }
                        }
                    }
                }
            }
            GridView {
                property int selectedIndex: monthControl.selectedDate.getMonth()

                id: monthGrid
                model: 12
                y: monthPickerHeader.height + 16
                width: parent.width
                height: parent.height - monthPickerHeader.height - 40
                cellHeight: height / 3
                cellWidth: width / 4
                interactive: false

                delegate: Rectangle {
                    Rectangle {
                        width: monthGrid.cellWidth
                        height: monthGrid.cellHeight

                        Rectangle {
                            anchors.fill: parent
                            color: "#315EFB"
                            radius: 4
                            visible: (index === monthGrid.selectedIndex)
                        }

                        Text {
                            text: index + 1
                            anchors.centerIn: parent
                            font.family: UIConfig.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Bold
                            color: (index === monthGrid.selectedIndex) ? "#FFFFFF" : "#0F172A"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked:{
                                monthControl.selectedDate.setMonth(index)
                                monthGrid.selectedIndex = monthControl.selectedDate.getMonth()
                            }
                        }
                    }
                }
            }
        }
    }

    function __getWeekName(index){
        switch(index)
        {
            case 0:
                return "Mon"
            case 1:
                return "Tue"
            case 2:
                return "Wed"
            case 3:
                return "Thur"
            case 4:
                return "Fri"
            case 5:
                return "Sat"
            case 6:
                return "Sun"
            default:
                return "N/A"
        }
    }

    //长度不足2 补零
    function __fillZero(value) {
        return value.toString().length < 2 ? ('0' + value) : value
    }
}
