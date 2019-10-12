#include "xptcontrol.h"

XptControl::XptControl()
{

}

XptControl::XptControl(Model* model, Controller* controller) {

    this->model = model;
    this->controller = controller;
}

XptControl::~XptControl()
{

}

QStringList XptControl::inputLabels()
{
    QList<QString> list = model->getXptInputLables();

    if(list.empty()){
        for (int i = 0;i < model->getXptNumberOfInputs();i++) {
            list.append(QString::number(i+1));
        }
        return QStringList(list);
    }

    return QStringList(list);
}

QList<int> XptControl::xptSources()
{
    QList<int> list;
    for(int i = 0; i < NUMBER_OF_SLOTS; i++) list.append(model->getXptSlotSource(i));
    m_xptSources = list;
    return list;
}

QStringList XptControl::outputLabels()
{
    QList<QString> list = model->getXptOutputLables();
    if(list.empty()){
        for (int i = 0;i < model->getXptNumberOfOutputs();i++) {
            list.append(QString::number(i+1,10));
        }
        return QStringList(list);
    }
    return QStringList(list);
}

int XptControl::xptDestination()
{
    m_xptDestination = model->getXptDestination();
    return m_xptDestination;
}

QStringList XptControl::ipAdress()
{
    QString ipAdress = model->getXptIpAdress();
    QStringList list = ipAdress.split(".");
    return list;
}

void XptControl::connectXpt(bool checked)
{
    model->setXptEnabled(checked);
    controller->queueEvent(E_XPT_CONNECT);
}

void XptControl::updateAll()
{
    emit inputLabelsChanged();
    emit xptSourcesChanged();
    emit outputLabelsChanged();
    emit xptDestinationChanged();
    emit ipAdressChanged();
    emit xptTypeChanged();
}

bool XptControl::connected()
{

        m_connected = model->getXptConnected();
        return m_connected;

}

int XptControl::xptType()
{
    return model->getXptType();
}

void XptControl::setXptSources(QList<int> xptSources)
{
    if (m_xptSources == xptSources)
        return;

    m_xptSources = xptSources;
    for (int i = 0;i < NUMBER_OF_SLOTS;i++) {model->setXptSlotSource(i,xptSources.first()); xptSources.pop_front();}
    emit xptSourcesChanged();

}

void XptControl::setXptDestination(int xptDestination)
{
    if (m_xptDestination == xptDestination)
        return;

    m_xptDestination = xptDestination;
    model->setXptDestination(xptDestination);
   // emit xptDestinationChanged(m_xptDestination);

}

void XptControl::setIpAdress(QStringList ipAdress)
{
    QString xptIpAdress = ipAdress.join(".");
    model->setXptIpAdress(xptIpAdress);
    emit ipAdressChanged();
}

void XptControl::setXptType(int xptType)
{
    model->setXptType(xptType);
}


