import QtQuick 2.9
import QtQuick.Controls 2.2
//import QtQuick.Controls.impl 2.2
//import QtQuick.Templates 2.2 as T
import QtQuick.Controls.Material 2.2
import io.qt.examples.backend 1.0
import com.bbm.config 1.0

Button{
    id: xptSelector

    property int slotNumber: slotNumber
    property bool output: output


    text: output ? xptBackend.outputLabels[tumbler.currentIndex] : xptBackend.inputLabels[tumbler.currentIndex]
    onClicked: popup.open()

    Label{
        text: output ? "" : Number(slotNumber)
        anchors.right: parent.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    Popup {
        id: popup
        width: 200
        height: 200
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        Tumbler {
            id: tumbler

            height: parent.height
            anchors.fill: parent
            model: output ? xptBackend.outputLabels : xptBackend.inputLabels
            wrap: false
            currentIndex: output ? xptBackend.xptDestination  : xptBackend.xptSources[slotNumber]
            //onCurrentItemChanged: output ? xptBackend.xptDestination = currentIndex : xptBackend.xptSources[slotNumber] = currentIndex
            onMovingChanged: output ? xptBackend.xptDestination = currentIndex : xptBackend.xptSources[slotNumber] = currentIndex
            //onCurrentIndexChanged: output ? xptBackend.xptDestination = currentIndex : xptBackend.xptSources[slotNumber] = currentIndex
        }


    }
}
