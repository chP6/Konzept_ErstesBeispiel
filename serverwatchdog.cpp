#include "serverwatchdog.h"

#include <stdint.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>

ServerWatchdog::ServerWatchdog()
{
    int time_err=0;
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);

    timeout.it_value.tv_sec = 1;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 1;
    timeout.it_interval.tv_nsec = 0;

    time_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (time_err<0)
        error(time_err,errno,"failed at creating timer");
}

void ServerWatchdog::processEvent(){
    // neue Speicherplatzallokation nicht zulässig (weil von anderem thread aufgerufen wird) -> alle Variablen in Klasse deklarieren.
    read(timer_fd, &timersElapsed, 8);
}
