import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Component 1.0
import Other 1.0

/** 商品面板 */
Rectangle {

    /** 列表数据模型 */
    property alias model: listView.model

    /** 列表数据行数 */
    property alias count: listView.count

    /** 列表当前索引 */
    property alias currentIndex: listView.currentIndex


    /** 表头数据 */
    readonly property var __headers: [
        { "name": "序号", "preferredWidth": 10, "minimumWidth": 10, "maximumWidth": 50 },
        { "name": "商品名称", "preferredWidth": 32, "minimumWidth": 25, "maximumWidth": 1000 },
        { "name": "零售价", "preferredWidth": 25, "minimumWidth": 18, "maximumWidth": 150 },
        { "name": "批发价", "preferredWidth": 48, "minimumWidth": 35, "maximumWidth": 300 },
        { "name": "商品库存", "preferredWidth": 30, "minimumWidth": 32, "maximumWidth": 150 }
    ]

    /** 行点击 */
    signal signalItemClicked(int index)


    id: root
    color: "#F6F8FB"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 列表头
        Rectangle {
            id: listHeader
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "white"

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Repeater {
                    model: __headers

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.minimumWidth: modelData.minimumWidth
                        Layout.preferredWidth: modelData.preferredWidth
                        Layout.maximumWidth: modelData.maximumWidth
                        Layout.fillHeight: true
                        color: index === 1 ? "transparent" : "white"

                        // 列名
                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            color: "#666666"
                            font.family: UIConfig.fontFamily
                            font.pixelSize: 14
                            font.weight: Font.Normal
                            horizontalAlignment: index === 1 ? Text.AlignLeft:Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            wrapMode: index === 0 ? Text.NoWrap : Text.WordWrap
                            text: modelData.name
                        }
                    }
                }
            }

            Rectangle {
				anchors.bottom: parent.bottom
                width: parent.width
				height: 1
				color: "#F6F8FB"
            }
        }

        // 列表
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"

            ListView {
                id: listView
                anchors.fill: parent
                boundsBehavior:Flickable.StopAtBounds
                maximumFlickVelocity: 2500
                spacing: 0
                cacheBuffer: 300
                clip: true
                delegate: itemDelegate

                // 背景层
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width//bkImage.implicitWidth
                    height: bkImage.implicitHeight + bkText.height + 24
                    visible: listView.count <= 0

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 24

                        Image {
                            id: bkImage
                            Layout.alignment: Qt.AlignHCenter
                            sourceSize.width: Math.max(root.width * 0.25, 200)
                            sourceSize.height: Math.max(root.width * 0.25, 200)
                            source: "qrc:/Resources/Images/ware_bk.svg"
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            ColumnLayout{
                                anchors.fill: parent
                                spacing: 8
                                Text {
                                    id: bkText
                                    text: "当前未维护商品信息"
                                    color: "#1E293B"
                                    font.family: UIConfig.fontFamily
                                    font.pixelSize: 16
                                    font.weight: Font.Bold
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }


                    }
                }

                // 滚动条
                ScrollBar.vertical: ScrollBar {
                    id: vscroll
                }

                // 自动选中第一行
                onCountChanged: {
                    if (listView.count <= 0){
                        listView.currentIndex = -1
                    }
                    else {              
                        root.select(0)
                    }
                }
            }
        }
    }

    // 列表项
    Component {
        id: itemDelegate

        UIFocusArea
        {
            //property alias mask: animLayer.opacity
            default property int index_: index
            width: parent.width
            height: 80

            ColumnLayout
            {
                width: parent.width
                height: parent.height
                spacing: 0

                Rectangle
                {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    id: listItem
                    color: index === listView.currentIndex ? "#C3DEFF" : (index % 2 === 0 ? "#FFFFFF" : "#F9FAFB")

                    Column
                    {
                        anchors.fill: parent

                        // 标准商品行
                        Loader
                        {
                            property int index_: index
                            property string name_: name
                            property string code_: code
                            property string stock_: stock
                            property string retailPrice_: retailPrice
                            property string wholesalePrice_: wholesalePrice
                            width: parent.width
                            height: 80
                            sourceComponent: standardItem
                        }
                    }
                }
            }
        }
    }

    // 商品组件
    Component {
        id: standardItem

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // 序号列
            Loader {
                property int wareIndex: index_

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: __headers[0].minimumWidth
                Layout.preferredWidth: __headers[0].preferredWidth
                Layout.maximumWidth: __headers[0].maximumWidth
                sourceComponent: wareIndexComponent
            }

            // 名称列
            Loader {
                property int wareIndex: index_
                property string wareCode: code_
                property string wareName: name_

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: __headers[1].minimumWidth
                Layout.preferredWidth: __headers[1].preferredWidth
                Layout.maximumWidth: __headers[1].maximumWidth
                sourceComponent: wareNameComponent
            }

            // 零售价格列
            Loader {
                property int wareIndex: index_
                property string wareCode: code_
                property string wareRetailPrice: retailPrice_

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: __headers[2].minimumWidth
                Layout.preferredWidth: __headers[2].preferredWidth
                Layout.maximumWidth: __headers[2].maximumWidth
                sourceComponent: wareRetailPriceComponent
            }

            // 批发价格列
            Loader {
                property int wareIndex: index_
                property string wareCode: code_
                property string wareWholesalePrice: wholesalePrice_

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: __headers[3].minimumWidth
                Layout.preferredWidth: __headers[3].preferredWidth
                Layout.maximumWidth: __headers[3].maximumWidth
                sourceComponent: wareWholesalePriceComponent
            }

            // 库存数量列
            Loader {
                property int wareIndex: index_
                property string wareStock: stock_

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: __headers[4].minimumWidth
                Layout.preferredWidth: __headers[4].preferredWidth
                Layout.maximumWidth: __headers[4].maximumWidth
                sourceComponent: wareStockComponent
            }
        }
    }

    // 商品序号组件
    Component {
        id: wareIndexComponent

        Rectangle {
            anchors.fill: parent
            color: "transparent"

            // 序号
            Text {
                id: indexText
                anchors.fill: parent
                font.family: UIConfig.fontFamily
                font.pixelSize: 16
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "#9CA3AF"
                text:wareIndex + 1
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.select(wareIndex)
            }
        }
    }

    // 商品名称组件
    Component {
        id: wareNameComponent

        Rectangle {
            anchors.fill: parent
            color: "transparent"


            ColumnLayout {
                anchors.fill: parent
                spacing: 2

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 2
                }

                // 名称
                Text {
                    Layout.leftMargin: 8
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 16
                    font.weight: Font.Bold
                    maximumLineCount: 2
                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    clip: true
                    color: "#191919"
                    text: wareName
                }

                // 条码
                Text {
                    id: codeText
                    Layout.leftMargin: 8
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight
                    font.family: UIConfig.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.Normal
                    elide: Text.ElideRight
                    color: "#9CA3AF"
                    text: wareCode
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.bottomMargin: 2
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.select(wareIndex)
            }
        }
    }

    // 商品零售价格组件
    Component {
        id: wareRetailPriceComponent

        Rectangle {
            anchors.fill: parent
            color: "transparent"

            // 序号
            Text {
                id: wareRetailPriceText
                anchors.fill: parent
                font.family: UIConfig.fontFamily
                font.pixelSize: 16
                font.weight: Font.Bold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "black"
                text:wareRetailPrice
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.select(wareIndex)
            }
        }
    }

    // 商品批发价格组件
    Component {
        id: wareWholesalePriceComponent

        Rectangle {
            anchors.fill: parent
            color: "transparent"

            // 序号
            Text {
                id: wareWholesalePriceText
                anchors.fill: parent
                font.family: UIConfig.fontFamily
                font.pixelSize: 16
                font.weight: Font.Bold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "black"
                text:wareWholesalePrice
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.select(wareIndex)
            }
        }
    }

    // 商品库存组件
    Component {
        id: wareStockComponent

        Rectangle {
            anchors.fill: parent
            color: "transparent"

            // 库存
            Text {
                anchors.fill: parent
                font.family: UIConfig.fontFamily
                font.pixelSize: 16
                font.weight: Font.Bold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: wareStock
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.select(wareIndex)
            }
        }
    }

    /** 选中列表项 */
    function select(index) {
        listView.currentIndex = index
        signalItemClicked(index)
    }
}
