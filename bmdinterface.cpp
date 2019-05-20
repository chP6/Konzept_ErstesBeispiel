#include "bmdinterface.h"


BmdInterface::BmdInterface()
{

}

BmdInterface::~BmdInterface()
{

}



int BmdInterface::connectToXpt()
{
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        return -1;
    }
    connect_err = connect(sockfd,(struct sockaddr *)&xpt_adress,sizeof(struct sockaddr));
        if(connect_err < 0)
        {
            return -1;
        }

   recv_err=recv(sockfd, rxbuffer, 2048, 0);     //receive xpt dump
        if (recv_err < 0) {
            return -2;
            }

        QByteArray input;
        input.append(rxbuffer);
        QList<QByteArray> message = appendInput(input);
        BmdInterface::SrvAnswer result = processMessage(message);
        if(result==DeviceType){
            return 1;
        }
        else {
            return 0;
        }
}



BmdInterface::SrvAnswer BmdInterface::processMessage(QList<QByteArray> &message)
{
    if (message.length() < 1) {
        return BmdInterface::Error;
    }
    QByteArray header = message.first();
    message.pop_front();                //erster Eintrag löschen

    if (header.startsWith("ACK")) {

        return  BmdInterface::ACK;
    }
    else if (header.startsWith("NACK")) {
        return BmdInterface::NACK;
    }
    else if (header.startsWith("PROTOCOL PREAMBLE:")) {
        Q_FOREACH(QByteArray line, message) {
            if(line.startsWith("Video inputs:")){
                int index = line.indexOf(':');
                QByteArray inputs = line.right(line.length()-(index+2));
                bool ok;
                numberOfInputs = inputs.toInt(&ok, 10);
                if(!ok){
                    numberOfInputs=40;
                }

            }
            if(line.startsWith("Video outputs:")){
                int index = line.indexOf(':');
               QByteArray outputs = line.right(line.length()-(index+2));
               bool ok;
               numberOfOutputs = outputs.toInt(&ok, 10);
               if(!ok){
                   numberOfOutputs=40;
               }
            }
        }
        return BmdInterface::DeviceType;
    }
    else {
        return BmdInterface::NOP;
    }



}

QList<QByteArray> BmdInterface::appendInput(QByteArray &input)
{
    QList<QByteArray> message;
    QList<QByteArray> lines = input.split('\n');
    Q_FOREACH(QByteArray line, lines)
    {
       /* if ((line.length() == 1 && line.at(0) == '\r') || line.length() == 0) {
            message.clear();
            return message;
        } else {*/
            message.append(line);
        //}
    }
    return message;

}



int BmdInterface::sendChange(int source, int destination)
{
    memset(txBuffer,0,sizeof(txBuffer));
    sprintf(txBuffer,"VIDEO OUTPUT ROUTING:\n%d %d\n\n",source,destination);
    if(connect_err < 0){
        return -1;
    }
    send_err = send(sockfd,txBuffer,sizeof (txBuffer)/sizeof (txBuffer[0]),MSG_NOSIGNAL);
    if (send_err < 0) {
        return -1;
        }

    return  0;
}

int BmdInterface::checkConnection()
{
    memset(txBuffer,0,sizeof(txBuffer));
    memset(rxbuffer,0,sizeof(rxbuffer));
    sprintf(txBuffer,"PING:\n\n");
    send_err = send(sockfd,txBuffer,10,MSG_NOSIGNAL);
    if (send_err < 0) {
        connect_err = -1;
        return -1;
        }

    recv_err=recv(sockfd, rxbuffer, MAXDATASIZE,0);
    if (recv_err < 0) {
        connect_err = -1;
        return -2;
        }
    QByteArray input;
    input.append(rxbuffer);
    QList<QByteArray> message = appendInput(input);
    BmdInterface::SrvAnswer result = processMessage(message);
    if(result == ACK){
        return 1;
    }
    else if (result == NACK ) {
        return 0;
    }
    else {
     return  -1;
    }



}

