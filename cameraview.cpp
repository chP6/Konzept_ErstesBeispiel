#include "cameraview.h"


CameraView::CameraView()
{

}

CameraView::CameraView(Model* model, Controller* controller){
    this->model = model;
    this->controller = controller;
}

CameraView::~CameraView()
{

}

QList<QVariant> CameraView::getIris()
{
    QList<QVariant> list;
    properties_t property = Iris;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::blackLevel()
{
    QList<QVariant> list;
    properties_t property = Ped;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::gammaTable()
{
    QList<QVariant> list;
    properties_t property = GammaTable;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::gamma()
{
    QList<QVariant> list;
    properties_t property = Gamma;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::kneePoint()
{
    QList<QVariant> list;
    properties_t property = KneePoint;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::knee()
{
    QList<QVariant> list;
    properties_t property = Knee;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::saturation()
{
    QList<QVariant> list;
    properties_t property = Color;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::colorTemperature()
{
    QList<QVariant> list;
    properties_t property = ColorTemp;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::gain()
{
    QList<QVariant> list;
    properties_t property = Gain;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::detail()
{
    QList<QVariant> list;
    properties_t property = Detail;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::shutter()
{
    QList<QVariant> list;
    properties_t property = Shutter;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::ndFilter()
{
    QList<QVariant> list;
    properties_t property = NdFilter;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::redWhite()
{
    QList<QVariant> list;
    properties_t property = WhiteRed;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::blueWhite()
{
    QList<QVariant> list;
    properties_t property = WhiteBlue;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::redBlack()
{
    QList<QVariant> list;
    properties_t property = BlackRed;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> CameraView::blueBlack()
{
    QList<QVariant> list;
    properties_t property = BlackBlue;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

void CameraView::setValue(int property, int value)
{
    model->setValue(Absolute,Property(property),value);
    qDebug()<<value;
}

void CameraView::setField(int property)
{
    model->setRotaryField(properties_t(property),SEND);
}

void CameraView::updateAll()
{
    emit updateView();
    emit blackLevelChanged();
    emit irisChanged();
    emit gammaTableChanged();
    emit gammaChanged();
    emit kneePointChanged();
    emit kneeChanged();
    emit saturationChanged();
    emit colorTemperatureChanged();
    emit gainChanged();
    emit detailChanged();
    emit shutterChanged();
    emit ndFilterChanged();
    emit redWhiteChanged();
    emit blueWhiteChanged();
    emit redBlackChanged();
    emit blueBlackChanged();
}

void CameraView::onNewSignalReceived(int property)
{

}
