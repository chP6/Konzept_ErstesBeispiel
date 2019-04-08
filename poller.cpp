#include "poller.h"
#include "controller.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <error.h>
#include "eventqueue.h"

Poller::Poller(Controller& controller)
{
    this->controller = &controller;
    poll_err = 0;

    poll_fd[0].fd = srvWatchdog.timer_fd;               // poll struct setup
    poll_fd[0].events = POLLIN;
    poll_fd[1].fd = joystick.joystick_fd;
    poll_fd[1].events = POLLIN;
}

void Poller::listener(){
    std::vector<int> data;
    data.reserve(10);

    while(1){

        data.clear();
        poll_err = poll(poll_fd,2,-1);                      //poll. Blocks until event occurs
        if(poll_err<0){
            error(poll_err,errno, "fail at poll");
        }

        if(poll_fd[0].revents & POLLIN) {                    // Server watchdog event
            srvWatchdog.processEvent();

            // send ping to server
            data.push_back(1);
            controller->queueEvent(E_INCREASE, data);
            // receive answer
        }

        if(poll_fd[1].revents & POLLIN) {                   // Joystick event
            joystickData jsData = joystick.processEvent();

            if (jsData.button_val > 0) {
                controller->queueEvent(E_CLEAR,1);
                // send joystick button pushed event
            }
            else{
                data.push_back(jsData.debug[0]);
                data.push_back(jsData.debug[1]);
                controller->queueEvent(E_SET_TILT,data);

                /*for (int i=0;i<4;i++) {
                    data.push_back(jsData.tilt_val[i]);
                }
                controller->queueEvent(E_SET_TILT,data);*/    // send tilt
            }
        }
    }
}

void Poller::startListener(){
    std::thread t2(&Poller::listener, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t2.detach();
}
