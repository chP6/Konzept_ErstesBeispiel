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
    BBMJoystick joystick;               //for init debug joystick
private:
    [[noreturn]]void listener();
    struct pollfd poll_fd[21];
    int poll_err;
    int sense_val;
    signed char rotary_val;
    int xold,yold,zold;
    Controller* controller;
    ServerWatchdog srvWatchdog;

    Rotary rotary1, rotary2;
    Tastenfeld presetbus;
    Tastenfeld camerabus;
    ServerWatchdog xptWatchdog;
    ServerWatchdog autoSaveWatchdog;
    OCP ocp;
};

#endif // POLLER_H
