#include "view.h"



View::View(Model& model, Controller& controller)
    : homeBackend(&model, &controller),cameraBackend(&model, &controller),
      othersBackend(&model, &controller), xptBackend(&model, &controller)
{
    this->controller = &controller;
    this->model = &model;
    std::signal(SIGINT,View::signalHandler);
}

View::~View()
{
    //delete ui;
}



void View::signalHandler(int signum)
{
    printf("Why do you kill me %d \n",signum);
    //QApplication::quit();
}



void View::on_modelUpdate()
{
    /*QtSlot: update all views  - connected to Model: updateView()*/
//    ui->home->updateUi();
//    ui->cameraView->updateUi();
//    ui->others->updateUi();
//    ui->xptControl->updateUi();
//    ui->controls->updateUi();
    cameraBackend.updateAll();
    homeBackend.updateAll();
    xptBackend.updateAll();
    othersBackend.updateAll();
}

void View::on_modelSetUp()
{
    /*QtSlot: set up camera view -> see setUpUi  - connected to Model: setUpView()*/
//    ui->cameraView->setUpUi();
}

void View::on_sppUpdate(bool active)
{
    /*QtSlot: start or stop at spp - connected to Model: ?*/
    //ui->home->sppUpdate(active);
}


void View::on_serverConnectionStatusChanged()
{
    homeBackend.serverConnectedChanged();
}

void View::on_cameraConnectionStatusChanged(bool connected)
{
    /*QtSlot: camera connected indicator  - connected to Model: updateCameraConnectionStatus()*/
    //ui->home->cameraConnectionChanged(connected);
}


void View::on_xptConnectionStatusChanged(bool connected)
{
    xptBackend.connectedChanged(connected);
    /*QtSlot: xpt connected indicator  - connected to Model: updateXptConnectionStatus()*/
    //ui->xptControl->xptStatusChanged(connected);
}

void View::on_xptEnableStatusChanged(bool connected)
{
    /*QtSlot: to release connect button if connection fails  - connected to Model: updateXptEnableStatus()*/
    //ui->xptControl->xptEnableChanged(connected);
}


void View::on_loadButtonCleared()
{
    /*?*/
    //ui->others->clearLoadButton();

}

void View::on_newReceive(int property)
{
    /*QtSlot: to indicate a new received value from camera - connected to Model: newSignalReceived()*/
    //ui->cameraView->signalRequest(property);
}

void View::on_newRequest()
{
  /*QtSlot: to indicate everything will be requested again - connected to Model: receiveAllNew()*/
    //ui->cameraView->newRequest();
}

void View::on_modelUpdateProperty(properties_t property)
{
    switch (property) {
    case Ped: cameraBackend.blackLevelChanged(); break;
    case Iris: cameraBackend.irisChanged();break;
    case GammaTable: cameraBackend.gammaTableChanged();break;
    case Gamma: cameraBackend.gammaChanged();break;
    case KneePoint: cameraBackend.kneePointChanged();break;
    case Color: cameraBackend.saturationChanged();break;
    case ColorTemp: cameraBackend.colorTemperatureChanged();break;
    case Gain: cameraBackend.gainChanged();break;
    case Detail: cameraBackend.detailChanged();break;
    case Shutter: cameraBackend.shutterChanged();break;
    case NdFilter: cameraBackend.ndFilterChanged();break;
    case WhiteRed: cameraBackend.redWhiteChanged();break;
    case WhiteBlue: cameraBackend.blueWhiteChanged();break;
    case BlackRed: cameraBackend.redBlackChanged();break;
    case BlackBlue: cameraBackend.blueBlackChanged();break;
    case PtSpeed: homeBackend.ptSpeedChanged();break;
    case BounceZoomSpeed: homeBackend.zoomSpeedChanged();break;
    case TransSpeed: homeBackend.transitionSpeedChanged();break;
    case Ramp: homeBackend.rampChanged();break;
    case Spp1: homeBackend.spp1Changed();break;
    case Spp2: homeBackend.spp2Changed();break;
    case SppwWaitTime: homeBackend.sppWaitTimeChanged();break;
    case HeadNr: homeBackend.headNrChanged();break;
    case Mirror: othersBackend.mirrorChanged();break;
    case HeadPower: othersBackend.headPowerChanged();break;
    default: break;
    }

}

void View::on_modelUpdateFlag(flags_t flag)
{
    switch (flag) {
    case SppEnabled: homeBackend.sppStartChanged();break;
    case BounceEnabled: homeBackend.bounceChanged(); break;
    case PresetMoving: homeBackend.presetMovingChanged();break;
    case FastTransEnabled: homeBackend.fpmChanged(); break;
    case CameraConnected: homeBackend.cameraConnectedChanged();break;
    case PanInverted:  break;
    case TiltInverted: break;
    case ZoomInverted: break;
    case FocusInverted: break;
    case TravellingInverted: break;
    default: break;
    }
}




void View::on_btHome_clicked()
{
/*    ui->stackedWidget->setCurrentIndex(0);  //change window to home
    ui->home->stackChanged();     */          //push last pressed button on this page again
}

void View::on_btCamCtrl_clicked()
{
  /*  ui->stackedWidget->setCurrentIndex(1);  //change window to camera control
    ui->cameraView->stackChanged(); */        //push last pressed button on this page again

}

void View::on_btXptControl_clicked()
{
 /*   ui->stackedWidget->setCurrentIndex(2);  //change window to xpt control
    ui->xptControl->stackChanged();  */      //push last pressed button on this page again

}

void View::on_btOthers_clicked()
{
  /*  ui->stackedWidget->setCurrentIndex(3);  //change window to others
    ui->others->stackChanged(); */            //push last pressed button on this page again

}

void View::on_btControls_clicked()
{
//    ui->stackedWidget->setCurrentIndex(4);
//    ui->controls->stackChanged();
}
