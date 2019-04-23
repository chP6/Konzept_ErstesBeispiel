#include "udplistener.h"
#include "controller.h"
#include <thread>
#include "config.h"

UdpListener::UdpListener(Controller& controller)
{
    this->controller = &controller;
    rx_err = 0;

    rx_err = rxSocket.init(8000);
    if(rx_err < 0){
        rx_err = errno;
        controller.logSystemError(rx_err, "Could not initialize Udp Listener");
    }
}

void UdpListener::startListener(){
    std::thread t1(&UdpListener::listener, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t1.detach();
}

void UdpListener::listener(){
    char buf[100];

    while(1){
        rx_err = rxSocket.receive(buffer);       //blockiert bis etwas empfangen
        if(rx_err < 0){
            rx_err = errno;
            controller->logSystemError(rx_err, "Could not receive udp traffic");
        }
        std::string addr;
        rxSocket.getSenderAddr(addr);

        sprintf(buf,"UDP RX: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d", buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9]);
        std::string str(buf);

        //controller->logError(str + " Addr: " + addr);
    }
}
