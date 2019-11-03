#ifndef POLLER_H
#define POLLER_H

#include "watchdog.h"
#include "rotary.h"
#include "tastenfeld.h"
#include "hotplug.h"
#include <poll.h>
#include <vector>
#include <thread>
#include <list>

class Controller;

class Poller
{
public:
    Poller(Controller& controller);
    ~Poller();
    void startListener();
    void listener();
    std::thread t3;
    void stopListener();
    bool applicationRunning=false;

private:
    struct pollfd *poll_fd;        // eleganter: nach Instanzierung aller Input-Devices dynamisch Speicher mit malloc() allozieren
    int poll_err;
    Controller* controller;
    std::list<class InputDevice*> m_devices;
    std::list<class InputDevice*> m_hotplugdevices;
    std::list<class Watchdog*> m_timers;
    Hotplug *m_hotplugobserver;

};

#endif // POLLER_H
