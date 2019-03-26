#include "poller.h"
#include "controller.h"
#include <unistd.h>
#include <thread>
#include "events.h"

Poller::Poller(Controller& controller)
{
    this->controller = &controller;
}

void Poller::metronom(){
    while(1){
        usleep(1000*1000);
        controller->queueEvent(E_INCREASE, 1);
    }
}

void Poller::startMetronom(){
    std::thread t1(&Poller::metronom, this);       //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t1.detach();
}
