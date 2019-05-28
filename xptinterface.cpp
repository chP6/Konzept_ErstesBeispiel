#include "xptinterface.h"
#include <sys/socket.h>
#include <QDebug>





XptInterface::XptInterface()
{

}

int XptInterface::init(XptType type, char *ipAdress)
{

    switch (type) {
    case XptInterface::BalckMagic:
        xpt_adress.sin_port = htons(BMDPORT);
        numberOfInputs = 40;
        numberOfOutputs = 40;
        inputLabels.clear();
        outputLabels.clear();
        break;
    case XptInterface::Ross:
        xpt_adress.sin_port = htons(ROSSPORT);
        numberOfInputs = 24;
        numberOfOutputs = 8;
        inputLabels.clear();
        for (int i = 0;i<numberOfInputs;i++) {
            inputLabels.append(QString::number(i+1));
        }

        outputLabels.clear();
        for (int i=0;i<numberOfOutputs;i++) {
            outputLabels.append(QStringLiteral("Aux %1").arg(i+1));
        }
        break;
    }

    xpt_adress.sin_family = AF_INET;
    inet_aton(ipAdress, &xpt_adress.sin_addr);
    bzero(&(xpt_adress.sin_zero), 8);

    return 0;

}

int XptInterface::connectToXpt()
{
    struct timeval tv;
            tv.tv_usec=1;

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv,sizeof(struct timeval));

    if(sockfd<0){
        return -1;
    }
    connect_err = ::connect(sockfd,(struct sockaddr *)&xpt_adress,sizeof(struct sockaddr));
        if(connect_err < 0)
        {
            return -1;
        }

        switch (xptType) {
            case XptInterface::BalckMagic:
                {
                XptInterface::SrvAnswer answer=bmdReceive();
                    if (answer==XptInterface::Preamble) {
                        answer=bmdReceive();
                        if (answer==XptInterface::ACK) {
                            return 1;
                                }
                                        }
                    else if (answer == XptInterface::ACK) {
                        return 1;
                        }
                    else {
                        return -1;
                       }
            break;
            }
            case XptInterface::Ross:
                emit inputLabelsChanged();
                return 1; //see what happens when connected to ross
            break;
        }

    return -1;
}
XptInterface::SrvAnswer XptInterface::bmdReceive()
{
    recv_err=recv(sockfd, rxbuffer, MAXDATASIZE, 0);     //receive xpt dump
         if (recv_err < 0) {
             return XptInterface::Error;
             }

         QByteArray input;
         input.append(rxbuffer);
         qDebug()<<input;
         QList<QByteArray> message = appendInput(input);

         if(!message.empty()){
             XptInterface::SrvAnswer result = processBmdMessages(message);
             qDebug()<<"message processed";
             return result;
         }
         return XptInterface::Error;
}

XptInterface::SrvAnswer XptInterface::processRossMessages(QList<QByteArray> &message)
{
    if (message.length() < 1) {
        return XptInterface::Error;
    }
    QByteArray header;

   if(!message.empty()){
       message.clear();     //to do: see what it sends exactely
      return XptInterface::ACK;
   }
   else {
       return XptInterface::Error;
   }
}

XptInterface::SrvAnswer XptInterface::rossReceive()
{
    recv_err=recv(sockfd, rxbuffer, MAXDATASIZE, 0);     //receive xpt dump
         if (recv_err < 0) {
             return XptInterface::Error;
             }

         QByteArray input;
         input.append(rxbuffer);
         qDebug()<<input;
         QList<QByteArray> message = appendInput(input);

         if(!message.empty()){
             XptInterface::SrvAnswer result = processRossMessages(message);
             qDebug()<<"message processed";
             return result;
         }
         return XptInterface::Error;
}

int XptInterface::disconnect()
{
    connect_err=close(sockfd);
    if(connect_err<0){
        return  -1;
    }
    return 0;
}

XptInterface::SrvAnswer XptInterface::processBmdMessages(QList<QByteArray> &message)
{
    if (message.length() < 1) {
        return XptInterface::Error;
    }
    QByteArray header;
    int processedMessages=0;

while(!message.empty()){
    header = message.first();
    message.pop_front();                //erster Eintrag löschen

    if (header.startsWith("ACK")) {

        processedMessages++;
    }
    else if (header.startsWith("NACK")) {
        processedMessages++;
    }
    else if (header.startsWith("VIDEOHUB DEVICE:")) {
        Q_FOREACH(QByteArray line, message) {
            message.pop_front();
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
            if(line.isEmpty()){
                qDebug()<<"break";
                break;

            }
        }
        processedMessages++;
    }
    else if (header.startsWith("INPUT LABELS:")) {
        QByteArray line;
        int endOfmessage=message.indexOf("");
        if(!inputLabels.empty())
            for(int i=0;i<endOfmessage;i++){
                line=message.first();
                bool ok;
                int index = line.left(1).toInt(&ok,10);
                if(ok){
                    inputLabels[index]=line.right(line.length()-2);
                    }
                message.pop_front();
        }
        else{
            inputLabels.clear();
            line = message.first();
            inputLabels.append(line.right(line.length()-2));
        for (int i =0 ; i<numberOfInputs-1 ; i++) {
            message.pop_front();
            line=message.first();
            inputLabels.append(line.right(line.length()-2));
        }
        }
        processedMessages++;
        emit inputLabelsChanged();
    }
    else if (header.startsWith("OUTPUT LABELS:")) {
        QByteArray line;
        int endOfmessage=message.indexOf("");
        if(!outputLabels.empty())
            for(int i=0;i<endOfmessage;i++){
                line=message.first();
                bool ok;
                int index = line.left(1).toInt(&ok,10);
                if(ok){
                    outputLabels[index]=line.right(line.length()-2);
                    }
                message.pop_front();
        }
        else{
            outputLabels.clear();
            line = message.first();
            outputLabels.append(line.right(line.length()-2));
        for (int i =0 ; i<endOfmessage ; i++) {
            message.pop_front();
            line=message.first();
            outputLabels.append(line.right(line.length()-2));
        }
        }
        processedMessages++;
        emit inputLabelsChanged();
    }
    else if (header.startsWith("PROTOCOL PREAMBLE:")) {

        if (message.length()<10) {
            return XptInterface::Preamble;
        }
        processedMessages++;
    }

}
    if(!processedMessages){
        return XptInterface::Error;
    }
    else {
        return XptInterface::ACK;
    }

}

QList<QByteArray> XptInterface::appendInput(QByteArray &input)
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



int XptInterface::getNumberOfInputs()
{
    return numberOfInputs;
}

int XptInterface::getNumberOfOutputs()
{
    return numberOfOutputs;
}

QList<QString> XptInterface::getOutputLabels()
{
    return outputLabels;
}

QList<QString> XptInterface::getInputLabels()
{
    return inputLabels;
}

void XptInterface::setXptType(XptInterface::XptType type)
{
    if (xptType != type) {
        xptType = type;
    }
}


int XptInterface::changeIP(char *ipAdress)
{
   connect_err = inet_aton(ipAdress, &xpt_adress.sin_addr);
   if (connect_err < 0) {
       return -1;
   }

   return 0;
}

int XptInterface::sendChange(int source, int destination)
{
    memset(txBuffer,0,sizeof(txBuffer));

    switch (xptType) {
    case XptInterface::BalckMagic:

            sprintf(txBuffer,"VIDEO OUTPUT ROUTING:\n%d %d\n\n",destination-1,source-1);
            if(connect_err < 0){
                return -1;
                    }
            send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
            if (send_err < 0) {
                return -1;
                    }

            return  0;

    case XptInterface::Ross:
        sprintf(txBuffer,"XPT AUX:%d:IN:%d\n",destination-1,source-1);
        if(connect_err < 0){
            return -1;
                }
        send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
        if (send_err < 0) {
            return -1;
                }

        return  0;
    }

}

int XptInterface::checkConnection()
{
    memset(txBuffer,0,sizeof(txBuffer));
    memset(rxbuffer,0,sizeof(rxbuffer));

        switch (xptType) {
        case XptInterface::BalckMagic:
        {
            sprintf(txBuffer,"PING:\n\n");
            send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
            if (send_err < 0) {
                return -1;
                }

            XptInterface::SrvAnswer answer=bmdReceive();
                if (answer==XptInterface::ACK) {
                    return 1;
                }
                else {
                    return -1;
                }
        }
        case XptInterface::Ross:
            sprintf(txBuffer,"HELP\n");
            send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
            if (send_err < 0) {
                return -1;
                }

            XptInterface::SrvAnswer answer=rossReceive();
                if (answer==XptInterface::ACK) {
                    return 1;
                }
                else {
                    return -1;
                }

        }



}

