#include "generictimer.h"
#include "controller.h"
#include <thread>
#include <QDebug>


GenericTimer::GenericTimer()
{
}


int GenericTimer::init(int interval_us, int command, Controller& controller){
    this->controller = &controller;
    this->command = command;

    sec = interval_us/(1000*1000);
    usec = interval_us%(1000*1000);
}

void GenericTimer::start(){
    //only start if not already active
    if(!active){
        active = true;
        timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);

        timeout.it_value.tv_sec = sec;
        timeout.it_value.tv_nsec = usec*1000;
        timeout.it_interval.tv_sec = sec;
        timeout.it_interval.tv_nsec = usec*1000;

        timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
        if (timer_err<0){
            timer_err = errno;
            controller->logSystemError(timer_err, "Could not create GenericTimer");
        }

        poll_fd[0].fd = timer_fd;
        poll_fd[0].events = POLLIN;

        std::thread t1(&GenericTimer::listen, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
        t1.detach();
    }
}

void GenericTimer::stop(){
    active = false;
    close(timer_fd);
}


void GenericTimer::listen(){
    while(active){
        timer_err = poll(poll_fd,1,-1);                      //poll. Blocks until event occurs -> SIZE setzen! current = 15; -1 = infinite timeout

        if(timer_err<0){
            timer_err = errno;
            controller->logSystemError(timer_err, "Could not read Generic Timer Pollstruct");
        }

        if(poll_fd[0].revents & POLLIN) {
            timer_err = read(timer_fd, &timersElapsed, 8);  //readout timer

            if(timer_err<0){
                timer_err = errno;
                controller->logSystemError(timer_err, "Could not read Generic Timer");
            }
            controller->queueEvent(command);
            qDebug("Generic Timer Tick, command: %d",command);
        }
    }
}
