import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.2

Page {
    width: 800
    height: 320
    title: "Controls"
    id: page

    ListModel{
        id: itemsModel
        ListElement{text: "Joystick Left/Right"}
        ListElement{text: "Joystick Up/Down"}
        ListElement{text: "Joystick Rotation"}
        ListElement{text: "Zoom Rocker"}
        ListElement{text: "Focus Wheel"}
        ListElement{text: "none"}
    }


    Column{
        id:col2
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 45

        ComboBox{
            id: panCombobox
            width: 200
            model: itemsModel
        Label{
            id: lbPan
            text: "Pan"
            anchors.right: panCombobox.left
            anchors.rightMargin: 25
            anchors.verticalCenter: panCombobox.verticalCenter
        }
        Button{
            id: btInvertPan
            text: "invert"
            anchors.left: panCombobox.right
            anchors.leftMargin: 25
            anchors.verticalCenter: panCombobox.verticalCenter
            checkable: true
        }
      }

        ComboBox{
            id: tiltCombobox
            model: itemsModel
            width: panCombobox.width
        Label{
            id: lbTilt
            text: "Tilt"
            anchors.right: tiltCombobox.left
            anchors.rightMargin: 25
            anchors.verticalCenter: tiltCombobox.verticalCenter
        }
        Button{
            id: btInvertTilt
            text: "invert"
            anchors.left: tiltCombobox.right
            anchors.leftMargin: 25
            anchors.verticalCenter: tiltCombobox.verticalCenter
            checkable: true
        }
      }

        ComboBox{
            id: zoomCombobox
            width: panCombobox.width
            model: itemsModel
        Label{
            id: lbZoom
            text: "Zoom"
            anchors.right: zoomCombobox.left
            anchors.rightMargin: 25
            anchors.verticalCenter: zoomCombobox.verticalCenter
        }
        Button{
            id: btInvertZoom
            text: "invert"
            anchors.left: zoomCombobox.right
            anchors.leftMargin: 25
            anchors.verticalCenter: zoomCombobox.verticalCenter
            checkable: true
        }
      }

        ComboBox{
            id: focusCombobox
            width: panCombobox.width
            model: itemsModel
        Label{
            id: lbFocus
            text: "Focus"
            anchors.right: focusCombobox.left
            anchors.rightMargin: 25
            anchors.verticalCenter: focusCombobox.verticalCenter
        }
        Button{
            id: btInvertFocus
            text: "invert"
            anchors.left: focusCombobox.right
            anchors.leftMargin: 25
            anchors.verticalCenter: focusCombobox.verticalCenter
            checkable: true
        }
      }

        ComboBox{
            id: travelingCombobox
            width: panCombobox.width
            model: itemsModel
        Label{
            id: lbTraveling
            text: "Travelling"
            anchors.right: travelingCombobox.left
            anchors.rightMargin: 25
            anchors.verticalCenter: travelingCombobox.verticalCenter
        }
        Button{
            id: btInvertTraveling
            text: "invert"
            anchors.left: travelingCombobox.right
            anchors.leftMargin: 25
            anchors.verticalCenter: travelingCombobox.verticalCenter
            checkable: true
        }
      }

    }



}
