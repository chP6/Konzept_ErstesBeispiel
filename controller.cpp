#include "controller.h"
#include "model.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <errno.h>


Controller::Controller(Model& model)// : poller(*this)    //poller Konstruktor aufrufen -> erwartet Objekt (as reference) darum this dereferenzieren
{
    setModel(model);
    contr_err = udpInterface.init();
    if(contr_err<0){
        contr_err = errno;  //zwischenspeichern (muss)
        logSystemError(contr_err, "Could not initialize udp-interface");
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
            udpInterface.send(1, TILT_PAN, x, y);
            break;
        default:
            break;
        }
    }
}
