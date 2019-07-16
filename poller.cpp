#include "poller.h"
#include "controller.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <error.h>
#include "eventqueue.h"
#include "tastenfeld.h"
#include "config.h"
#include "input.h"
#include <QDebug>
#include <QTimer>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <vector>

Poller::Poller(Controller& controller)
{



    this->controller = &controller;
    poll_err = 0;

    /*Initialize hardware modules*/
    poll_err = srvWatchdog.init(1);
    if(poll_err < 0){
        poll_err = errno;
        controller.logSystemError(poll_err, "Could not initialize Watchdog Timer");
    }

//    poll_err = xptWatchdog.init(1);
//    if(poll_err < 0){
//        poll_err = errno;
//        controller.logSystemError(poll_err, "Could not initialize Watchdog Timer Xpt");
//    }

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

    joystick.initRead();
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


    /*Setup pollstruct*/
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

    /*Try to open ocp, if already connected at startup*/
    if(ocp.connect() != -1){
        poll_fd[7].fd = ocp.fd;
        poll_fd[7].events = POLLIN;
    }

    /*Presetbus buttons*/
    for (unsigned int i = 0; i < 6; ++i) {
        poll_fd[i+8].fd = presetbus.button[i];
        poll_fd[i+8].events = POLLPRI;
    }

    /*Camerabus buttons*/
    for (unsigned int i = 0; i < 6; ++i) {
        poll_fd[i+14].fd = camerabus.button[i];
        poll_fd[i+14].events = POLLPRI;
    }

    /*Autosave watchdog*/
    poll_fd[20].fd = autoSaveWatchdog.timer_fd;
    poll_fd[20].events = POLLIN;

    m_devices.push_back(new RotaryEncoder("/dev/input/by-path/platform-rotary@17-event"));
    m_devices.push_back(new Keyboard("/dev/input/by-path/platform-button@18-event", {
                                           /* key         event          data */
                                           { KEY_ENTER, { E_FAST_IRIS, { }}}}));
    m_devices.push_back(new Keyboard("/dev/input/by-path/platform-led-buttons-event", {
                                           /* key      event             data */
                                           { KEY_F1, { E_CAMERA_CHANGE, { 0 }}},
                                           { KEY_F2, { E_CAMERA_CHANGE, { 1 }}},
                                           { KEY_F3, { E_CAMERA_CHANGE, { 2 }}},
                                           { KEY_F4, { E_CAMERA_CHANGE, { 3 }}},
                                           { KEY_F5, { E_CAMERA_CHANGE, { 4 }}},
                                           { KEY_F6, { E_CAMERA_CHANGE, { 5 }}},
                                           { KEY_1,  { E_PRESET_CHANGE, { 0 }}},
                                           { KEY_2,  { E_PRESET_CHANGE, { 1 }}},
                                           { KEY_3,  { E_PRESET_CHANGE, { 2 }}},
                                           { KEY_4,  { E_PRESET_CHANGE, { 3 }}},
                                           { KEY_5,  { E_PRESET_CHANGE, { 4 }}},
                                           { KEY_6,  { E_PRESET_CHANGE, { 5 }}} }));

    int i = 21;
    for (InputDevice* d : m_devices)
        d->init(&poll_fd[i++]);
}

Poller::~Poller() {
    for (InputDevice* d : m_devices) delete d;
    m_devices.clear();
}


/*Starts listener function as seperate thread*/
void Poller::startListener(){
    applicationRunning = true;
    //std::thread t2(&Poller::listener, this); //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t3 = std::thread(&Poller::listener, this);
    //t2.detach();
}


/*Blocks until hardware interrupts occur*/
void Poller::listener(){
    std::vector<int> data;
    joystickData jsData;
    data.reserve(10);
// obsolete ??
//flush all interrupts
//    for (int i = 0;i<17;i++) {
//        if(i!=4)
//            poll_fd[i].revents = 0;
//    }


    /*Get rid of any undeterministic events from HW boot-up*/
    {
        rotary1.readSense();
        rotary1.readButton();
        rotary2.readSense();
        rotary2.readButton();

        for (int i=0;i<6;i++) {
            presetbus.readButton(i);
            camerabus.readButton(i);
        }
    }

    /*Endless loop*/
    while(applicationRunning){

        data.clear();
        /*Blocks until event occurs. -1 = infinite timeout*/
        poll_err = poll(poll_fd, sizeof(poll_fd) / sizeof(poll_fd[0]), -1);                      //poll. Blocks until event occurs -> SIZE setzen! -1 = infinite timeout

        if(poll_err<0){
            poll_err = errno;
            controller->logSystemError(poll_err, "Could not read Poller pollstruct");
        }

        /*Server watchdog interrupt*/
        if(poll_fd[0].revents & POLLIN) {
            poll_err = srvWatchdog.processEvent();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Watchdog Timer");
            }
            controller->queueEvent(E_TX_SERV_WATCHDOG);
        }

        /*Joystick interrupt*/
        if(poll_fd[1].revents & POLLIN) {
            poll_err = joystick.processEvent(jsData);
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Joystick");
            }

             /*Joystick Pushbutton actuated*/
            if (jsData.buttonVal > 0) {
                controller->queueEvent(E_AUTOFOCUS);
            }
            else{
                /*Only send event on change*/
                if(!(jsData.xCoord == xold && jsData.yCoord == yold)){
                    data.push_back(jsData.xCoord);
                    data.push_back(jsData.yCoord);
                    controller->queueEvent(E_SET_TILT, data);
                }

                /*Only send event on change*/
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


        /*Rotary 1 interrupt*/
        if(poll_fd[2].revents & POLLPRI){

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
        }

        if(poll_fd[3].revents & POLLPRI){                    // Rotary1 Button
            usleep(3000);

            poll_err = rotary1.readButton();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary1 button");
            }
            if(poll_err==0){ //otherwise it was a bounce
                controller->queueEvent(E_FAST_IRIS);

            }



        }

        /*Rotary 2 interrupt*/
        if(poll_fd[4].revents & POLLPRI){

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
            controller->queueEvent(E_FOCUS_CHANGE, rotary_val);
        }


        if(poll_fd[5].revents & POLLPRI){                    // Rotary2 Button
            usleep(3000);


            poll_err = rotary2.readButton();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Rotary2 button");
            }
            if(poll_err==0){
            controller->queueEvent(E_AUTOFOCUS);
            }
        }

        /*OCP connect interrupt on local loopback interface*/
        if(poll_fd[6].revents & POLLIN){
            ocp.closeFd();                     //close current fd (if any old open)

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
            /*Add entry at pollstruct*/
            poll_fd[7].fd = ocp.fd;
            poll_fd[7].events = POLLIN;
            controller->logError("OCP Connected!");
        }

        /*OCP interrupt*/
        if(poll_fd[7].revents & POLLIN){
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

        for (int i=0;i<6;i++) {
            /*Presetbus button interrupts*/
            if(poll_fd[8+i].revents & POLLPRI){
                usleep(DEBOUNCE_T);
                poll_err=presetbus.readButton(i);
                if (poll_err<0) {
                    poll_err = errno;
                    controller->logSystemError(poll_err, "Could not readout Presetbus Button " + std::to_string(i+1));
                }
                if(poll_err==0){
                    controller->queueEvent(E_PRESET_CHANGE,5-i);
                }
            }

            /*Camerabus button interrupts*/
            if(poll_fd[14+i].revents & POLLPRI){
                usleep(DEBOUNCE_T);
                poll_err=camerabus.readButton(i);
                if (poll_err<0) {
                    poll_err = errno;
                    controller->logSystemError(poll_err, "Could not readout Camerabus Button " + std::to_string(i+1));
                }
                if(poll_err==0){
                     controller->queueEvent(E_CAMERA_CHANGE,5-i);
                }
            }
        }


        /*
        //Presetbus button interrupts
        if(poll_fd[8].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=presetbus.readButton(0);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Presetbus Button 1");
            }
            if(poll_err==0){
                controller->queueEvent(E_PRESET_CHANGE,5);
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
                controller->queueEvent(E_PRESET_CHANGE,4);
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
                controller->queueEvent(E_PRESET_CHANGE,3);
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
                controller->queueEvent(E_PRESET_CHANGE,2);
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
                controller->queueEvent(E_PRESET_CHANGE,1);
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
                controller->queueEvent(E_PRESET_CHANGE,0);
            }
        }

        //Camerabus button interrupts
        if(poll_fd[14].revents & POLLPRI){
            usleep(DEBOUNCE_T);
            poll_err=camerabus.readButton(0);
            if (poll_err<0) {
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not readout Camerabus Button 1");
            }
            if(poll_err==0){
                 controller->queueEvent(E_CAMERA_CHANGE,5);
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
                controller->queueEvent(E_CAMERA_CHANGE,4);
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
                controller->queueEvent(E_CAMERA_CHANGE,3);
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
                controller->queueEvent(E_CAMERA_CHANGE,2);
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
                controller->queueEvent(E_CAMERA_CHANGE,1);
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
                controller->queueEvent(E_CAMERA_CHANGE,0);
            }
        }
        */

        /*Autosave watchdog event*/
        if(poll_fd[20].revents & POLLIN) {
            poll_err = autoSaveWatchdog.processEvent();
            if(poll_err<0){
                poll_err = errno;
                controller->logSystemError(poll_err, "Could not read Autosave Watchdog Timer");
            }
            controller->queueEvent(E_WRITE_AUTOSAVE);
        }

        for (InputDevice* d : m_devices) {
            std::vector<int> data;
            int event = d->getEvent(data);
            if (event != E_NULLEVENT)
                controller->queueEvent(event, data);
        }
    }
}

void Poller::stopListener()
{

    presetbus.~Tastenfeld();
    camerabus.~Tastenfeld();
    rotary1.~Rotary();
    rotary2.~Rotary();
    joystick.~BBMJoystick();
    autoSaveWatchdog.~ServerWatchdog();
    srvWatchdog.~ServerWatchdog();
    ocp.~OCP();
    applicationRunning = false;
}

