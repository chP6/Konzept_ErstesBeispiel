import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.2

Page {
    width: 800
    height: 400
    title: "Others"
    id: page

    property bool clickable: true
    property int accent: accent
    property int theme: theme

    function click(){
        if(buttonGroup.checkedButton){
            buttonGroup.checkedButton.clicked();
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
        }

        Button{
            id:btSetLowerLimit
            text: "Set Lower Limit"
        }

        Button{
            id:btClearLimit
            text: "Clear Limit"
        }

        Button{
            id: btCalibrate
            text: "Calibreate"
        }
//        Rectangle{
//            id:spacer1
//            height: 30
//            width: 30
//            color: "transparent"
//        }

        CustomDial{
            id:dialHeadPower
            anchors.horizontalCenter: btCalibrate.horizontalCenter
            name: "HeadPower"
            from: 0
            to: 3
            checkable: true
            autoExclusive: true
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
            }

            Button{
                id:btSaveFile
                text: "Save File"
            }

            Button{
                id:btLoadFile
                text: "Load File"
            }
            Button{
                id: btErrorWindow
                text: "Error Window"
                onClicked: errorWindow.open()
            }

//            Rectangle{
//                id:spacer2
//                height: 30
//                width: 30
//                color: "transparent"
//            }

            CustomDial{
                id:dialMrror
                name: "Mirror"
                from: 0
                to: 3
                checkable: true
                autoExclusive: true
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
                currentIndex: 6
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
                       ListElement { text: "ShadeA700" ; accent: Material.ShadeA700 }
                       ListElement { text: "Teal" ; accent: Material.Teal }
                       ListElement { text: "Yellow" ; accent: Material.Yellow }
                   }
                onCurrentIndexChanged: accent = cbItems.get(currentIndex).accent
        }

            Switch{
                id: themeSwitch
                text: checked ?  "Light" : "Dark"
                onCheckedChanged: checked ?  theme = Material.Dark : theme = Material.Light
            }

    }

}
