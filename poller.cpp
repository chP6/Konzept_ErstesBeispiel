#define LEGACY_HARDWARE

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
#include "logging.h"
#include <libudev.h>


Poller::Poller(Controller& controller)
{

    this->controller = &controller;
    poll_err = 0;

    /*Initialize hardware modules*/
    m_timers.push_back(new Watchdog(1, E_TX_SERV_WATCHDOG));
    m_timers.push_back(new Watchdog(10,E_WRITE_AUTOSAVE));
#ifdef LEGACY_HARDWARE
    m_devices.push_back(new Keyboard("/dev/input/by-path/platform-led_buttons_extra-event",keyboardmap));
    m_devices.push_back(new I2cRotaryEncoder("/dev/input/by-path/platform-rotary-sense-event",
                    {{ KEY_F9, { E_INCREASE, { 0x10 }}},
                     { KEY_F10, { E_FOCUS_CHANGE, { 0x18 }}}}));
    m_devices.push_back(new Keyboard("/dev/input/by-path/platform-button@10-event", {{ KEY_ENTER, { E_FAST_IRIS, { }}}}));
#else
    m_devices.push_back(new Keyboard("/dev/input/by-path/platform-led-buttons-event",keyboardmap));
    m_devices.push_back(new RotaryEncoder("/dev/input/by-path/platform-rotary@17-event"));
    m_devices.push_back(new Keyboard("/dev/input/by-path/platform-button@18-event", {
                                           /* key         event          data */
                                            { KEY_ENTER, { E_FAST_IRIS, { }}}}));
#endif
    m_devices.push_back(new XYZJoystick("/dev/input/by-id/usb-CH_Products_APEM_HF_Joystick-event-joystick"));
    m_devices.push_back(new ZoomFocusJoystick("/dev/input/by-id/usb-Adafruit_LLC_Adafruit_ItsyBitsy_M4_HIDAC-event-if02"));
    m_devices.push_back(new UsbOcp("/dev/input/by-id/usb-Mitsumi_Electric_Apple_Extended_USB_Keyboard-event-kbd",ocpmap));

    poll_fd = (struct pollfd* )malloc(sizeof (struct pollfd) * (m_devices.size()+m_timers.size()));
    int i = 0;
    for (InputDevice* d : m_devices)
        d->init(&poll_fd[i++]);
    for (Watchdog* t : m_timers)
        t->init(&poll_fd[i++]);

}

Poller::~Poller() {
    for (InputDevice* d : m_devices) delete d;
    m_devices.clear();
    for(Watchdog* t : m_timers) delete t;
    m_timers.clear();
    free(poll_fd);
}


/*Starts listener function as seperate thread*/
void Poller::startListener(){
    applicationRunning = true;
    t3 = std::thread(&Poller::listener, this);
}


/*Blocks until hardware interrupts occur*/
void Poller::listener(){


    /*Endless loop*/
    while(applicationRunning){

        /*Blocks until event occurs. -1 = infinite timeout*/
        if(poll(poll_fd, m_devices.size()+m_timers.size(), -1) < 0){    //poll. Blocks until event occurs -> SIZE setzen! -1 = infinite timeout
            poll_err = errno;
            controller->logSystemError(poll_err, "Could not read Poller pollstruct");
        }

        for (Watchdog* t : m_timers){
            int event = t->getEvent();
            if (event != E_NULLEVENT) {
                controller->queueEvent(event);
            }
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
    applicationRunning = false;
}

