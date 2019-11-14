import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import io.qt.examples.backend 1.0
import com.bbm.config 1.0


Page{
    width: 800
    height: 320
    title: "Camera"
    id: camera
    property bool clickable: true
    function click(){
        if(buttonGroup.checkedButton){
            buttonGroup.checkedButton.clicked();
        }
    }

    ButtonGroup {
        id: buttonGroup
    }
//    Config{
//        id: configs
//    }

    Row{
        id:row1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 25
        anchors.leftMargin: 20
        spacing: -7


        CustomDial {
            id: bRed
            backend: cameraBackend.redBlack
            field: Config.BlackRed
            ButtonGroup.group: buttonGroup
            col: "indianred"
        }
        ToolSeparator{
            id:sep1
            height: bRed.height
        }

        CustomDial {
            id: bBlue
            backend: cameraBackend.blueBlack
            field: Config.BlackBlue
            ButtonGroup.group: buttonGroup
            col: "cornflowerblue"
        }
    }

   Rectangle{
       id: hline1
       x: row1.x
       y: row1.y
       height: 1
       width: bBlue.width + bRed.width
       color: Material.foreground
       Label{
           anchors.bottom: parent.top
           anchors.horizontalCenter: parent.horizontalCenter
           text: "Black"
       }
   }

   Row{
       id:row2
       anchors.verticalCenter: parent.verticalCenter
       anchors.left: parent.left
       anchors.leftMargin: 20
       spacing: -7

       CustomDial {
           id: wRed
           backend: cameraBackend.redWhite
           field: Config.WhiteRed
           ButtonGroup.group: buttonGroup
           col: "indianred"

       }
       ToolSeparator{
            id: sep2
            height: wRed.height
       }

       CustomDial {
           id: wBlue
           backend: cameraBackend.blueWhite
           field: Config.WhiteBlue
           ButtonGroup.group: buttonGroup
           col: "cornflowerblue"

       }
   }

   Rectangle{
       id: hline2
       x: row2.x
       y: row2.y
       height: 1
       width: wBlue.width + wRed.width
       color: Material.foreground
       Label{
           anchors.bottom: parent.top
           anchors.horizontalCenter: parent.horizontalCenter
           text: "White"
       }
   }

   Row{
       id:row3
       anchors.top: parent.top
       anchors.left: parent.left
       anchors.topMargin: 25
       anchors.leftMargin: 20
       spacing: -7

       CustomDial {
           id: shutter
           name: "Shutter"
           backend: cameraBackend.shutter
           field: Config.Shutter
           ButtonGroup.group: buttonGroup
       }

       ToolSeparator{
            height: shutter.height
       }

       CustomDial {
           id: ndFilter
           name: "ND Filter"
           backend: cameraBackend.ndFilter
           field: Config.NdFilter
           ButtonGroup.group: buttonGroup
       }
   }

//   Rectangle{
//       height: 200
//       width: 1
//       x: gain.x//(gain.x-ndFilter.x)/2
//       y: ndFilter.y
//       color: Material.foreground
//   }

   Row{
       id:row4
       anchors.bottom: parent.bottom
       anchors.horizontalCenter: parent.horizontalCenter
       anchors.bottomMargin: 25
       spacing: -7

       CustomDial {
           id: gamma
           name: "Gamma"
           backend: cameraBackend.gamma
           field: Config.Gamma
           ButtonGroup.group: buttonGroup
       }

       ToolSeparator{
            height: gamma.height
       }

       CustomDial {
           id: gammaTab
           name: "Gamma Tab."
           backend: cameraBackend.gammaTable
           field: Config.GammaTable
           ButtonGroup.group: buttonGroup
       }


}

   Row{
       id:row5
       anchors.verticalCenter: parent.verticalCenter
       anchors.horizontalCenter: parent.horizontalCenter
       spacing: -7

       CustomDial {
           id: knee
           name: "Knee"
           backend: cameraBackend.knee
           field: Config.Knee
           ButtonGroup.group: buttonGroup


       }
       ToolSeparator{
            height: knee.height
       }

       CustomDial {
           id: kneePt
           name: "Knee Pt."
           backend: cameraBackend.kneePoint
           field: Config.KneePoint
           ButtonGroup.group: buttonGroup
       }


}
   Row{
       id:row6
       anchors.top: parent.top
       anchors.topMargin: 25
       anchors.horizontalCenter: parent.horizontalCenter
       spacing: -7

       CustomDial {
           id: gain
           name: "Gain"
           backend: cameraBackend.gain
           field: Config.Gain
           ButtonGroup.group: buttonGroup
       }

       ToolSeparator{
            height: gain.height
       }

       CustomDial {
           id: detail
           name: "Detail"
           backend: cameraBackend.detail
           field: Config.Detail
           ButtonGroup.group: buttonGroup
       }


}
//   Rectangle{
//       height: parent.height-40
//       width: 2
//       anchors.right: row7.left
//       color: Material.foreground
//   }

   Row{
       id:row7
       anchors.top: parent.top
       anchors.right: parent.right
       anchors.topMargin: 25
       anchors.rightMargin: 20
       spacing: -7

       CustomDial {
           id: saturation
           name: "Saturation"
           backend: cameraBackend.saturation
           field: Config.Color
           ButtonGroup.group: buttonGroup
       }
       ToolSeparator{
            height: saturation.height
       }

       CustomDial {
           id: colTemp
           name: "Col. Temp"
           backend: cameraBackend.colorTemperature
           field: Config.ColorTemp
           ButtonGroup.group: buttonGroup
       }
   }

   Row{
       id:row8
       anchors.bottom: parent.bottom
       anchors.right: parent.right
       anchors.rightMargin: 20
       anchors.bottomMargin: 25
       spacing: -7


   CustomDial {
       id: blackLevel
       name: "Black Level"
       backend: cameraBackend.blackLevel
       field:  Config.Ped
       ButtonGroup.group: buttonGroup
   }

   ToolSeparator{
        height: blackLevel.height
   }

   CustomDial {
       id: iris
       name: "Iris"
       backend: cameraBackend.iris
       field: Config.Iris
       ButtonGroup.group: buttonGroup

   }

}
}




