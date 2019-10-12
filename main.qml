import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.VirtualKeyboard 2.2

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 400
    title: qsTr("Controller Application")
    Material.theme: others.theme
    Material.accent: others.accent



    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: -parent.height
        width: parent.width


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

    SwipeView{
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex


        onCurrentIndexChanged: currentItem.clickable ? currentItem.click() : {}//findActive()
        //Material.theme:  Material.Light//fpm.checked ? Material.Dark : Material.Light
        //Material.accent: Material.Lime
    Home{
        id:home
    }
    Camera{
        id:camera
    }
    Xpt{
        id: xpt
    }

    Controls{
        id: controls

    }

    Others{
        id: others
    }


    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Home")
            //onClicked: home.click()
        }
        TabButton {
            text: qsTr("Camera")
            //onClicked: camera.click()
        }
        TabButton {
            text: qsTr("Xpt Control")
        }
        TabButton {
            text: qsTr("Controls")
        }
        TabButton {
            text: qsTr("Others")
            //onClicked: others.click()
        }
    }

//    InputPanel {
//        id: inputPanel
//        z: 99
//        x: 0
//        y: window.height
//        width: window.width

//        states: State {
//            name: "visible"
//            when: inputPanel.active
//            PropertyChanges {
//                target: inputPanel
//                y: window.height - inputPanel.height
//            }
//        }
//        transitions: Transition {
//            from: ""
//            to: "visible"
//            reversible: true
//            ParallelAnimation {
//                NumberAnimation {
//                    properties: "y"
//                    duration: 250
//                    easing.type: Easing.InOutQuad
//                }
//            }
//        }
//    }
}
