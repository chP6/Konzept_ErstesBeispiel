#include "xptinterface.h"
#include <sys/socket.h>
#include <QDebug>
#include "logging.h"


XptInterface::XptInterface()
{

}

int XptInterface::init(XptType type, char *ipAdress)
{
/*Prepare initialization according to the xpt type*/
    switch (type) {
        case XptInterface::BalckMagic:
            xptType = XptInterface::BalckMagic;
            xpt_adress.sin_port = htons(BMDPORT);
            numberOfInputs = 40;
            numberOfOutputs = 40;
            inputLabels.clear();
            outputLabels.clear();
         break;
        case XptInterface::Ross:
            xptType = XptInterface::Ross;
            xpt_adress.sin_port = htons(ROSSPORT);
            numberOfInputs = 24;
            numberOfOutputs = 8;
            inputLabels.clear();
            /*Ross is alway the same, doesen't send labels etc.*/
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
            tv.tv_usec=100000;
    /*Create a tcp socket*/
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    /*prevent blocking when connection is lost, eg cable plugged out*/
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv,sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tv,sizeof(struct timeval));

    if(sockfd<0){
        return -1;
    }
    /*connect to the prepared xpt in init*/
    connect_err = ::connect(sockfd,(struct sockaddr *)&xpt_adress,sizeof(struct sockaddr));
        if(connect_err < 0)
        {
            return -1;
        }

        switch (xptType) {
            case XptInterface::BalckMagic:
                {
            /*receive initial dump of BMD device*/
                XptInterface::SrvAnswer answer=bmdReceive();
                /*catch the preamble line*/
                    if (answer==XptInterface::Preamble) {
                        /*receive the rest of the dump, inpul labels etc.*/
                        answer=bmdReceive();
                        if (answer==XptInterface::ACK) {
                            /*cath ack and return 1, all good*/
                            return 1;
                                }
                                        }
                    else if (answer == XptInterface::ACK) {
                        /*if no dump was sent, just look after ack*/
                        return 1;
                        }
                    else {
                        return -1;
                       }
            break;
            }
            case XptInterface::Ross:
            /*ross doesn't send a dump*/
                emit inputLabelsChanged(); //update the input label on touchscreen
                return 1;
        }

    return -1;
}
XptInterface::SrvAnswer XptInterface::bmdReceive()
{
    /*receive xpt answer*/
    recv_err=recv(sockfd, rxbuffer, MAXDATASIZE,0);
         if (recv_err < 0) {
             return XptInterface::Error;
             }

         QByteArray input;
         input.append(rxbuffer);    //put buffer in a Byte Array from Qt
         qCDebug(xptIo)<< "Received message from BMD device | Message:" << input;
         QList<QByteArray> message = appendInput(input); //put input in a message separated by line feed

         if(!message.empty()){
             XptInterface::SrvAnswer result = processBmdMessages(message); //do something according to message
             qCDebug(xptIo)<< "BMD message processed";
             return result;
         }
         return XptInterface::Error;
}

XptInterface::SrvAnswer XptInterface::processRossMessages(QList<QByteArray> &message)
{
    /*Nothing in Message*/
    if (message.length() < 1) {
        return XptInterface::Error;
    }

    /*Everything is good as long something is in the message*/
   if(!message.empty()){
       message.clear();
       return XptInterface::ACK;
   }
   else {
       return XptInterface::Error;
   }

}

XptInterface::SrvAnswer XptInterface::rossReceive()
{
    /*receive xpt answer*/
    recv_err=recv(sockfd, rxbuffer, MAXDATASIZE, 0);
         if (recv_err < 0) {
             return XptInterface::Error;
             }

         QByteArray input;
         input.append(rxbuffer);    //put buffer in a Byte Array from Qt
         qCDebug(xptIo)<< "Received message from Ross device | Message:" << input;
         QList<QByteArray> message = appendInput(input);    //put input in a message separated by line feed

         if(!message.empty()){
             XptInterface::SrvAnswer result = processRossMessages(message); //do something according to message
             qCDebug(xptIo)<< "Ross message processed";
             return result;
         }
         return XptInterface::Error;
}

int XptInterface::disconnect()
{
    /*close socket*/
    connect_err=close(sockfd);
    if(connect_err<0){
        return  -1;
    }
    return 0;
}

XptInterface::SrvAnswer XptInterface::processBmdMessages(QList<QByteArray> &message)
{
    /*Message shouldn't be empty*/
    if (message.length() < 1) {
        return XptInterface::Error;
    }
    QByteArray header;
    int processedMessages=0;

while(!message.empty()){
    header = message.first();               //Store first entry of message into header
    message.pop_front();                    //delete first entry of message

    if (header.startsWith("ACK")) {         //ACK received

        processedMessages++;                //Message processed
    }
    else if (header.startsWith("NACK")) {   //NACK received
        processedMessages++;                //Message processed
    }
    else if (header.startsWith("VIDEOHUB DEVICE:")) { //This is something from the initial dump. Means the following information is about the Number of I/O
        Q_FOREACH(QByteArray line, message) {
            message.pop_front();                    //delete first line of the message
            if(line.startsWith("Video inputs:")){   //look after Video inputs
                int index = line.indexOf(':');
                QByteArray inputs = line.right(line.length()-(index+2)); //find the number
                bool ok;
                numberOfInputs = inputs.toInt(&ok, 10);                 //store new value if number is correct
                if(!ok){
                    numberOfInputs=40;              //if number is not correct store the default value of 40 inputs
                }

            }
            if(line.startsWith("Video outputs:")){                         //look after Video inputs
                int index = line.indexOf(':');
               QByteArray outputs = line.right(line.length()-(index+2));   //find the number
               bool ok;
               numberOfOutputs = outputs.toInt(&ok, 10);                    //store new value if number is correct
               if(!ok){
                   numberOfOutputs=40;                                      //if number is not correct store the default value of 40 outputs
               }
            }
            if(line.isEmpty()){                   //entries after VIDEOHUB DEVICE: are processed, leave FOREACH
                break;

            }
        }
        processedMessages++;                      //Messages have been processed
    }
    else if (header.startsWith("INPUT LABELS:")) { //This is something from the initial dump or whenever a label has changed. Means the following information is about Input labels
        QByteArray line;
        int endOfmessage=message.indexOf("");           //end of the input label list
        if(!inputLabels.empty())                        //replace a label if it has changed
            for(int i=0;i<endOfmessage;i++){
                line=message.first();                   //take first line
                bool ok;
                int index = line.left(1).toInt(&ok,10); //find the number of the input
                if(ok){
                    inputLabels[index]=line.right(line.length()-2); //replace it with the corresponding label
                    }
                message.pop_front();                    // delete line go on with next line
        }
        else{                                           //all new labels from initial dump
            inputLabels.clear();
            line = message.first();                     //take first entry
            inputLabels.append(line.right(line.length()-2)); //add first label
        for (int i =0 ; i<numberOfInputs-1 ; i++) {         // do it for all following labels
            message.pop_front();
            line=message.first();
            inputLabels.append(line.right(line.length()-2));
        }
        }
        processedMessages++;
        emit inputLabelsChanged();                      //update view
    }
    else if (header.startsWith("OUTPUT LABELS:")) { //This is something from the initial dump or whenever a label has changed. Means the following information is about Output labels
        QByteArray line;
        int endOfmessage=message.indexOf("");           //end of the input label list
        if(!outputLabels.empty())                       //replace a label if it has changed
            for(int i=0;i<endOfmessage;i++){
                line=message.first();                   //take first line
                bool ok;
                int index = line.left(1).toInt(&ok,10); //find the number of the output
                if(ok){
                    outputLabels[index]=line.right(line.length()-2); //replace it with the corresponding label
                    }
                message.pop_front();                    // delete line go on with next line
        }
        else{                                           //all new labels from initial dump
            outputLabels.clear();
            line = message.first();                     //take first entry
            outputLabels.append(line.right(line.length()-2)); //add first label
        for (int i =0 ; i<endOfmessage ; i++) {         // do it for all following labels
            message.pop_front();
            line=message.first();
            outputLabels.append(line.right(line.length()-2));
        }
        }
        processedMessages++;
        emit inputLabelsChanged();
    }
    else if (header.startsWith("PROTOCOL PREAMBLE:")) { // detect preamble

        if (message.length()<10) {          // return when nothing else follows
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
            message.append(line);
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

            sprintf(txBuffer,"VIDEO OUTPUT ROUTING:\n%d %d\n\n",destination,source); //send change xpt should send ack
          //  if(connect_err < 0){
            //    return -1;
              //      }
            send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
            if (send_err < 0) {
                return -1;
                    }

            return  0;

    case XptInterface::Ross:
        sprintf(txBuffer,"XPT AUX:%d:IN:%d\n",destination+1,source+1); //send change ross doesn't confirm
        //if(connect_err < 0){
          //  return -1;
            //    }
        send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
        if (send_err < 0) {
            return -1;
                }

        return  0;
    }
    return -1;
}

int XptInterface::checkConnection()
{
    //clear buffers
    memset(txBuffer,0,sizeof(txBuffer));
    memset(rxbuffer,0,sizeof(rxbuffer));

        switch (xptType) {
        case XptInterface::BalckMagic:
        {
            sprintf(txBuffer,"PING:\n\n");
            /*send a ping*/
            send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
            if (send_err < 0) {
                return -1;
                }
            /*receive answer*/
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
            /*send ping*/
            send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
            if (send_err < 0) {
                return -1;
                }
            /*receive answer*/
            XptInterface::SrvAnswer answer=rossReceive();
                if (answer==XptInterface::ACK) {
                    return 1;
                }
                else {
                    return -1;
                }

        }

    return -1;

}

