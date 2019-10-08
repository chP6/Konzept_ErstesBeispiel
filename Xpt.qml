import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.VirtualKeyboard 2.1
import QtQuick.VirtualKeyboard.Settings 2.2


Page {
    width: 800
    height: 320
    title: "Xpt Control"
    id: page
    focusPolicy: Qt.ClickFocus

    function nextField(object, value)
    {
        object.focus = true;
        console.log("value is ", parseInt(value,10));
    }

    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: -window.height
        width: window.width


        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: 0
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"

                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }



    Column{
        id: col1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 45
        anchors.topMargin: 25

        Label{
            text: "Source"
        }

        Button{
            id: xptSource1
            text: myModel.xptLabels[t1.currentIndex]
            onClicked: popup.open()

            Label{
                text: "1"
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            Popup {
                id: popup
                width: 200
                height: t1.height
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                Tumbler {

                    id: t1
                    anchors.fill: parent
                    model: myModel.xptLabels
                    wrap: false
                    currentIndex: myModel.xptSettings[0]
                    onCurrentIndexChanged: myModel.xptSettings[0] = currentIndex

                }

            }
        }
        Button{
            id: xptSource2
            text: myModel.xptLabels[t2.currentIndex]
            onClicked: popup2.open()

            Label{
                text: "2"
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            Popup {
                id: popup2
                width: 200
                height: t2.height
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                Tumbler {
                    id: t2
                    anchors.fill: parent
                    model: myModel.xptLabels
                    wrap: false
                    currentIndex: myModel.xptSettings[1]
                    onCurrentIndexChanged: myModel.xptSettings[1] = currentIndex
                }

            }

        }
        Button{
            id: xptSource3
            text: myModel.xptLabels[t3.currentIndex]
            onClicked: popup3.open()

            Label{
                text: "3"
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            Popup {
                id: popup3
                width: 200
                height: t3.height
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                Tumbler {
                    id: t3
                    anchors.fill: parent
                    model: myModel.xptLabels
                    wrap: false
                    currentIndex: myModel.xptSettings[2]
                    onCurrentIndexChanged: myModel.xptSettings[2] = currentIndex
                }

            }
        }

        Button{
            id: xptSource4
            text: myModel.xptLabels[t4.currentIndex]
            onClicked: popup4.open()

            Label{
                text: "4"
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            Popup {
                id: popup4
                width: 200
                height: t4.height
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                Tumbler {
                    id: t4
                    anchors.fill: parent
                    model: myModel.xptLabels
                    wrap: false
                    currentIndex: myModel.xptSettings[3]
                    onCurrentIndexChanged: myModel.xptSettings[3] = currentIndex
                }

            }
        }

        Button{
            id: xptSource5
            text: myModel.xptLabels[t5.currentIndex]
            onClicked: popup5.open()

            Label{
                text: "5"
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            Popup {
                id: popup5
                width: 200
                height: t5.height
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                Tumbler {
                    id: t5
                    anchors.fill: parent
                    model: myModel.xptLabels
                    wrap: false
                    currentIndex: myModel.xptSettings[4]
                    onCurrentIndexChanged: myModel.xptSettings[4] = currentIndex
                }

            }
        }

        Button{
            id: xptSource6
            text: myModel.xptLabels[t6.currentIndex]
            onClicked: popup6.open()


            Label{
                text: "6"
                anchors.right: parent.left
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            Popup {
                id: popup6
                width: 200
                height: t6.height
                modal: true
                focus: true
                y: parent.y - 2*height
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                Tumbler {
                    id: t6
                    anchors.fill: parent
                    model: myModel.xptLabels
                    wrap: false
                    currentIndex: myModel.xptSettings[5]
                    onCurrentIndexChanged: myModel.xptSettings[5] = currentIndex
                }

            }
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

        Button{
            id: xptDestination
            text: myModel.xptLabels[t7.currentIndex]
            onClicked: popup7.open()


            Popup {
                id: popup7
                width: 200
                height: 300
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                Tumbler {

                    id: t7
                    anchors.fill: parent
                    model: myModel.xptLabels
                    wrap: false

                    //                          currentIndex: myModel.xptSettings[0]
                    //                          onCurrentIndexChanged: myModel.xptSettings[0] = currentIndex

                }

            }
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
        model: ["BlackMagic","Ross"]
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
            text: "192"
            validator: IntValidator {bottom: 0; top: 255;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: nextField(ip2,text)
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip1.color = Material.foreground
            onTextEdited:  ip1.color = "red"
        }
        TextField{
            id:ip2
            text: "168"
            validator: IntValidator {bottom: 0; top: 255;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: nextField(ip3,text)
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip2.color = Material.foreground
            onTextEdited:  ip2.color = "red"
        }
        TextField{
            id:ip3
            text: "1"
            validator: IntValidator {bottom: 0; top: 255;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: nextField(ip4,text)
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip3.color = Material.foreground
            onTextEdited:  ip3.color = "red"
        }
        TextField{
            id:ip4
            text: "100"
            validator: IntValidator {bottom: 2; top: 254;}
            inputMethodHints: Qt.ImhDigitsOnly
            maximumLength: 3
            onAccepted: nextField(page,text)
            Keys.onPressed: event.key === Qt.Key_Enter ? text : ip4.color = Material.foreground
            onTextEdited:  ip4.color = "red"
        }
    }
}
