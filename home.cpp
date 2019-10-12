#include "home.h"

Home::Home()
{

}

Home::Home(Model *model, Controller *controller){
    this->model = model;
    this->controller = controller;
}

Home::~Home()
{

}

QList<QVariant> Home::ptSpeed()
{
    QList<QVariant> list;
    properties_t property = PtSpeed;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> Home::zoomSpeed()
{
    QList<QVariant> list;
    properties_t property = BounceZoomSpeed;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> Home::transitionSpeed()
{
    QList<QVariant> list;
    properties_t property = TransSpeed;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> Home::ramp()
{
    QList<QVariant> list;
    properties_t property = Ramp;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> Home::spp1()
{
    QList<QVariant> list;
    properties_t property = Spp1;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> Home::spp2()
{
    QList<QVariant> list;
    properties_t property = Spp2;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> Home::sppWaitTime()
{
    QList<QVariant> list;
    properties_t property = SppwWaitTime;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

int Home::preset()
{
    return model->getActivePreset();
}

void Home::storePreset()
{
    controller->queueEvent(E_STORE_PRESET);
}

void Home::updateAll()
{
    emit updateView();
    emit ptSpeedChanged();
    emit zoomSpeedChanged();
    emit transitionSpeedChanged();
    emit rampChanged();
    emit spp1Changed();
    emit spp2Changed();
    emit sppWaitTimeChanged();
    emit headNrChanged();
    emit presetMovingChanged();
    emit bounceChanged();
    emit sppStartChanged();
    emit fpmChanged();
    emit cameraConnectedChanged();
    emit serverConnectedChanged();
}

bool Home::presetMoving()
{
    return model->getCamFlag(PresetMoving);
}

bool Home::bounce()
{
    return model->getCamFlag(BounceEnabled);
}

bool Home::sppStart()
{
    return model->getCamFlag(SppEnabled);
}

bool Home::fpm()
{
    return model->getCamFlag(FastTransEnabled);
}

bool Home::cameraConnected()
{
    return model->getCamFlag(CameraConnected);
}

bool Home::serverConnected()
{
    return model->getServerStatus();
}

void Home::setBounce(bool bounce)
{
    model->setCamFlag(BounceEnabled,bounce);
}

void Home::setSppStart(bool sppStart)
{
    model->setCamFlag(SppEnabled,sppStart);
    sppStart ? controller->queueEvent(E_SPP_START) : controller->queueEvent(E_SPP_ABORT);
}

void Home::setFpm(bool fpm)
{
    model->setCamFlag(FastTransEnabled,fpm);
}

int Home::headNr()
{
    return model->getValue(Absolute,HeadNr);
}







