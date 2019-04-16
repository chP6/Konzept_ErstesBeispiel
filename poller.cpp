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

    poll_err = srvWatchdog.init();
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Watchdog Timer");
    }

    poll_err = joystick.init();
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Joystick");
    }

    poll_err = rotary1.init(4);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Rotary1");
    }

    poll_fd[0].fd = srvWatchdog.timer_fd;               // poll struct setup
    poll_fd[0].events = POLLIN;
    poll_fd[1].fd = joystick.joystick_fd;
    poll_fd[1].events = POLLIN;
    poll_fd[2].fd = rotary1.fd;
    poll_fd[2].events = POLLPRI;
}

void Poller::listener(){
    std::vector<int> data;
    data.reserve(10);

    while(1){

        data.clear();
        poll_err = poll(poll_fd,3,-1);                      //poll. Blocks until event occurs -> SIZE setzen! current = 3; -1 = infinite timeout
        if(poll_err<0){
            error(poll_err,errno, "fail at poll");
        }

        if(poll_fd[0].revents & POLLIN) {                    // Server watchdog event
            poll_err = srvWatchdog.processEvent();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Watchdog Timer");
            }

            // debug
            data.push_back(1);
            controller->queueEvent(E_INCREASE, data);
            controller->queueEvent(E_TX_WATCHDOG);
            // receive answer?
        }

        if(poll_fd[1].revents & POLLIN) {                   // Joystick event
            joystickData jsData;
            poll_err = joystick.processEvent(jsData);
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Joystick");
            }

            if (jsData.buttonVal > 0) {
                // send joystick button pushed event
                controller->queueEvent(E_CLEAR,1);
            }
            else{
                data.push_back(jsData.xCoord);
                data.push_back(jsData.yCoord);
                controller->queueEvent(E_SET_TILT,data);
            }
        }

        if(poll_fd[2].revents & POLLPRI){                    // Rotary1 event
            poll_err = rotary1.readSense(sense_val);
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary1 sense");
            }

            //debug
            char buf[100];
            sprintf(buf, "SenseVal: %d",sense_val);
            controller->logError("Rotary1 Sense!");
        }
    }
}

void Poller::startListener(){
    std::thread t2(&Poller::listener, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t2.detach();
}
