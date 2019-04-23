#include "controller.h"
#include "model.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <errno.h>

#define PRESET_COLOR 0xFFFFFF
#define CAMERA_COLOR 0xFF00F0
#define ACT_PRESET_COLOR 0x00FF00

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


void Controller::startQueueProcessThread(){
    std::thread t1(&Controller::processQeue, this);       //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t1.detach();
}

void Controller::processQeue(){
    Event_s loadedEvent;

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
            break;
        case E_TX_WATCHDOG:
            txSocket.send(0, WATCHDOG);
            break;
        case E_PRESET_1:
            presetbus.setLed(PRESET_COLOR,0);
            model->setActivePreset(0);
            if(model->getPresetInStore()){
                model->setUsedPreset(0);
                model->setPresetInStore(0);
                txSocket.send(1,STORE_PRESET,1);
            }else{
                txSocket.send(1,GOTO_PRESET,1);
            }
            break;
        case E_PRESET_2:
            presetbus.setLed(PRESET_COLOR,1);
            model->setActivePreset(1);
            if(model->getPresetInStore()){
                model->setUsedPreset(1);
                model->setPresetInStore(0);
                txSocket.send(1,STORE_PRESET,2);
            }else{
                txSocket.send(1,GOTO_PRESET,2);
            }
            break;
        case E_PRESET_3:
            presetbus.setLed(PRESET_COLOR,2);
            model->setActivePreset(2);
            if(model->getPresetInStore()){
                model->setUsedPreset(2);
                model->setPresetInStore(0);
                txSocket.send(1,STORE_PRESET,3);
            }else{
                txSocket.send(1,GOTO_PRESET,3);
            }
            break;
        case E_PRESET_4:
            presetbus.setLed(PRESET_COLOR,3);
            model->setActivePreset(3);
            if(model->getPresetInStore()){
                model->setUsedPreset(3);
                model->setPresetInStore(0);
                txSocket.send(1,STORE_PRESET,4);
            }else{
                txSocket.send(1,GOTO_PRESET,4);

            }
            break;
        case E_PRESET_5:
            presetbus.setLed(PRESET_COLOR,4);
            model->setActivePreset(4);
            if(model->getPresetInStore()){
                model->setUsedPreset(4);
                model->setPresetInStore(0);
                txSocket.send(1,STORE_PRESET,5);
            }else{
                txSocket.send(1,GOTO_PRESET,5);

            }
            break;
        case E_PRESET_6:
            presetbus.setLed(PRESET_COLOR,5);
            model->setActivePreset(5);
            if(model->getPresetInStore()){
                model->setUsedPreset(5);
                model->setPresetInStore(0);
                txSocket.send(1,STORE_PRESET,6);
            }else{
                txSocket.send(1,GOTO_PRESET,6);
            }
            break;
        case E_CAMERA_1:
            camerabus.setLed(CAMERA_COLOR,0);
            model->setActiveCamera(0);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            break;
        case E_CAMERA_2:
            camerabus.setLed(CAMERA_COLOR,1);
            model->setActiveCamera(1);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            break;
        case E_CAMERA_3:
            camerabus.setLed(CAMERA_COLOR,2);
            model->setActiveCamera(2);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            break;
        case E_CAMERA_4:
            camerabus.setLed(CAMERA_COLOR,3);
            model->setActiveCamera(3);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            break;
        case E_CAMERA_5:
            camerabus.setLed(CAMERA_COLOR,4);
            model->setActiveCamera(4);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            break;
        case E_CAMERA_6:
            camerabus.setLed(CAMERA_COLOR,5);
            model->setActiveCamera(5);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            break;
        case E_STORE_PRESET:
            presetbus.setLed(ACT_PRESET_COLOR,model->getActivePreset());
            model->setPresetInStore(1);
            presetbus.showStored(model->getUsedPreset());

            break;
        default:
            break;
        }
    }
}
