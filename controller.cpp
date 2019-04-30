#include "controller.h"
#include "model.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <errno.h>
#include "config.h"
#include <QDebug>

Controller::Controller(Model& model)// : poller(*this)    //poller Konstruktor aufrufen -> erwartet Objekt (as reference) darum this dereferenzieren
{
    setModel(model);
    contr_err = txSocket.init(9000);
    if(contr_err<0){
        contr_err = errno;  //zwischenspeichern (muss)
        logSystemError(contr_err, "Could not initialize udp-interface");
    }

    presetbus.setRow(UPPER_ROW);
    contr_err=presetbus.initSpi();
    if(contr_err<0){
        contr_err = errno;
        logSystemError(contr_err, "Could not open SPI-BUS 1");
    }

    camerabus.setRow(LOWER_ROW);
    camerabus.initSpi();
    if(contr_err<0){
        contr_err = errno;
        logSystemError(contr_err, "Could not open SPI-BUS 2");
    }
}

void Controller::setModel(Model &model){
    this->model = &model;
}

void Controller::increment(int inc){
    int value = model->getData();
    value+=inc;
    model->setData(value);
    //logError("Test Error");
}

void Controller::clear(){
    model->setData(0);
    usleep(2000*1000);
}

void Controller::setAxis(int x, int y){
    model->setAxis(x,y);
}

void Controller::logSystemError(int err_no, std::string msg){
    model->addError("(S) " + msg + " ERRNO: " + strerror(err_no));        //msg + string aus errno nummer
}

void Controller::logError(std::string msg){
    model->addError(msg);
}

void Controller::clearErrors(){
    model->clearErrors();
}






void Controller::queueEvent(int evt){
    eventQueue.qeueEvent(evt);
}

void Controller::queueEvent(int evt, std::vector<int> data){
    eventQueue.qeueEvent(evt, data);
}

void Controller::queueEvent(int evt, int singleData){
    std::vector<int> data;
    data.push_back(singleData);
    eventQueue.qeueEvent(evt, data);
}

void Controller::queueEvent(int evt, bool sta){
    eventQueue.qeueEvent(evt, sta);
}

void Controller::queueEvent(int evt, unsigned char number)
{
    eventQueue.qeueEvent(evt, number);
}


void Controller::startQueueProcessThread(){
    std::thread t1(&Controller::processQeue, this);       //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t1.detach();
}

void Controller::processQeue(){
    event_s loadedEvent;

    while(1){
        eventQueue.pullEvent(loadedEvent);      //blockiert, falls queue leer
        switch (loadedEvent.evt) {
        case E_CLEAR:
            clear();
            logError("Counter cleared!");
            break;
        case E_INCREASE:
            increment(loadedEvent.data.back());     //Last Element
            break;
        case E_SET_TILT:
            int x,y;
            x = loadedEvent.data[0];
            y = loadedEvent.data[1];
            setAxis(x,10000-y);
            txSocket.send(1, TILT_PAN, x, y);

            //debug
            char str[100];
            sprintf(str,"%d/%d",x,y);
            qDebug() << "Axis: " << str;

            break;
         case E_SET_ZOOM:
            txSocket.send(1,ZOOM_FOCUS_SET, loadedEvent.data[0]);
            break;
        case E_TX_WATCHDOG:
            txSocket.send(SERVER, WATCHDOG);
            model->setWatchdogWaitingflag(true);
            break;
        case E_RX_WATCHDOG:
            model->setWatchdogWaitingflag(false);   //clear flag
            break;
        case E_REQ_TEST:
            txSocket.request(1, FOCUS_SET_ABSOLUTE);
            logError("Sending Request!");
            break;
        case E_STORE_PRESET_DEBUG:
            txSocket.send(1, STORE_PRESET, loadedEvent.data[0]);
            logError("Store Preset!");
            break;
        case E_GOTO_PRESET:
            txSocket.send(1, GOTO_PRESET, loadedEvent.data[0]);
            break;
        case E_FOCUS_TEST:
            increment(loadedEvent.data[0]);
            //txSocket.send(1, FOCUS_SET_ABSOLUTE, model->getData());
            break;
        case E_STORE_PRESET:
            presetbus.setLed(ACT_PRESET_COLOR,model->getActivePreset());
            model->setCamFlag(PRST_IN_STORE,TRUE);
            presetbus.showStored(model->getUsedPreset(),model->getActivePreset());
            break;
        case E_PRESET_CHANGE:
            presetbus.setLed(PRESET_COLOR,loadedEvent.number);
            model->setActivePreset(loadedEvent.number);
            if(model->getCamFlag(PRST_IN_STORE)){
                model->setUsedPreset(loadedEvent.number);
                model->setCamFlag(PRST_IN_STORE,FALSE);
                txSocket.send(1, STORE_PRESET, loadedEvent.number+1);
            }else{
                txSocket.send(1, GOTO_PRESET, loadedEvent.number+1);
            }
            break;
        case E_CAMERA_CHANGE:
            camerabus.setLed(CAMERA_COLOR,loadedEvent.number);
            model->setActiveCamera(loadedEvent.number);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            break;

        default:
            break;
        }
    }
}
