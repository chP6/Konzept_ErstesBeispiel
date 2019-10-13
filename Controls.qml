import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.2
import com.bbm.config 1.0

Page {
    width: 800
    height: 320
    title: "Controls"
    id: page

    property int axis: axis
    property int controlType: controlType
    property bool clickable: true
    function click(){
      cameraBackend.setField(Config.None);
    }
    ListModel{ 
        id: itemsModel
        ListElement{text: "none"; controlType: Config.KControlNone}
        ListElement{text: "Joystick Left/Right"; controlType: Config.KControlJoystickX }
        ListElement{text: "Joystick Up/Down"; controlType: Config.KControlJoystickY}
        ListElement{text: "Joystick Rotation"; controlType: Config.KControlJoystickZ}
        ListElement{text: "Zoom Rocker"; controlType: Config.KControlZoomRocker}
        ListElement{text: "Focus Wheel"; controlType: Config.KControlFocusWheel}
    }


    Column{
        id:col2
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 45

        ComboBox{
            id: panCombobox
            textRole: "text"
            width: 200
            model: itemsModel
            onCurrentIndexChanged: controlsBackend.setControl(Config.KAxisPan,itemsModel.get(currentIndex).controlType)
            currentIndex: controlsBackend.control[Config.KAxisPan]



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
            checked: controlsBackend.invertPan
            onClicked: checked ? controlsBackend.invertPan = true : controlsBackend.invertPan = false
        }
      }

        ComboBox{
            id: tiltCombobox
            model: itemsModel
            textRole: "text"
            width: panCombobox.width
            onCurrentIndexChanged: controlsBackend.setControl(Config.KAxisTilt,itemsModel.get(currentIndex).controlType)
            currentIndex: controlsBackend.control[Config.KAxisTilt]
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
            checked: controlsBackend.invertTilt
            onClicked: checked ? controlsBackend.invertTilt = true : controlsBackend.invertTilt = false
        }
      }

        ComboBox{
            id: zoomCombobox
            width: panCombobox.width 
            textRole: "text"
            model: itemsModel
            onCurrentIndexChanged: controlsBackend.setControl(Config.KAxisZoom,itemsModel.get(currentIndex).controlType)
            currentIndex: controlsBackend.control[Config.KAxisZoom]
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
            checked: controlsBackend.invertZoom
            onClicked: checked ? controlsBackend.invertZoom = true : controlsBackend.invertZoom = false
        }
      }

        ComboBox{
            id: focusCombobox
            width: panCombobox.width
            textRole: "text"
            model: itemsModel
            onCurrentIndexChanged: controlsBackend.setControl(Config.KAxisFocus,itemsModel.get(currentIndex).controlType)
            currentIndex: controlsBackend.control[Config.KAxisFocus]
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
            checked: controlsBackend.invertFocus
            onClicked: checked ? controlsBackend.invertFocus = true : controlsBackend.invertFocus = false
        }
      }

        ComboBox{
            id: travelingCombobox
            width: panCombobox.width
            textRole: "text"
            model: itemsModel
            onCurrentIndexChanged: controlsBackend.setControl(Config.KAxisTravelling,itemsModel.get(currentIndex).controlType)
            currentIndex: controlsBackend.control[Config.KAxisTravelling]
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
            checked: controlsBackend.invertTravelling
            onClicked: checked ? controlsBackend.invertTravelling = true : controlsBackend.invertTravelling = false
        }
      }

    }



}
