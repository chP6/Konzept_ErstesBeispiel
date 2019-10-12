#include "others.h"


Others::Others()
{

}

Others::Others(Model* model, Controller* controller) {
    this->model = model;
    this->controller = controller;
}
Others::~Others()
{

}

QList<QVariant> Others::headPower()
{
    QList<QVariant> list;
    properties_t property = HeadPower;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

QList<QVariant> Others::mirror()
{
    QList<QVariant> list;
    properties_t property = Mirror;
    list.insert(Min,model->getMin(property));
    list.insert(Max,model->getMax(property));
    list.insert(Value,model->getValue(Absolute,property));
    list.insert(TextList,model->getTextList(property));
    list.insert(State,model->getDialState(property));
    return list;
}

void Others::setUpperLimit()
{
    controller->queueEvent(E_SET_UPPER_LIMIT);
}

void Others::setLowerLimit()
{
    controller->queueEvent(E_SET_LOWER_LIMIT);
}

void Others::calibrateHead()
{
    controller->queueEvent(E_CALIB_HEAD);
}

void Others::clearLimits()
{
    controller->queueEvent(E_CLEAR_LIMIT);
}

void Others::requestValues()
{
    controller->queueEvent(E_REQUEST_SETTINGS);
}

void Others::loadSaveFile()
{
    controller->queueEvent(E_LOAD_AUTOSAVE);
}

void Others::saveSaveFile()
{
    controller->queueEvent(E_WRITE_AUTOSAVE);
}

void Others::updateAll()
{
    emit  headPowerChanged();
    emit  mirrorChanged();
}

void Others::setColor(QColor color)
{
        m_color = color;
        qCDebug(logicIo)<< "Color Name" << color.name() << "Color hex" << QString::number(color.rgb(),16);
}




