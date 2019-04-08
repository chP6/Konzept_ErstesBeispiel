#ifndef SERVERWATCHDOG_H
#define SERVERWATCHDOG_H

#include <sys/time.h>
#include <sys/timerfd.h>
#include <stdint.h>

class ServerWatchdog
{
public:
    ServerWatchdog();
    void processEvent();
    int timer_fd;
private:
    struct itimerspec timeout;
    int timersElapsed;
    uint8_t buf[10];            //für antwort vom server. hier behalten?
};


#endif // SERVERWATCHDOG_H
