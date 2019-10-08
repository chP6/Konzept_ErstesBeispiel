import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.2
import io.qt.examples.backend 1.0
import com.bbm.config 1.0

Page {
    width: 800
    height: 400
    title: "Home"
    id: page
    property bool clickable: true
    function click(){
        if(buttonGroup.checkedButton){
            buttonGroup.checkedButton.clicked();
        }
    }
    Material.onAccentChanged: Material.accent === Material.Grey ? btStorePreset.highlighted = false : btStorePreset.highlighted = true

  //Material.accent: cbItems.get(combobox.currentIndex).accent
    //Material.accent: fpm.checked ? Material.DeepPurple : Material.Blue
  //Material.accent: Material.Lime
    //accent: cbItems.get(combobox.currentIndex).accent

    ButtonGroup {
        id: buttonGroup

    }

        Row {
            id:row1
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            //spacing: 2

            CustomDial {

                id: ptSpeed
                name: "P/T Speed"
                backend: homeBackend.ptSpeed
                field: Config.PtSpeed
                ButtonGroup.group: buttonGroup
            }
            ToolSeparator{
                height: ptSpeed.height
            }

            CustomDial {
                id: transSpeed
                name: "Trans. Speed"
                backend: homeBackend.transitionSpeed
                field: Config.TransSpeed
                ButtonGroup.group: buttonGroup
            }

            ToolSeparator{
                height: ptSpeed.height
            }

            CustomDial {
                id: ramp
                name: "Ramp"
                backend: homeBackend.ramp
                field: Config.Ramp
                ButtonGroup.group: buttonGroup

            }

            ToolSeparator{
                height: ptSpeed.height
            }

            Switch {
                id: fpm
                text: qsTr("FPM")
                anchors.verticalCenter: ptSpeed.verticalCenter
                checked: homeBackend.fpm
                onClicked: checked ? homeBackend.fpm = true : homeBackend.fpm = false

            }

        }

        Row {
            id: row2
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            //spacing: 2

            CustomDial {
                id: spp1              
                name: "Preset 1"
                backend: homeBackend.spp1
                field: Config.Spp1
                ButtonGroup.group: buttonGroup
            }
            ToolSeparator{
                height: spp1.height
            }

            CustomDial {
                id: spp2
                name: "Preset 2"
                backend: homeBackend.spp2
                field: Config.Spp2
                ButtonGroup.group: buttonGroup
            }

            ToolSeparator{
               height: spp1.height
            }

            CustomDial{
                id:waitTime
                name:"Wait Time"
                backend: homeBackend.sppWaitTime
                field: Config.SppwWaitTime
                ButtonGroup.group: buttonGroup
            }

            ToolSeparator{
                height: spp1.height
            }
            Switch {
                id: swSppstart
                text: qsTr("Start")
                onClicked: checked ? [text = "Stop", homeBackend.sppStart = true] : [text = "Start", homeBackend.sppStart = false]
                anchors.verticalCenter: spp1.verticalCenter
                checked: homeBackend.sppStart
            }
        }

        Button {
            id: btStorePreset
            width: 220
            height: 130
            text: qsTr("Store Preset")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20
            highlighted: true
            onClicked: homeBackend.storePreset();
        }

      Row{
          id: row3
          anchors.left: parent.left
          anchors.leftMargin: 20
          anchors.top: parent.top
          anchors.topMargin: 20
          //spacing: 2

          Switch{
              id: swBounce
              text: qsTr("Bounce")
              checked: homeBackend.bounce
              onClicked: checked ? homeBackend.bounce = true : homeBackend.bounce = false
              anchors.verticalCenter: dialAutoZoomSpeed.verticalCenter
          }
          ToolSeparator{
              height: dialAutoZoomSpeed.height
          }

          Button{
              id: btWideSet
              text: qsTr("Wide Set")
              enabled: swBounce.checked ? true : false
              anchors.verticalCenter: dialAutoZoomSpeed.verticalCenter

          }
          ToolSeparator{

            height: dialAutoZoomSpeed.height
          }

          CustomDial{
              id: dialAutoZoomSpeed
              name: "A. Zoom Speed"
              backend: homeBackend.zoomSpeed
              field: Config.BounceZoomSpeed
              ButtonGroup.group: buttonGroup              
          }
      }

      Row{
          id: row4
          anchors.right: parent.right
          anchors.rightMargin: 20
          anchors.top: parent.top
          anchors.topMargin: 30
          spacing: 5

          Rectangle{
              function startAnimation(){
                  indicator.x = box.x
                  indicator.y = box.y-indicator.width
                  //indicator.visible = true;
                  startScale.start();
              }

              function stopAnimation(){
                  stopScale.start();
                  //indicator.visible = false;
                  //indicator.x = box.x
                  //indicator.y = box.y-indicator.width

              }


              id: box
              width: 90
              height: 90
              border.color: Material.accent
              radius: 3
              color: "transparent"
              anchors.verticalCenter: btHeadNr.verticalCenter

              Rectangle{
                  id: indicator
                  radius: 10
                  width:10
                  height: 10
                  scale: 0
                  color: Material.accent
                  x: box.x
                  y: box.y-indicator.width
                  //visible: false


                  ScaleAnimator{
                      id: startScale
                            target: indicator
                            from: 0
                            to: 1
                            easing.type: Easing.OutExpo;
                            duration: 500
                                        }
                  ScaleAnimator{
                                  id:stopScale
                                  target: indicator
                                  from: 1
                                  to: 0
                                  easing.type: Easing.OutExpo;
                                  duration: 500

                                                }


                  SequentialAnimation{

                    id: presetAnimation


                      NumberAnimation {
                          target: indicator
                          property: "x"
                          duration: 500
                          easing.type: Easing.InOutQuad
                          from: box.x
                          to: box.x+box.width
                      }

                      NumberAnimation {
                          target: indicator
                          property: "y"
                          duration: 500
                          easing.type: Easing.InOutQuad
                          from: box.y-indicator.width
                          to: box.y+box.width-indicator.width
                      }
                      NumberAnimation {
                          target: indicator
                          property: "x"
                          duration: 500
                          easing.type: Easing.InOutQuad
                          from: box.x+box.width
                          to: box.x
                      }

                      NumberAnimation {
                          target: indicator
                          property: "y"
                          duration: 500
                          easing.type: Easing.InOutQuad
                          from: box.y+box.width-indicator.width
                          to: box.y-indicator.width
                      }



                      running: homeBackend.presetMoving ? true : false
                      loops: Animation.Infinite
                      onRunningChanged: running ?  box.startAnimation() : box.stopAnimation()
                  }


              }



              Label{
                  id: lbPresetNr
                  text:Number(homeBackend.preset+1)
                  font.pixelSize: 28
                  anchors.centerIn: parent
              }

              Label{
                  text: "Preset Number"
                  font.pixelSize: 14
                  anchors.bottom: parent.top
                  anchors.bottomMargin: 5
                  anchors.horizontalCenter: parent.horizontalCenter

              }


          }
          ToolSeparator{
          height: 100
          }

          Button{
              id: btHeadNr
              width: 100
              height: 100
              text: Number(homeBackend.headNr)
              font.pixelSize: 28
              checkable: true
              autoExclusive: true
              ButtonGroup.group: buttonGroup
              onClicked: cameraBackend.setField(Config.HeadNr)

              Label{
                  id: lbHeadNumber
                  text: "Head Number"
                  font.pixelSize: 14
                  anchors.bottom: parent.top
                  anchors.horizontalCenter: parent.horizontalCenter
              }
          }
      }



          Label{
              id: lbServer
              text: "Server:"
              anchors.verticalCenter: parent.verticalCenter
              anchors.verticalCenterOffset: -30
              anchors.right: serverOk.left
              anchors.rightMargin: 20
          }
          Label{
              id: serverOk
              text: "Ok"
              anchors.verticalCenter: lbServer.verticalCenter
              anchors.right: parent.right
              anchors.rightMargin: 20
              //visible: myModel.serverStatus
              //onVisibleChanged: serverOk.visible ? serverBusy.visible = false : serverBusy.visible = true

          }
          BusyIndicator{
             id:serverBusy
             running: true
             scale: 0.5
             anchors.horizontalCenter: serverOk.horizontalCenter
             anchors.verticalCenter: serverOk.verticalCenter
             visible: !serverOk.visible

         }




          Label{
              id: lbCamera
              text: "Camera:"
              anchors.top: lbServer.bottom
              anchors.topMargin: 5
              anchors.horizontalCenter: lbServer.horizontalCenter

          }
          Label{
              id: cameraOk
              text: "Ok"
              anchors.verticalCenter: lbCamera.verticalCenter
              anchors.horizontalCenter: serverOk.horizontalCenter
              //visible: myModel.cameraStatus
              //onVisibleChanged: cameraOk.visible ? cameraBusy.visible = false : cameraBusy.visible = true
          }

          BusyIndicator{
             id:cameraBusy
             running: true
             scale: 0.5
             anchors.horizontalCenter: cameraOk.horizontalCenter
             anchors.verticalCenter: cameraOk.verticalCenter
             visible: !cameraOk.visible
         }




}


