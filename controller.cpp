#include "controller.h"
#include "model.h"
#include <unistd.h>
#include <thread>
#include "events.h"


Controller::Controller(Model& model)// : poller(*this)    //poller Konstruktor aufrufen -> erwartet Objekt (as reference) darum this dereferenzieren
{
    setModel(model);
}

void Controller::setModel(Model &model){
    this->model = &model;
}

void Controller::increment(int inc){
    int value = model->getData();
    value+=inc;
    model->setData(value);
}

void Controller::clear(){
    model->setData(0);
    usleep(2000*1000);
}

void Controller::setAxis(int x, int y){
    model->setAxis(x,y);
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
            break;
        case E_INCREASE:
            increment(loadedEvent.data.back());     //Last Element
            break;
        case E_SET_TILT:
            int x,y;
            y = loadedEvent.data.back();
            loadedEvent.data.pop_back();
            x = loadedEvent.data.back();
            setAxis(x,10000-y);
//            int x,y,tilt[4];
//            for (int i=3;i>=0;i--) {
//                tilt[i] = loadedEvent.data.back();
//                loadedEvent.data.pop_back();
//            }
//            x = ((uint8_t)tilt[0] && (uint8_t)tilt[1] << 8);
//            y = ((uint8_t)tilt[2] && (uint8_t)tilt[3] << 8);
//            setAxis(x,y);
            break;
        default:
            break;
        }
    }
}
