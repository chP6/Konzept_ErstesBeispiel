#ifndef GENERICTIMER_H
#define GENERICTIMER_H

#include <poll.h>
#include <sys/timerfd.h>

class Controller;        //forward declaration

class GenericTimer
{
public:
    GenericTimer();
    int init(int interval_us, int command, Controller& controller);
    void start();
    void stop();
    int command;
    bool active = false;

private:
    void listen();
    Controller* controller;
    struct pollfd poll_fd[1];
    int timer_fd;
    struct itimerspec timeout;
    int timersElapsed;
    int timer_err;
    int sec,usec;
};

#endif // GENERICTIMER_H
