#include "controller.h"
#include "model.h"
#include <unistd.h>
#include <thread>
#include "events.h"


Controller::Controller(Model& model)
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

void Controller::queueEvent(int evt, int val){
    eventQueue.qeueEvent(evt, val);
}

void Controller::queueEvent(int evt, bool sta){
    eventQueue.qeueEvent(evt, sta);
}


void Controller::startQueueProcessThread(){
    std::thread t1(&Controller::processQeue, this);       //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t1.detach();
}

void Controller::processQeue(){
    eventStruct loadedEvent;

    while(1){
        eventQueue.pullEvent(loadedEvent);      //blockiert, falls queue leer
        switch (loadedEvent.evt) {
        case E_CLEAR:
            clear();
            break;
        case E_INCREASE:
            increment(loadedEvent.val);
            break;
        default:
            break;
        }
    }
}
