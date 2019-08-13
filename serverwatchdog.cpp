#include "serverwatchdog.h"

#include <stdint.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include "config.h"

ServerWatchdog::ServerWatchdog()
{
}

ServerWatchdog::~ServerWatchdog()
{
    close(timer_fd);
}

/*Initialization*/
int ServerWatchdog::init(int interval_s){
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);

    timeout.it_value.tv_sec = interval_s;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = interval_s;
    timeout.it_interval.tv_nsec = 0;

    wdg_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (wdg_err<0){
        //error(time_err,errno,"failed at creating timer");
        return -1;
    }
    return 0;
}

/*Read timer*/
int ServerWatchdog::processEvent(){
    // neue Speicherplatzallokation nicht zulässig (weil von anderem thread aufgerufen wird) -> alle Variablen in Klasse als Attribute deklarieren.
    wdg_err = read(timer_fd, &timersElapsed, 8);
    return wdg_err;
}
