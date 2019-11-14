#ifndef POLLER_H
#define POLLER_H

#include "watchdog.h"
#include "tastenfeld.h"
#include "hotplug.h"
#include <poll.h>
#include <vector>
#include <thread>
#include <list>
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
#include "logging.h"

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
    std::list<class Watchdog*> m_timers;
    Hotplug *m_hotplugobserver;

};

#endif // POLLER_H
