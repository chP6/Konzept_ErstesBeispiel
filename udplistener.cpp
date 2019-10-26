#include "udplistener.h"
#include "controller.h"
#include "events.h"
#include "config.h"
#include <thread>
#include <QDebug>
#include <csignal>

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

/*Starts listener as a seperate thread*/
void UdpListener::startListener(){
    applicationRunning = true;
    t2 = std::thread(&UdpListener::listener, this);                  //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    //t1.detach();
}

void UdpListener::stopListener()
{
    rxSocket.closeSocket();
    applicationRunning = false;
    //std::raise(SIGCONT);

}

/*Read pending data on socket (or block), decode bbmnet telegram and push event to queue accordingly*/
void UdpListener::listener(){
    answer_s answer;

    /*Blocks until message received on socket*/
    while(applicationRunning){
        rx_err = rxSocket.receive(buffer);
        if(rx_err < 0){
            rx_err = errno;
            controller->logSystemError(rx_err, "Could not receive udp traffic");
        }
        std::string addr;
        rxSocket.getSenderAddr(addr);
        telegramBuilder.decode(buffer, answer);

        // =========== DECIDE WHAT TO DO ==============================================

        /*Command from adjacent RCP in the network*/
        if(answer.type == TYPED_TO_HC && answer.command != WATCHDOG && answer.command != GOTO_PRESET){
            if(!answer.data.empty()){
            controller->queueEvent(E_RX_ADJ_RCP_CMD, answer.from, answer.command, answer.data[0]);
            }
        }

        /*Preset reached*/
        if(answer.type == TYPEC_FROM_HC && answer.command == PRESET_REACHED){
            if(!answer.data.empty()){
            controller->queueEvent(E_PRESET_REACHED, answer.from, answer.data[0]);
            //qDebug("Answer: HeadNr: %d Command: %d Data: %d %d %d %d",answer.from,answer.command, answer.data[0],answer.data[1] ,answer.data[2] ,answer.data[3]);
            }
        }

        /*Preset goto confirmation ??*/
        if(answer.type == TYPED_TO_HC && answer.command == GOTO_PRESET){
            controller->queueEvent(E_EXT_PRESET_CHANGE, answer.from, answer.data[0]);
        }

        /*Reply from camera,head*/
        if(answer.type == TYPEC_FROM_HC && answer.command != WATCHDOG && answer.from != SERVER && answer.command != PRESET_REACHED){
            controller->queueEvent(E_CAMERA_ANSWER, answer.from, answer.command, answer.data[0]);
            //qDebug("Answer: HeadNr: %d Command: %d Data: %d",answer.from,answer.command, answer.data[0]);
        }

        /*Watchdog answer from server*/
        if(answer.command == WATCHDOG && answer.from == SERVER){
            controller->queueEvent(E_RX_SERV_WATCHDOG);
        }

        /*Watchdog answer from camera,head*/
        if(answer.command == WATCHDOG && answer.from != SERVER){
            controller->queueEvent(E_RX_CAMERA_WATCHDOG, answer.from, answer.data[1]);
            qDebug("Answer: HeadNr: %d Command: %d Data: %d",answer.from,answer.command, answer.data[1]);
            //qDebug()<<answer.from<<answer.data[1];
        }

//        /*Autofocus answer (todo: remove ?? double with reply from camera)*/
//        if(answer.type == TYPEC_FROM_HC && answer.command == FOCUS_SET_ABSOLUTE){
//            controller->queueEvent(E_AUTOFOCUS_ANSWER, answer.data[0]);
//        }

        //debug: show answer packages
//        if((1) ){
//            qDebug("UDP RX: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d", buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],buffer[8],buffer[9]);
//        }
        answer.data.clear();
    }
}
