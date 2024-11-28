import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import Other 1.0
import Component 1.0

Page {
    id: root

    Image {
        anchors.fill: parent
        source: "qrc:/Resources/Images/launcher_bg.png"
    }
    
    Rectangle {
        id: main
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // 状态栏
            StatusBar{
                Layout.fillWidth: true
            }
            
			// 主体   
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                RowLayout {
                    anchors.fill: parent
                    spacing: 1

                    StockWarePanel {
                        id: left
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: page.stockWarePanel.model
                        onSignalItemClicked: page.controller.itemClicked(index)
                    }

                    Rectangle {
                        id: right
                        Layout.preferredWidth: parent.width * 0.3
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.topMargin: 16
                            anchors.bottomMargin: 8
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            spacing: 8

                            Rectangle {
                                Layout.preferredHeight: parent.height * 0.3
                                Layout.fillWidth: true

                                Image {
                                    width: parent.width - 50
                                    height: parent.height - 50
                                    anchors.centerIn: parent
                                    source: page.currentWareInfo.imgPath

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            var jsonFile = {
                                                "nameFilters": ["Image files (*.svg *.jpg *.png *.jpeg)"],
                                                "fileMode": FileDialog.OpenFile,
                                                "accept": function(path){
                                                    page.controller.uploadWareImage(page.currentWareInfo.index, path)
                                                }
                                            }
                                            showFileDialog(root,jsonFile)
                                        }
                                    }
                                }

                            }

                            Text {
                                Layout.fillWidth: true
                                text: page.currentWareInfo.name
                                font.family: UIConfig.fontFamily
                                font.pixelSize: 18
                                font.weight: Font.Bold
                                color: "#0F172A"
                                verticalAlignment: Text.AlignVCenter
                                maximumLineCount: 2
                                elide: Text.ElideRight
                                wrapMode: Text.WrapAnywhere
                            }

                            // 码
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 18
                                // ID
                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 4

                                    //条码标记
                                    Rectangle {
                                        implicitWidth: 18
                                        implicitHeight: 18
                                        color: "#E6F9F0"
                                        radius: 2

                                        Text {
                                            anchors.centerIn: parent
                                            text: "#"
                                            font.pixelSize: 14
                                            font.family: UIConfig.fontFamily
                                            font.weight: Font.Normal
                                            color: "#0FC269"
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                    }

                                    Text {
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                        Layout.alignment: Qt.AlignVCenter
                                        font.family: UIConfig.fontFamily
                                        font.pixelSize: 12
                                        font.weight: Font.Bold
                                        maximumLineCount: 1
                                        elide: Text.ElideRight
                                        color: "#616E80"
                                        text: page.currentWareInfo.code
                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter
                                        clip: true
                                    }
                                }
                            }

                            // 价格
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 44

                                RowLayout {
                                    anchors.fill:parent
                                    spacing: 8

                                    // 零售价
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.fillHeight:true
                                        color: "#FFF7ED"
                                        radius: 2

                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 0

                                            Text {
                                                id: idRetailPriceTitle

                                                Layout.topMargin: 2
                                                Layout.leftMargin: 8

                                                text: "(￥)零售价"
                                                font.pixelSize: 12
                                                font.family: UIConfig.fontFamily
                                                font.weight: Font.Normal
                                                color: "#616E80"
                                                horizontalAlignment: Text.AlignLeft
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            Text {
                                                id: idRetailPrice
                                                Layout.leftMargin: 8
                                                Layout.bottomMargin: 2

                                                text: page.currentWareInfo.retailPrice
                                                font.pixelSize: 14
                                                font.family: UIConfig.fontFamily
                                                font.weight: Font.Bold
                                                color: "#0F172A"
                                                horizontalAlignment: Text.AlignLeft
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                        }
                                    }

                                    // 批发价
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.fillHeight:true
                                        color: "#F6F8FB"
                                        radius: 2

                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 0

                                            Text {
                                                id: idWholesalePriceTitle

                                                Layout.topMargin: 2
                                                Layout.leftMargin: 8

                                                text: "(￥)批发价"
                                                font.pixelSize: 12
                                                font.family: UIConfig.fontFamily
                                                font.weight: Font.Normal
                                                color: "#616E80"
                                                horizontalAlignment: Text.AlignLeft
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            Text {
                                                id: idWholesalePrice

                                                Layout.leftMargin: 8
                                                Layout.bottomMargin: 2

                                                text: page.currentWareInfo.wholesalePrice
                                                font.pixelSize: 14
                                                font.family: UIConfig.fontFamily
                                                font.weight: Font.Bold
                                                color: "#0F172A"
                                                horizontalAlignment: Text.AlignLeft
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                        }
                                    }

                                    // 商品库存
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.fillHeight:true
                                        color: "#E6F9F0"
                                        radius: 2

                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 0

                                            Text {
                                                id: idStockTitle

                                                Layout.topMargin: 2
                                                Layout.leftMargin: 8

                                                text: "(Q)商品库存"
                                                font.pixelSize: 12
                                                font.family: UIConfig.fontFamily
                                                font.weight: Font.Normal
                                                color: "#616E80"
                                                horizontalAlignment: Text.AlignLeft
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            Text {
                                                id: idStock

                                                Layout.leftMargin: 8
                                                Layout.bottomMargin: 2

                                                text: page.currentWareInfo.stock
                                                font.pixelSize: 14
                                                font.family: UIConfig.fontFamily
                                                font.weight: Font.Bold
                                                color: "#0F172A"
                                                horizontalAlignment: Text.AlignLeft
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                        }
                                    }

                                }
                            }

                            Item {
                               Layout.fillWidth: true
                               Layout.fillHeight: true
                            }
                        }
                    }
                }
            }
        }
    }

    /** 文件选择 */
    function showFileDialog(parent, json) {
        var component = Qt.createComponent("qrc:/Qml/Component/FileDialog.qml")
        if(component.status === Component.Ready) {
            var object = component.createObject(parent, json)
            object.open()
        } else {
            console.error(component.errorString())
        }
    }
}
