import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Component 1.0
import Page 1.0

// 功能按钮列表
ListView {
    /** 功能按钮点击 */
    signal signalFunctionButtonClicked(int type)

    id: functionListView
    boundsBehavior:Flickable.StopAtBounds
    spacing: 8
    clip: true
    model: page.functionPanel.functionModel

    delegate: FocusScope {
        width: functionListView.width 
        height: 40

        Rectangle {
            anchors.centerIn: parent
            width: parent.width-20
            height: parent.height
            anchors.topMargin: 0
            radius: 2
            border.width: 2
            border.color:"#BFDBFE"
            //border.color: Qt.rgba(0, 0, 0, 0.16)
        }

        UILabelButton {
            anchors.centerIn: parent
            width: parent.width-22
            height: parent.height-2
            color: "#EFF6FF"
            spacing: 9
            radius: 2
            focus: true
            leftMargin: 19
            rightMargin: 19
            imageWidth: 22
            imageHeight: 22
            source: ""
            fontWeight: Font.Bold
            fontSize: 14
            textColor: "#1E293B"
            textHorizontalAlignment: Text.AlignHCenter
            text: modelData.funcName

            onSignalClicked: {
                signalFunctionButtonClicked(modelData.funcType)
            }
        }
    }
}
