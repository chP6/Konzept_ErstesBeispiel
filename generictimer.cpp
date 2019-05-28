#include "generictimer.h"
#include "controller.h"
#include <QDebug>


GenericTimer::GenericTimer()
{
}

GenericTimer::~GenericTimer(){
    close(timer_fd);
    active = false;
}


int GenericTimer::init(int command, Controller& controller){
    this->controller = &controller;
    this->command = command;
    this->command_data = -1;

    //creating timer, disarmed
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_nsec = 0;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        timer_err = errno;
        controller.logSystemError(timer_err, "Could not create GenericTimer");
    }

    poll_fd[0].fd = timer_fd;
    poll_fd[0].events = POLLIN;

    if(!init_done){
        init_done=true;
        std::thread t1(&GenericTimer::listen, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
        t1.detach();
        //qDebug("Timer Thread created");
    }
    return 0;
}

int GenericTimer::init(int command, int command_data, Controller& controller){
    this->controller = &controller;
    this->command = command;
    this->command_data = command_data;


    //creating timer, disarmed
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_nsec = 0;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        timer_err = errno;
        controller.logSystemError(timer_err, "Could not set GenericTimer");
    }

    poll_fd[0].fd = timer_fd;
    poll_fd[0].events = POLLIN;

    if(!init_done){
        init_done=true;
        std::thread t1(&GenericTimer::listen, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
        t1.detach();
        //qDebug("Timer Thread created");
    }
     return 0;
}

void GenericTimer::start(){
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = usec*1000;
    timeout.it_interval.tv_sec = sec;
    timeout.it_interval.tv_nsec = usec*1000;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        timer_err = errno;
        controller->logSystemError(timer_err, "Could not set GenericTimer");
    }

    //qDebug("ARMED");
}

void GenericTimer::stop(){
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_nsec = 0;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        timer_err = errno;
        controller->logSystemError(timer_err, "Could not set GenericTimer");
    }
    //qDebug("DISARMED");
}

void GenericTimer::setInterval(int interval_us){
    sec = interval_us/(1000*1000);
    usec = interval_us%(1000*1000);
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
            if(command_data < 0){
                controller->queueEvent(command);
                //qDebug("Generic Timer Tick");
            }
            else{
                controller->queueEvent(command, command_data);
                //qDebug("Generic Timer %d Tick, command: %d",command_data,command);
            }
            //qDebug("Timer Tick");
        }
    }
    //qDebug("Timer Thread died");
}
