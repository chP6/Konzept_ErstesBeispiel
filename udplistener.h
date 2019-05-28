#ifndef UDPLISTENER_H
#define UDPLISTENER_H

#include "networkinterface.h"
#include "eventqueue.h"
#include "telegrambuilder.h"

class Controller;

class UdpListener
{
public:
    UdpListener(Controller& controller);
    void startListener();
private:
    Networkinterface rxSocket;
    EventQueue* eventQueue;
    Controller* controller;
    Telegrambuilder telegramBuilder;
    uint8_t buffer[10];
    int rx_err;
    [[noreturn]]void listener();
};

#endif // UDPLISTENER_H
