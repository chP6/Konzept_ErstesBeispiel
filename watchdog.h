#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <sys/time.h>
#include <sys/timerfd.h>
#include <stdint.h>
#include <poll.h>
#include <errno.h>
#include <QDebug>
#include <error.h>
#include <unistd.h>
#include "config.h"

class Watchdog
{
public:
    Watchdog(int period, int event);
    virtual ~Watchdog();
    virtual int init(struct pollfd *fd);
    virtual int getEvent();


private:
    bool eventReceived();
    bool readTimer();
    struct itimerspec m_timeout;
    int timersElapsed;
    struct pollfd* m_fd;
    int m_period;
    int m_event;
};


#endif // SERVERWATCHDOG_H
