#ifndef POLLER_H
#define POLLER_H

#include "serverwatchdog.h"
#include "bbmjoystick.h"
#include "rotary.h"
#include "tastenfeld.h"
#include "ocp.h"
#include <poll.h>
#include <vector>

class Controller;

class Poller
{
public:
    Poller(Controller& controller);
    void startListener();
private:
    void listener();
    struct pollfd poll_fd[20];
    int poll_err;
    int sense_val;
    signed char rotary_val;
    int xold,yold,zold;
    Controller* controller;
    ServerWatchdog srvWatchdog;    uint8_t buf[10];            //für antwort vom server. hier behalten?
    BBMJoystick joystick;
    Rotary rotary1, rotary2;
    Tastenfeld presetbus;
    Tastenfeld camerabus;
    OCP ocp;
};

#endif // POLLER_H
