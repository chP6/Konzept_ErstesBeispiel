#ifndef POLLER_H
#define POLLER_H

#include "serverwatchdog.h"
#include "bbmjoystick.h"
#include "rotary.h"
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
    struct pollfd poll_fd[3];
    int poll_err;
    int sense_val;
    Controller* controller;
    ServerWatchdog srvWatchdog;
    BBMJoystick joystick;
    Rotary rotary1;
};

#endif // POLLER_H
