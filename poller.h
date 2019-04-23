#ifndef POLLER_H
#define POLLER_H

#include "serverwatchdog.h"
#include "bbmjoystick.h"
#include "rotary.h"
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
    struct pollfd poll_fd[5];
    int poll_err;
    int sense_val;
    signed char rotary_val;
    Controller* controller;
    ServerWatchdog srvWatchdog;
    BBMJoystick joystick;
    Rotary rotary1;
    OCP ocp;
};

#endif // POLLER_H
