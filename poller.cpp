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

    poll_err = rotary1.init(4, 0x10);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Rotary1");
    }

    poll_err = ocp.initLoopbackInterface();
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize ocp loopback interface");
    }


    poll_fd[0].fd = srvWatchdog.timer_fd;               // poll struct setup
    poll_fd[0].events = POLLIN;
    poll_fd[1].fd = joystick.joystick_fd;
    poll_fd[1].events = POLLIN;
    poll_fd[2].fd = rotary1.fd_sense;
    poll_fd[2].events = POLLPRI;
    poll_fd[3].fd = ocp.loopback_socket_fd;
    poll_fd[3].events = POLLIN;

    if(ocp.connect() != -1){    //try to open ocp, if already connected at startup
        poll_fd[4].fd = ocp.fd;
        poll_fd[4].events = POLLIN;
    }
}

void Poller::listener(){
    std::vector<int> data;
    data.reserve(10);

    while(1){

        data.clear();
        poll_err = poll(poll_fd,5,-1);                      //poll. Blocks until event occurs -> SIZE setzen! current = 3; -1 = infinite timeout
        if(poll_err<0){
            error(poll_err,errno, "fail at poll");
        }

        if(poll_fd[0].revents & POLLIN) {                    // Server watchdog event
            poll_err = srvWatchdog.processEvent();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Watchdog Timer");
            }

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
                controller->queueEvent(E_REQ_TEST);
            }
            else{
                data.push_back(jsData.xCoord);
                data.push_back(jsData.yCoord);
                controller->queueEvent(E_SET_TILT, data);

                data.clear();
                data.push_back(jsData.zCoord);
                controller->queueEvent(E_SET_ZOOM, data);
            }
        }


        if(poll_fd[2].revents & POLLPRI){                    // Rotary1 event

            poll_err = rotary1.readSense();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary1 sense");
            }

            poll_err = rotary1.readRotary(rotary_val);
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary1 value");
            }
            controller->queueEvent(E_INCREASE, rotary_val);
            switch(rotary_val){
            case 1:
                controller->queueEvent(E_STORE_PRESET);
                break;
            case -1:
                controller->queueEvent(E_GOTO_PRESET);
                break;
            }
        }


        if(poll_fd[3].revents & POLLIN){                    // OCP connected event
            ocp.closeFd();      //close current fd (if any old open)

            poll_err = ocp.receiveLoopback();
            if (poll_err < 0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read OCP loopback interface");
            }

            poll_err = ocp.connect();
            if (poll_err < 0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not open OCP");
            }
            poll_fd[4].fd = ocp.fd;
            poll_fd[4].events = POLLIN;
            controller->logError("OCP Connected!");
        }


        if(poll_fd[4].revents & POLLIN){                    // OCP event
            int ocpEvent;
            poll_err = ocp.processEvent(ocpEvent);
            if (poll_err < 0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read OCP");
            }

            if(ocpEvent != -1){     //-1 if other nonimportant hid events
                //controller->logError("OCP Event: " + std::to_string(ocpEvent));

                //debug: focus test
                if(ocpEvent == 1 || ocpEvent == 2){
                    int inc = 10;
                    if(ocpEvent == 2) inc=-10;
                    controller->queueEvent(E_FOCUS_TEST, inc);
                }

            }
        }
    }
}

void Poller::startListener(){
    std::thread t2(&Poller::listener, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t2.detach();
}
