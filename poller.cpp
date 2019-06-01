#include "poller.h"
#include "controller.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <error.h>
#include "eventqueue.h"
#include "tastenfeld.h"
#include "config.h"
#include <QDebug>
#include <QTimer>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

Poller::Poller(Controller& controller)
{

    // ================ INIT HW MODULES ======================================================================

    this->controller = &controller;
    poll_err = 0;

    poll_err = srvWatchdog.init(1);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Watchdog Timer");
    }

    poll_err = xptWatchdog.init(1);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Watchdog Timer Xpt");
    }

    poll_err = autoSaveWatchdog.init(10);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Autosave Timer");
    }

    poll_err = joystick.init();
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Joystick");
    }

    poll_err = rotary1.init(4, 16, 0x10);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Rotary1");
    }

    poll_err = rotary2.init(17, 20, 0x18);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Rotary2");
    }
  
      poll_err = ocp.initLoopbackInterface();
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize ocp loopback interface");
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


    // ================ INIT POLL STRUCT ======================================================================

    poll_fd[0].fd = srvWatchdog.timer_fd;
    poll_fd[0].events = POLLIN;
    poll_fd[1].fd = joystick.joystick_fd;
    poll_fd[1].events = POLLIN;
    poll_fd[2].fd = rotary1.fd_sense;
    poll_fd[2].events = POLLPRI;
    poll_fd[3].fd = rotary1.fd_button;
    poll_fd[3].events = POLLPRI;
    poll_fd[4].fd = rotary2.fd_sense;
    poll_fd[4].events = POLLPRI;
    poll_fd[5].fd = rotary2.fd_button;
    poll_fd[5].events = POLLPRI;
    poll_fd[6].fd = ocp.loopback_socket_fd;
    poll_fd[6].events = POLLIN;

    if(ocp.connect() != -1){        //try to open ocp, if already connected at startup
        poll_fd[7].fd = ocp.fd;
        poll_fd[7].events = POLLIN;
    }

    for (unsigned int i = 0; i < 6; ++i) {
        poll_fd[i+8].fd = presetbus.button[i];
        poll_fd[i+8].events = POLLPRI;
    }

    for (unsigned int i = 0; i < 6; ++i) {
        poll_fd[i+14].fd = camerabus.button[i];
        poll_fd[i+14].events = POLLPRI;
    }

    poll_fd[20].fd = autoSaveWatchdog.timer_fd;
    poll_fd[20].events = POLLIN;

}


void Poller::listener(){
    std::vector<int> data;
    joystickData jsData;
    data.reserve(10);

//flush all interrupts
//    for (int i = 0;i<17;i++) {
//        if(i!=4)
//            poll_fd[i].revents = 0;
//    }


    // get rid of any undeterministic events from HW init
    rotary1.readSense();
    rotary1.readButton();
    rotary2.readSense();
    rotary2.readButton();

//    for (int i=0;i<10;i++) {
//        joystick.processEvent(jsData);
//    }

    for (int i=0;i<6;i++) {
        presetbus.readButton(i);
        camerabus.readButton(i);
    }

    while(1){

        data.clear();
        poll_err = poll(poll_fd,21,-1);                      //poll. Blocks until event occurs -> SIZE setzen! -1 = infinite timeout

        if(poll_err<0){
            poll_err = errno;
            controller->logSystemError(poll_err, "Could not read Poller pollstruct");
        }

        if(poll_fd[0].revents & POLLIN) {                    // Server watchdog event
            poll_err = srvWatchdog.processEvent();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Watchdog Timer");
            }
            controller->queueEvent(E_TX_SERV_WATCHDOG);
        }

        if(poll_fd[1].revents & POLLIN) {                   // Joystick event
            poll_err = joystick.processEvent(jsData);
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Joystick");
            }

            if (jsData.buttonVal > 0) {                     // Joystick Pushbutton
                controller->queueEvent(E_AUTOFOCUS);
            }
            else{
                //reducing number of necessary events
                if(!(jsData.xCoord == xold && jsData.yCoord == yold)){
                    data.push_back(jsData.xCoord);
                    data.push_back(jsData.yCoord);
                    controller->queueEvent(E_SET_TILT, data);
                }

                if(!(jsData.zCoord == zold)){
                    data.clear();
                    data.push_back(jsData.zCoord);
                    controller->queueEvent(E_SET_ZOOM, data);
                }

                xold=jsData.xCoord;
                yold=jsData.yCoord;
                zold=jsData.zCoord;
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
            qDebug("Rot Val: %d", rotary_val);
        }

        if(poll_fd[3].revents & POLLPRI){                    // Rotary1 Button
            usleep(DEBOUNCE_T);

            poll_err = rotary1.readButton();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary1 button");
            }
            if(poll_err==0){ //otherwise it was a bounce
                //controller->queueEvent(E_WRITE_SAVEFILE);       //debug
                controller->queueEvent(E_FAST_IRIS);

            }



        }

        if(poll_fd[4].revents & POLLPRI){                    // Rotary2 event

            poll_err = rotary2.readSense();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary2 sense");
            }

            poll_err = rotary2.readRotary(rotary_val);
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary2 value");
            }
            qDebug()<<rotary_val;
            controller->queueEvent(E_FOCUS_CHANGE, rotary_val);
        }

        if(poll_fd[5].revents & POLLPRI){                    // Rotary2 Button

            //todo proper debounce
            poll_err = rotary2.readButton();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary2 button");
            }
            if(poll_err==0){
            controller->queueEvent(E_LOAD_SAVEFILE);
            }
        }


        if(poll_fd[6].revents & POLLIN){                    // OCP connected event
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
            poll_fd[7].fd = ocp.fd;
            poll_fd[7].events = POLLIN;
            controller->logError("OCP Connected!");
        }


        if(poll_fd[7].revents & POLLIN){                    // OCP event
            int ocpEvent;
            poll_err = ocp.processEvent(ocpEvent);
            if (poll_err < 0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read OCP");
            }

            if(ocpEvent != -1){     //-1 are other nonimportant hid events
                switch (ocpEvent) {
                case OCP_E_IRIS_UP:
                    controller->queueEvent(E_IRIS_CHANGE,1);
                    break;
                case OCP_E_IRIS_DOWN:
                    controller->queueEvent(E_IRIS_CHANGE,-1);
                    break;
                case OCP_E_PED_UP:
                    controller->queueEvent(E_PED_CHANGE,1);
                    break;
                case OCP_E_PED_DOWN:
                    controller->queueEvent(E_PED_CHANGE,-1);
                    break;
                case OCP_E_WRED_UP:
                    controller->queueEvent(E_WRED_CHANGE,1);
                    break;
                case OCP_E_WRED_DOWN:
                    controller->queueEvent(E_WRED_CHANGE,-1);
                    break;
                case OCP_E_WBLUE_UP:
                    controller->queueEvent(E_WBLUE_CHANGE,1);
                    break;
                case OCP_E_WBLUE_DOWN:
                    controller->queueEvent(E_WBLUE_CHANGE,-1);
                    break;
                case OCP_E_BRED_UP:
                    controller->queueEvent(E_BRED_CHANGE,1);
                    break;
                case OCP_E_BRED_DOWN:
                    controller->queueEvent(E_BRED_CHANGE,-1);
                    break;
                case OCP_E_BBLUE_UP:
                    controller->queueEvent(E_BBLUE_CHANGE,1);
                    break;
                case OCP_E_BBLUE_DOWN:
                    controller->queueEvent(E_BBLUE_CHANGE,-1);
                    break;
                case OCP_E_GAIN_UP:
                    controller->queueEvent(E_GAIN_CHANGE,1);
                    break;
                case OCP_E_GAIN_DOWN:
                    controller->queueEvent(E_GAIN_CHANGE,-1);
                    break;
                case OCP_E_SHUTTER_UP:
                    controller->queueEvent(E_SHUTTER_CHANGE,1);
                    break;
                case OCP_E_SHUTTER_DOWN:
                    controller->queueEvent(E_SHUTTER_CHANGE,-1);
                    break;
                default:
                    break;
                }

                //debug:
                //controller->logError("OCP Event: " + std::to_string(ocpEvent));
            }
        }

        if(poll_fd[8].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=presetbus.readButton(0);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 1");
            }
            if(poll_err==0){
                controller->queueEvent(E_PRESET_CHANGE,0);
            }
        }

        if(poll_fd[9].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=presetbus.readButton(1);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 2");
            }
            if(poll_err==0){
                controller->queueEvent(E_PRESET_CHANGE,1);
            }
        }

        if(poll_fd[10].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=presetbus.readButton(2);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 3");
            }
            if(poll_err==0){
                controller->queueEvent(E_PRESET_CHANGE,2);
            }
        }

        if(poll_fd[11].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=presetbus.readButton(3);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 4");
            }
            if(poll_err==0){
                controller->queueEvent(E_PRESET_CHANGE,3);
            }
        }

        if(poll_fd[12].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=presetbus.readButton(4);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 5");
            }
            if(poll_err==0){
                controller->queueEvent(E_PRESET_CHANGE,4);
            }
        }

        if(poll_fd[13].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=presetbus.readButton(5);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 6");
            }
            if(poll_err==0){
                controller->queueEvent(E_PRESET_CHANGE,5);
            }
        }

        if(poll_fd[14].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=camerabus.readButton(0);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 1");
            }
            if(poll_err==0){
                 controller->queueEvent(E_CAMERA_CHANGE,0);
            }
        }

        if(poll_fd[15].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=camerabus.readButton(1);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 2");
            }
            if(poll_err==0){
                controller->queueEvent(E_CAMERA_CHANGE,1);
            }
        }

        if(poll_fd[16].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=camerabus.readButton(2);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 3");
            }
            if(poll_err==0){
                controller->queueEvent(E_CAMERA_CHANGE,2);
            }
        }

        if(poll_fd[17].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=camerabus.readButton(3);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 4");
            }
            if(poll_err==0){
                controller->queueEvent(E_CAMERA_CHANGE,3);
            }
        }

        if(poll_fd[18].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=camerabus.readButton(4);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 5");
            }
            if(poll_err==0){
                controller->queueEvent(E_CAMERA_CHANGE,4);
            }
     }

        if(poll_fd[19].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=camerabus.readButton(5);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 6");
            }
            if(poll_err==0){
                controller->queueEvent(E_CAMERA_CHANGE,5);
            }
        }

        if(poll_fd[20].revents & POLLIN) {                    // Autosave watchdog event
            poll_err = autoSaveWatchdog.processEvent();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Autosave Watchdog Timer");
            }
            controller->queueEvent(E_WRITE_AUTOSAVE);
        }

    }
}


void Poller::startListener(){
    std::thread t2(&Poller::listener, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t2.detach();
}
