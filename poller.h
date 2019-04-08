#ifndef POLLER_H
#define POLLER_H

#include "serverwatchdog.h"
#include "bbmjoystick.h"
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
    Controller* controller;
    ServerWatchdog srvWatchdog;
    BBMJoystick joystick;
};

#endif // POLLER_H
