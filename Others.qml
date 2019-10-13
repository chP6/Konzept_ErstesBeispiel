import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.2
import io.qt.examples.backend 1.0
import Qt.labs.settings 1.0
import com.bbm.config 1.0

Page {
    width: 800
    height: 400
    title: "Others"
    id: page

    Settings{
        id: settings
        category: "userStyle"
        //fileName: "/opt/userStyle"
        property alias accent: page.accent
        property alias theme: page.theme
        property alias comboboxCurrentIndex: combobox.currentIndex
        property alias themeSwitchStatus: themeSwitch.checked
    }

    property bool clickable: true
    property int accent: accent
    property int theme: theme

    function click(){
        if(buttonGroup.checkedButton){
            buttonGroup.checkedButton.clicked();
        }
        else{
            buttonGroup.buttons[0].checked = true;
        }
    }

    ButtonGroup {
        id: buttonGroup
    }

    Column{
        id: col1
        anchors.left: parent.left
        anchors.leftMargin: 45
        anchors.top: parent.top
        anchors.topMargin: 20
        spacing: 10


        Label{
            id:lbHeadSetteings
            text: "Head Settings"
            font.pixelSize: 25
        }

        Button{
            id:btSetUpperLimit
            text: "Set Upper Limit"
            onClicked: othersBackend.setUpperLimit()
        }

        Button{
            id:btSetLowerLimit
            text: "Set Lower Limit"
            onClicked: othersBackend.setLowerLimit()
        }

        Button{
            id:btClearLimit
            text: "Clear Limit"
            onClicked: othersBackend.clearLimits()
        }

        Button{
            id: btCalibrate
            text: "Calibreate"
            onClicked: othersBackend.calibrateHead()
        }

        CustomDial{
            id:dialHeadPower
            name: "HeadPower"
            backend: othersBackend.headPower
            field: Config.HeadPower
            ButtonGroup.group:buttonGroup
        }


     }

        Column{
            id:col2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            spacing: 10


            Label{
                id:lbGeneral
                text: "General"
                font.pixelSize: 25
            }

            Button{
                id:btRequestValues
                text: "Request Values"
                onClicked: othersBackend.requestValues()
            }

            Button{
                id:btSaveFile
                text: "Save File"
                onClicked: othersBackend.saveSaveFile()
            }

            Button{
                id:btLoadFile
                text: "Load File"
                onClicked: othersBackend.loadSaveFile()
            }
            Button{
                id: btErrorWindow
                text: "Error Window"
                onClicked: errorWindow.open()
            }

            CustomDial{
                id:dialMrror
                name: "Mirror"
                backend: othersBackend.mirror
                field: Config.Mirror
                ButtonGroup.group:buttonGroup
            }

        }

        Popup{
            id: errorWindow
            width: page.width
            height: page.height

            Button{
                id: btClose
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 25
                text: "Close"
                onClicked: errorWindow.close()
            }

        }

        Column{
            id: col3
            anchors.right: parent.right
            anchors.rightMargin: 45
            anchors.top: parent.top
            anchors.topMargin: 20
            spacing: 10

            Label{
                id:lbStyle
                text: "Style"
                font.pixelSize: 25
            }

            ComboBox{
                id:combobox
                textRole: "text"
                   model: ListModel {
                       id: cbItems
                       ListElement { text: "Amber"; accent: Material.Amber }
                       ListElement { text: "BlueGrey"; accent: Material.BlueGrey}
                       ListElement { text: "Blue" ; accent: Material.Blue }
                       ListElement { text: "Brown" ; accent: Material.Brown }
                       ListElement { text: "Cyan" ; accent: Material.Cyan  }
                       ListElement { text: "DeepOrange" ; accent: Material.DeepOrange }
                       ListElement { text: "DeepPurple" ; accent: Material.DeepPurple }
                       ListElement { text: "Green" ; accent: Material.Green }
                       ListElement { text: "Grey" ; accent: Material.Grey }
                       ListElement { text: "Indigo" ; accent: Material.Indigo }
                       ListElement { text: "LightBlue" ; accent: Material.LightBlue }
                       ListElement { text: "LightGreen" ; accent: Material.LightGreen }
                       ListElement { text: "Lime" ; accent: Material.Lime }
                       ListElement { text: "Orange" ; accent: Material.Orange }
                       ListElement { text: "Pink" ; accent: Material.Pink }
                       ListElement { text: "Purple" ; accent: Material.Purple }
                       ListElement { text: "Red" ; accent: Material.Red }
                       ListElement { text: "Teal" ; accent: Material.Teal }
                       ListElement { text: "Yellow" ; accent: Material.Yellow }
                   }
                onCurrentIndexChanged: [accent = cbItems.get(currentIndex).accent, othersBackend.setColor(Material.color(page.accent))]

        }

            Switch{
                id: themeSwitch
                text: checked ?  "Light" : "Dark"
                onCheckedChanged: checked ?  theme = Material.Dark : theme = Material.Light
            }

    }

}
