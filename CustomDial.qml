import QtQuick 2.9
import QtQuick.Controls 2.2
//import QtQuick.Controls.impl 2.2
//import QtQuick.Templates 2.2 as T
import QtQuick.Controls.Material 2.2
import io.qt.examples.backend 1.0
import com.bbm.config 1.0


    AbstractButton{

        property string name: name
        property string col: col
        property alias value: dial2.value
        property alias from: dial2.from
        property alias to: dial2.to
        property bool center: center
        property var backend: backend
        property int field: field
        property bool textValue: textValue
        signal move

        function clickAction(){
            dial2.enabled = true;
            cameraBackend.setField(field);
        }

        id:area
        implicitWidth: 110
        implicitHeight: 110

        from: backend[Config.Min]
        to: backend[Config.Max]
        value: backend[Config.Value]
        property var textList: backend[Config.TextList]

        property int centerValue: backend[Config.Value]-(backend[Config.Max]-backend[Config.Min])/2


        Text {
            id: valueText
            text: textValue ? textList[dial2.value.toFixed(0)-dial2.from] : Number(dial2.value).toFixed(0)
            anchors.bottom:parent.bottom
            anchors.horizontalCenter:  parent.horizontalCenter
            anchors.bottomMargin: -5
            color: Material.foreground
            font.pixelSize: 15
        }

        Text {
            id: titleText
            text: name
            anchors.top: parent.top
            anchors.horizontalCenter:  parent.horizontalCenter
            anchors.topMargin: -5
            color: Material.foreground
            font.pixelSize: 15
            //font.underline: true
        }

        states: [
            State {
                name: ""
            },
            State {
                name: "notAvailable"
                PropertyChanges { target: bgItem; color: "grey";}
                PropertyChanges { target: area; enabled: false;}
                PropertyChanges { target: valueText; text: "-";}
            },

            State {
                name: "notReady"
                PropertyChanges {target: bgItem; color: Material.accent}
                PropertyChanges {target: handleItem; color: Qt.tint(Material.accent, "#10FFFFFA")}
            },
            State {
                name: "text"
                PropertyChanges {target:area; textValue: true }
            },
            State {
                name: "center"
                PropertyChanges {target: area; center: true}
                PropertyChanges {target: area; from: -(backend[Config.Max] -backend[Config.Min])/2}
                PropertyChanges {target: area; to: (backend[Config.Max] -backend[Config.Min])/2}
                PropertyChanges {target: area; value: centerValue}
            }

        ]
        state: backend[Config.State]
        background: Dial{
            id:dial2
            width: parent.width-valueText.height-titleText.height
            height: parent.height-valueText.height-titleText.height
            anchors.centerIn: parent
            enabled: false
            snapMode: Dial.SnapAlways
            stepSize: 1


            onMoved: center ? cameraBackend.setValue(field,value+to) : cameraBackend.setValue(field,value)
            //onValueChanged: console.log(width, parent.width)

            background: Rectangle {
                   id: bgItem
                   x: dial2.width / 2 - width / 2
                   y: dial2.height / 2 - height / 2
                   width: Math.max(64, Math.min(dial2.width, dial2.height))
                   height: width
                   color: "transparent"
                   radius: width / 2
                   border.color: col ?  (dial2.pressed ? Qt.lighter(col,1.2) : col) : (dial2.pressed ? Qt.lighter(Material.accent,1.2) : Material.accent)
                   //border.color: Material.accent

                   opacity: dial2.enabled ? 1 : 0.3
               }

               handle: Rectangle {
                   id: handleItem
                   x: dial2.background.x + dial2.background.width / 2 - width / 2
                   y: dial2.background.y + dial2.background.height / 2 - height / 2
                   width: 16
                   height: 16
                   color: col ?  (dial2.pressed ? Qt.lighter(col,1.2) : col) : (dial2.pressed ? Qt.lighter(Material.accent,1.2) : Material.accent)
                   radius: 8
                   antialiasing: true
                   opacity: dial2.enabled ? 1 : 0.3
                   transform: [
                       Translate {
                           y: -Math.min(dial2.background.width, dial2.background.height) * 0.4 + handleItem.height / 2
                       },
                       Rotation {
                           angle: dial2.angle
                           origin.x: handleItem.width / 2
                           origin.y: handleItem.height / 2
                       }
                   ]
               }
        }
        checkable: true
        autoExclusive: true
        onClicked: clickAction()
        onCheckedChanged: dial2.enabled = false

    }


