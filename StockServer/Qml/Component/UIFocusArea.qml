import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

FocusScope {
    id: root

    property alias areaRadius : rectangle.radius
    default property alias items: stackLayout.children

    StackLayout {
        id: stackLayout
        anchors.fill: parent
    }

    Rectangle {
        id: rectangle
        anchors.fill: parent
        border.width: 3
        border.color: "transparent"
        color: "transparent"
        antialiasing: true
    }
}
