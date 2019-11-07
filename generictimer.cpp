#include "generictimer.h"
#include "controller.h"

/*Timer which can queue an event after expired interval*/
GenericTimer::GenericTimer()
{
}

GenericTimer::~GenericTimer(){
    close(timer_fd);
    active = false;
}

/*Initialization*/
int GenericTimer::init(int command, Controller& controller){
    this->controller = &controller;
    this->command = command;
    this->command_data = -1;

    /*creating timer, disarmed (interval = 0)*/
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_nsec = 0;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        qDebug("Could not create GenericTimer: %s",strerror(errno));
    }

    poll_fd[0].fd = timer_fd;
    poll_fd[0].events = POLLIN;
    return 0;
}

/*Overload function with data for event*/
int GenericTimer::init(int command, int command_data, Controller& controller){
    this->controller = &controller;
    this->command = command;
    this->command_data = command_data;


    /*creating timer, disarmed (it_value both 0)*/
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_nsec = 0;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        qDebug("Could not set GenericTimer: %s",strerror(errno));
    }

    poll_fd[0].fd = timer_fd;
    poll_fd[0].events = POLLIN;

//    if(!init_done){
//        init_done=true;
//        active = true;
//        std::thread t1(&GenericTimer::listen, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
//        t1.detach();
//        qDebug("Timer Thread created");
//    }
     return 0;
}

/**/
void GenericTimer::start(){
  if(!init_done){
    active = true;
    init_done=true;
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = usec*1000;
    timeout.it_interval.tv_sec = sec;
    timeout.it_interval.tv_nsec = usec*1000;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        qDebug("Could not start GenericTimer: %s",strerror(errno));
    }

        t1 = std::thread(&GenericTimer::listen, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
        //t1.detach();
        qCDebug(logicIo)<<"Timer Thread Created";
    }
}

void GenericTimer::stop(){
if(init_done){
    active = false;

    if (t1.joinable()) {
        t1.join();
    qCDebug(logicIo)<<"Timer Thread Joined";
    init_done = false;
    }
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_nsec = 0;
    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_nsec = 0;

    timer_err = timerfd_settime(timer_fd, 0, &timeout, nullptr);
    if (timer_err<0){
        qDebug("Could not set GenericTimer: %s",strerror(errno));
    }

}
    //qDebug("DISARMED");
}

/*Set timer interval*/
void GenericTimer::setInterval(int interval_us){
    sec = interval_us/(1000*1000);
    usec = interval_us%(1000*1000);

}

/*Starts as thread. Pushes event to queue after interval*/
void GenericTimer::listen(){
   do{
        /*Blocks until event occurs. -1 = infinite timeout*/
        timer_err = poll(poll_fd,1,-1);

        if(timer_err<0){
            qDebug("Could not read Generic Timer Pollstruct: %s",strerror(errno));
        }

        if(poll_fd[0].revents & POLLIN) {
            timer_err = read(timer_fd, &timersElapsed, 8);  //readout timer

            if(timer_err<0){
                qDebug("Could not read Generic Timer: %s",strerror(errno));
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
    } while(active);

    qCDebug(logicIo)<<"Timer Thread died";
}
