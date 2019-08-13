#ifndef SERVERWATCHDOG_H
#define SERVERWATCHDOG_H

#include <sys/time.h>
#include <sys/timerfd.h>
#include <stdint.h>

class ServerWatchdog
{
public:
    ServerWatchdog();
    ~ServerWatchdog();
    int processEvent();
    int init(int interval_s);
    int timer_fd;
private:
    struct itimerspec timeout;
    int timersElapsed;
    int wdg_err;
};


#endif // SERVERWATCHDOG_H
