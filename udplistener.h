#ifndef UDPLISTENER_H
#define UDPLISTENER_H

#include "networkinterface.h"
#include "eventqueue.h"
#include "telegrambuilder.h"
#include <thread>

class Controller;

class UdpListener
{
public:
    bool applicationRunning = false;
    std::thread t2;
    UdpListener(Controller& controller);
    void startListener();
    void stopListener();
private:
    Networkinterface rxSocket;
    Controller* controller;
    Telegrambuilder telegramBuilder;
    uint8_t buffer[10];
    int rx_err;
    //[[noreturn]]
    void listener();
};

#endif // UDPLISTENER_H
