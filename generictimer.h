#ifndef GENERICTIMER_H
#define GENERICTIMER_H

#include <poll.h>
#include <thread>
#include <sys/timerfd.h>
#include <QDebug>
#include "logging.h"
#include <csignal>

class Controller;        //forward declaration

class GenericTimer
{
public:
    GenericTimer();
    ~GenericTimer();
    int init(int command, Controller& controller);
    int init(int command, int command_data, Controller& controller);
    void start();
    void stop();
    void setInterval(int interval_us);
    int command;
    int command_data;
    bool active = false;

private:
    void listen();
    std::thread t1;
    Controller* controller;
    struct pollfd poll_fd[1];
    int timer_fd;
    struct itimerspec timeout;
    int timersElapsed;
    int timer_err;
    int sec,usec;
    bool init_done = false;
};

#endif // GENERICTIMER_H
