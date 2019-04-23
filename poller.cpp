#include "poller.h"
#include "controller.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <error.h>
#include "eventqueue.h"
#include "tastenfeld.h"
#include <QDebug>


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


    presetbus.setRow(LOWER_ROW);
    poll_err=presetbus.init();
    if(poll_err<0){
        poll_err=errno;
        controller.logSystemError(poll_err, "Presetbus: Couldn't init Button");
    }
    camerabus.setRow(UPPER_ROW);
    poll_err=camerabus.init();
    if(poll_err<0){
        poll_err=errno;
        controller.logSystemError(poll_err, "Camerabus: Couldn't init Button");
    }


    poll_fd[0].fd = srvWatchdog.timer_fd;               // poll struct setup
    poll_fd[0].events = POLLIN;
    poll_fd[1].fd = joystick.joystick_fd;
    poll_fd[1].events = POLLIN;
    poll_fd[2].fd = rotary1.fd;
    poll_fd[2].events = POLLPRI;

    for (unsigned int i = 0; i < 6; ++i) {

        poll_fd[i+3].fd = presetbus.button[i];
        poll_fd[i+3].events = POLLPRI;
    }

    for (unsigned int i = 0; i < 6; ++i) {

        poll_fd[i+9].fd = camerabus.button[i];
        poll_fd[i+9].events = POLLPRI;
    }


}

void Poller::listener(){
    std::vector<int> data;
    data.reserve(10);

    while(1){

        data.clear();
        poll_err = poll(poll_fd,15,-1);                      //poll. Blocks until event occurs -> SIZE setzen! current = 3; -1 = infinite timeout
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

        if(poll_fd[3].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            qDebug() << "Button1";
            poll_err=presetbus.readButton(0);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 1");
            }
            controller->queueEvent(E_PRESET_1);

        }

        if(poll_fd[4].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            qDebug() << "Button2";
            presetbus.readButton(1);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 2");
            }
            controller->queueEvent(E_PRESET_2);

        }

        if(poll_fd[5].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            presetbus.readButton(2);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 3");
            }
            controller->queueEvent(E_PRESET_3);
        }

        if(poll_fd[6].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            presetbus.readButton(3);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 4");
            }
            controller->queueEvent(E_PRESET_4);
        }

        if(poll_fd[7].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            presetbus.readButton(4);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 5");
            }
            controller->queueEvent(E_PRESET_5);
        }

        if(poll_fd[8].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            presetbus.readButton(5);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 6");
            }
            controller->queueEvent(E_PRESET_6);
        }

        if(poll_fd[9].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            qDebug() << "Button1C";
            camerabus.readButton(0);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 1");
            }
            controller->queueEvent(E_CAMERA_1);
        }

        if(poll_fd[10].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            camerabus.readButton(1);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 2");
            }
            controller->queueEvent(E_CAMERA_2);
        }

        if(poll_fd[11].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            camerabus.readButton(2);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 3");
            }
            controller->queueEvent(E_CAMERA_3);
        }

        if(poll_fd[12].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            camerabus.readButton(3);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 4");
            }
            controller->queueEvent(E_CAMERA_4);
        }

        if(poll_fd[13].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            camerabus.readButton(4);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 5");
            }
            controller->queueEvent(E_CAMERA_5);
        }

        if(poll_fd[14].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            camerabus.readButton(5);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 6");
            }
            controller->queueEvent(E_CAMERA_6);
        }


    }
}

void Poller::startListener(){
    std::thread t2(&Poller::listener, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t2.detach();
}
