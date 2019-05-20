#include "udplistener.h"
#include "controller.h"
#include "events.h"
#include "config.h"
#include <thread>
#include <QDebug>

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
    answer_s answer;

    while(1){
        rx_err = rxSocket.receive(buffer);       //blockiert bis etwas empfangen
        if(rx_err < 0){
            rx_err = errno;
            controller->logSystemError(rx_err, "Could not receive udp traffic");
        }
        std::string addr;
        rxSocket.getSenderAddr(addr);
        telegramBuilder.decode(buffer, answer);

        // =========== DECIDE WHAT TO DO ==============================================

        // Preset reached answer
        if(answer.type == TYPEC_FROM_HC && answer.command == PRESET_REACHED){
            controller->queueEvent(E_PRESET_REACHED, answer.from);
        }

        // Reply from camera/head
        if(answer.type == TYPEC_FROM_HC && answer.command != WATCHDOG && answer.from != SERVER){
            controller->queueEvent(E_CAMERA_ANSWER, answer.from, answer.command, answer.data[0]);
            //qDebug("Answer: %d", answer.data[0]);
        }

        // Watchdog answer from server
        if(answer.command == WATCHDOG && answer.from == SERVER){
            controller->queueEvent(E_RX_SERV_WATCHDOG);
        }

        // Watchdog answer from camera/head
        if(answer.command == WATCHDOG && answer.from != SERVER){
            controller->queueEvent(E_RX_CAMERA_WATCHDOG, answer.from, answer.data[1]);
            //qDebug()<<answer.from<<answer.data[1];
        }

        // Autofocus answer (todo: remove! double with reply from camera)
        if(answer.type == TYPEC_FROM_HC && answer.command == FOCUS_SET_ABSOLUTE){
            controller->queueEvent(E_AUTOFOCUS_ANSWER, answer.data[0]);
        }

        //debug: show answer packages
//        if((answer.from != SERVER) ){
//            qDebug("UDP RX: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d", buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9]);
//        }
        answer.data.clear();
    }
}
