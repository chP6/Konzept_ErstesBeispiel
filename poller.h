#ifndef POLLER_H
#define POLLER_H

#include "serverwatchdog.h"
#include "bbmjoystick.h"
#include "rotary.h"
#include "tastenfeld.h"
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
    struct pollfd poll_fd[15];
    int poll_err;
    int sense_val;
    Controller* controller;
    ServerWatchdog srvWatchdog;
    BBMJoystick joystick;
    Rotary rotary1;
    Tastenfeld presetbus;
    Tastenfeld camerabus;
};

#endif // POLLER_H
