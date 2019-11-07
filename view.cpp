#include "view.h"

View::View(Model& model, Controller& controller)
    : homeBackend(&model, &controller),cameraBackend(&model, &controller),
      othersBackend(&model, &controller), xptBackend(&model, &controller), controlsBackend(&model, &controller)
{
    this->controller = &controller;
    this->model = &model;
}

View::~View(){}

void View::on_modelUpdate()
{
    cameraBackend.update();
    homeBackend.update();
    xptBackend.update();
    othersBackend.update();
    controlsBackend.update();
}

void View::on_serverConnectionStatusChanged()
{
    homeBackend.serverConnectedChanged();
}


void View::on_xptConnectionStatusChanged(bool connected)
{
    xptBackend.connectedChanged(connected);
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
    case PanInverted: controlsBackend.invertPanChanged();  break;
    case TiltInverted: controlsBackend.invertTiltChanged();break;
    case ZoomInverted: controlsBackend.invertZoomChanged(); break;
    case FocusInverted: controlsBackend.invertFocusChanged(); break;
    case TravellingInverted: controlsBackend.invertTravellingChanged(); break;
    default: break;
    }
}

void View::on_newError()
{
    othersBackend.errorChanged();
}

