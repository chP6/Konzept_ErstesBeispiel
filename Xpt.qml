import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.2
import com.bbm.config 1.0

Page {
    property int type: type
    width: 800
    height: 320
    title: "Xpt Control"
    id: page
    focusPolicy: Qt.ClickFocus


    Column{
        id: col1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 45
        anchors.topMargin: 25

        Label{
            text: "Source"
        }
        CustomTumbler{
            slotNumber: 0
        }
        CustomTumbler{
            slotNumber: 1
        }
        CustomTumbler{
            slotNumber: 2
        }
        CustomTumbler{
            slotNumber: 3
        }
        CustomTumbler{
            slotNumber: 4
        }
        CustomTumbler{
            slotNumber: 5
        }

    }

    Column{
        id: col2
        anchors.left: col1.right
        anchors.top: parent.top
        anchors.leftMargin: 25
        anchors.topMargin: 25

        Label{
            text: "Destination"
        }
        CustomTumbler{
            output: true
        }
    }
    Button{
        id: xptConnect
        text: checked ? "disconnect" : "connect"
        width: 200
        height: 100
        anchors.top: parent.top
        anchors.topMargin: 35
        anchors.right: parent.right
        anchors.rightMargin: 25
        highlighted: true
        checkable: true
        checked: xptBackend.connected
        onClicked: xptBackend.connectXpt(checked)
    }
    Label{
        anchors.top: xptConnect.bottom
        anchors.topMargin: 15
        anchors.right: xptConnect.right
        visible: xptConnect.checked
        text: xptBackend.connected ? "Connected to: " + xptBackend.ipAdress[0]+"."+xptBackend.ipAdress[1]+"."+xptBackend.ipAdress[2]+"."+xptBackend.ipAdress[3] :
                                        "Connecting to: " + xptBackend.ipAdress[0]+"."+xptBackend.ipAdress[1]+"."+xptBackend.ipAdress[2]+"."+xptBackend.ipAdress[3]
    }

    Text {
        id: xptTypeLabel
        anchors.left: row1.left
        anchors.top: parent.top
        anchors.topMargin: 25
        text: "Type:"
        color: Material.foreground
    }

    ComboBox{

        id: xptType
        anchors.left: row1.left
        anchors.top: xptTypeLabel.bottom
        width: 200
        textRole: "text"
        model: ListModel {
            id: cbItems
            ListElement { text: "BlackMagic"; type: Config.Blackmagic  }
            ListElement { text: "Ross"; type: Config.Ross}
        }
        currentIndex: xptBackend.xptType
        onCurrentIndexChanged: xptBackend.xptType = cbItems.get(currentIndex).type
    }

    Text {
        id: ipLabel
        text: "IP Address:"
        anchors.left: row1.left
        anchors.bottom: row1.top
        color: Material.foreground
    }
    Row{
        id : row1
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 25
        anchors.right: parent.right
        anchors.rightMargin: 25

        TextField{
            id:ip1
            text: xptBackend.ipAdress[0]
            validator: IntValidator {bottom: 0; top: 255;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: [ip2.focus = true, xptBackend.ipAdress[0] = text]
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip1.color = Material.foreground
            onTextEdited:  ip1.color = "red"
        }
        TextField{
            id:ip2
            text: xptBackend.ipAdress[1]
            validator: IntValidator {bottom: 0; top: 255;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: [ip3.focus = true, xptBackend.ipAdress[1] = text]
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip2.color = Material.foreground
            onTextEdited:  ip2.color = "red"
        }
        TextField{
            id:ip3
            text: xptBackend.ipAdress[2]
            validator: IntValidator {bottom: 0; top: 255;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: [ip4.focus = true, xptBackend.ipAdress[2] = text]
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip3.color = Material.foreground
            onTextEdited:  ip3.color = "red"
        }
        TextField{
            id:ip4
            text: xptBackend.ipAdress[3]
            validator: IntValidator {bottom: 2; top: 254;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: [page.focus = true, xptBackend.ipAdress[3] = text]
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip4.color = Material.foreground
            onTextEdited:  ip4.color = "red"
        }
    }
}
