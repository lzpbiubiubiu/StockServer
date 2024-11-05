import QtQuick 2.12
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1

// 文件
FileDialog {
    property var accept: null
    
    id: fileDialog
    onAccepted: {
        console.log("You chose: " + fileDialog.file)
        fileDialog.close()
        if(fileDialog.accept && typeof fileDialog.accept === 'function')
            fileDialog.accept(fileDialog.file)
    }
    onRejected: {
        console.log("Canceled")
        fileDialog.close()
    }
    Component.onCompleted: visible = true
}
