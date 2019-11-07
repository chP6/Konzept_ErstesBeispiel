#include "backend.h"

Backend::Backend()
{

}

Backend::Backend(Model *model, Controller *controller)
    : m_model(model), m_controller(controller) {}

Backend::~Backend()
{

}

void Backend::update()
{
   /* emit all signals */
    QMetaMethod method;
    QMetaObject meta = *this->metaObject();
    for(int i = meta.methodOffset(); i< meta.methodCount(); i++){
      method = meta.method(i);
     QMetaMethod::MethodType type =  method.methodType();
     if(type == QMetaMethod::Signal){
          method.invoke(this);
     }
    }
}

QList<QVariant> Backend::getDialValues(properties_t property)
{
    QList<QVariant> list;
    list.insert(Min,m_model->getMin(property));
    list.insert(Max,m_model->getMax(property));
    list.insert(Value,m_model->getValue(Absolute,property));
    list.insert(TextList,m_model->getTextList(property));
    list.insert(State,m_model->getDialState(property));
    return list;
}
/********************************************************************************************/
Home::Home(){}
Home::Home(Model *model, Controller *controller) : Backend(model, controller){}

int Home::headNr(){return m_model->getValue(Absolute,HeadNr);}
/********************************************************************************************/
CameraView::CameraView(){}
CameraView::CameraView(Model* model, Controller* controller) : Backend(model, controller){}
/********************************************************************************************/
XptControl::XptControl(){}
XptControl::XptControl(Model* model, Controller* controller)  : Backend(model, controller){};

QStringList XptControl::inputLabels()
{
    QList<QString> list = m_model->getXptInputLables();
    if(list.empty()){
        for (int i = 0;i < m_model->getXptNumberOfInputs();i++) {
            list.append(QString::number(i+1));
        }
        return QStringList(list);
    }
    return QStringList(list);
}

QList<int> XptControl::xptSources()
{
    QList<int> list;
    for(int i = 0; i < NUMBER_OF_SLOTS; i++) list.append(m_model->getXptSlotSource(i));
    return list;
}

QStringList XptControl::outputLabels()
{
    QList<QString> list = m_model->getXptOutputLables();
    if(list.empty()){
        for (int i = 0;i < m_model->getXptNumberOfOutputs();i++) {
            list.append(QString::number(i+1,10));
        }
        return QStringList(list);
    }
    return QStringList(list);
}

int XptControl::xptDestination()
{
   return m_model->getXptDestination();
}

QStringList XptControl::ipAdress()
{
    QString ipAdress = m_model->getXptIpAdress();
    QStringList list = ipAdress.split(".");
    return list;
}

void XptControl::connectXpt(bool checked)
{
    m_model->setXptEnabled(checked);
    m_controller->queueEvent(E_XPT_CONNECT);
}

bool XptControl::connected()
{
   return m_model->getXptConnected();
}

int XptControl::xptType()
{
    return m_model->getXptType();
}

void XptControl::setXptSources(QList<int> xptSources)
{
    for (int i = 0;i < NUMBER_OF_SLOTS;i++) {m_model->setXptSlotSource(i,xptSources.first()); xptSources.pop_front();}
    emit xptSourcesChanged();
}

void XptControl::setXptDestination(int xptDestination)
{
    m_model->setXptDestination(xptDestination);
    emit xptDestinationChanged();
}

void XptControl::setIpAdress(QStringList ipAdress)
{
    QString xptIpAdress = ipAdress.join(".");
    m_model->setXptIpAdress(xptIpAdress);
    emit ipAdressChanged();
}

void XptControl::setXptType(int xptType)
{
    m_model->setXptType(xptType);
}
/********************************************************************************************/
Controls::Controls(){}
Controls::Controls(Model* model, Controller* controller) : Backend(model, controller){}
/********************************************************************************************/
Others::Others(){m_errors.clear();}
Others::Others(Model* model, Controller* controller) : Backend(model,controller){}

QString Others::errors(){
    QStringList errors;
    static FILE * fp;
    char *line = nullptr;
    size_t len = 0;
    fp = fopen(".config/"+QCoreApplication::organizationName().toLocal8Bit()+"/log.txt", "r" );
    if(fp){
    while ( getline(&line, &len, fp) != -1) {
        errors.insert(0,line);
       }

    fclose(fp);
    }
    if (line)
       free(line);
    return errors.join("\n");
}

void Others::update()
{
    void headPowerChanged();
    void mirrorChanged();
}
