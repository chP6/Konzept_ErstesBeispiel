#include "xptinterface.h"
#include <sys/socket.h>
#include <QDebug>
#include "logging.h"

xptinterface::xptinterface()
{

}

xptinterface::~xptinterface()
{

}


int xptinterface::disconnect()
{
    /*close socket*/
    connect_err=close(sockfd);
    if(connect_err<0){
        return  -1;
    }
    return 0;
}

QList<QString> xptinterface::getOutputLabels()
{
    return outputLabels;
}

QList<QString> xptinterface::getInputLabels()
{
    return inputLabels;
}

int xptinterface::getNumberOfInputs()
{
    return numberOfInputs;
}

int xptinterface::getNumberOfOutputs()
{
    return numberOfOutputs;
}


int xptinterface::changeIP(char *ipAdress)
{
    connect_err = inet_aton(ipAdress, &xpt_adress.sin_addr);
    if (connect_err < 0) {
        return -1;
    }

    return 0;
}

QList<QByteArray> xptinterface::appendInput(QByteArray &input)
{
    QList<QByteArray> message;
    QList<QByteArray> lines = input.split('\n');
    Q_FOREACH(QByteArray line, lines)
    {
            message.append(line);
    }
    return message;
}

/*************************************************************************/

RossInterface::RossInterface()
{

}

int RossInterface::init(QString ipAdress)
{
    xpt_adress.sin_port = htons(7788);
    numberOfInputs = 24+numberOfBanks*numberOfInputs;
    numberOfOutputs = 8;
    inputLabels.clear();
    /*Ross is alway the same, doesen't send labels etc.*/
        for (int i = 0;i<numberOfInputs;i++) {
            inputLabels.append(QString::number(i+1));
        }
        for (int i = 0;i<numberOfBanks;i++) {
            for (int j=0;j<numberOfInputs;j++) {
                inputLabels.append("CC"+QString::number(i)+" : "+QString::number(j));
            }

        }

    outputLabels.clear();
        for (int i=0;i<numberOfOutputs;i++) {
            outputLabels.append(QStringLiteral("Aux %1").arg(i+1));
            }

    xpt_adress.sin_family = AF_INET;
    QByteArray ba = ipAdress.toLocal8Bit();
    char *ipAdress_c = ba.data();
    inet_aton(ipAdress_c, &xpt_adress.sin_addr);
    bzero(&(xpt_adress.sin_zero), 8);

    return 0;
}

int RossInterface::connectToXpt()
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

    /*ross doesn't send a dump*/
    emit inputLabelsChanged(); //update the input label on touchscreen
    return 1;


}

int RossInterface::sendChange(int source, int destination)
{
    memset(txBuffer,0,sizeof(txBuffer));
    if (source < numberOfInputs) {
        sprintf(txBuffer,"XPT AUX:%d:IN:%d\n",destination+1,source+1);
    } else
    {
        int bank = (source - numberOfInputs) / numberOfInputs + 1;
        int macro = numberOfInputs*bank - source;
        sprintf(txBuffer,"CC: %d:%d\n",bank, macro);
    }

     //send change ross doesn't confirm
    //if(connect_err < 0){
      //  return -1;
        //    }
    send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
    if (send_err < 0) {
        return -1;
            }

    return  0;
}

int RossInterface::checkConnection()
{
    memset(txBuffer,0,sizeof(txBuffer));
    memset(rxbuffer,0,sizeof(rxbuffer));

    sprintf(txBuffer,"HELP\n");
    /*send ping*/
    send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
    if (send_err < 0) {
        return -1;
        }
    /*receive answer*/
    xptinterface::SrvAnswer answer=receive();
        if (answer==xptinterface::ACK) {
            return 1;
        }
        else {
            return -1;
        }
}

xptinterface::SrvAnswer RossInterface::processMessages(QList<QByteArray> &message)
{
    /*Nothing in Message*/
    if (message.length() < 1) {
        return xptinterface::Error;
    }

    /*Everything is good as long something is in the message*/
   if(!message.empty()){
       message.clear();
       return xptinterface::ACK;
   }
   else {
       return xptinterface::Error;
   }
}

xptinterface::SrvAnswer RossInterface::receive()
{
    /*receive xpt answer*/
    recv_err=recv(sockfd, rxbuffer, MAXDATASIZE, 0);
         if (recv_err < 0) {
             return xptinterface::Error;
             }

         QByteArray input;
         input.append(rxbuffer);    //put buffer in a Byte Array from Qt
         qCDebug(xptIo)<< "Received message from Ross device | Message:" << input;
         QList<QByteArray> message = appendInput(input);    //put input in a message separated by line feed

        // if(!message.empty()){
             xptinterface::SrvAnswer result = processMessages(message); //do something according to message
             qCDebug(xptIo)<< "Ross message processed";
             return result;
         //}
        // return xptinterface::Error;
}


/*******************************************************************************/

BmdInterface::BmdInterface()
{

}

int BmdInterface::init(QString ipAdress)
{
    xpt_adress.sin_port = htons(9990);
    numberOfInputs = 40;
    numberOfOutputs = 40;
    inputLabels.clear();
    outputLabels.clear();

    xpt_adress.sin_family = AF_INET;
    QByteArray ba = ipAdress.toLocal8Bit();
    char *ipAdress_c = ba.data();
    inet_aton(ipAdress_c, &xpt_adress.sin_addr);
    bzero(&(xpt_adress.sin_zero), 8);

    return 0;
}

int BmdInterface::connectToXpt()
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
        /*receive initial dump of BMD device*/
            xptinterface::SrvAnswer answer=receive();
            /*catch the preamble line*/
                if (answer==xptinterface::Preamble) {
                    /*receive the rest of the dump, inpul labels etc.*/
                    answer=receive();
                    if (answer==xptinterface::ACK) {
                        /*cath ack and return 1, all good*/
                        return 1;
                            }
                                    }
                else if (answer == xptinterface::ACK) {
                    /*if no dump was sent, just look after ack*/
                    return 1;
                    }

        return -1;
}

int BmdInterface::sendChange(int source, int destination)
{
    memset(txBuffer,0,sizeof(txBuffer));

    sprintf(txBuffer,"VIDEO OUTPUT ROUTING:\n%d %d\n\n",destination,source); //send change xpt should send ack

    send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
    if (send_err < 0) {
        return -1;
            }

    return  0;
}

int BmdInterface::checkConnection()
{
    //clear buffers
    memset(txBuffer,0,sizeof(txBuffer));
    memset(rxbuffer,0,sizeof(rxbuffer));

    sprintf(txBuffer,"PING:\n\n");
    /*send a ping*/
    send_err = send(sockfd,txBuffer,strlen(txBuffer),MSG_NOSIGNAL);
    if (send_err < 0) {
        return -1;
        }
    /*receive answer*/
    xptinterface::SrvAnswer answer=receive();
        if (answer==xptinterface::ACK) {
            return 1;
        }
        else {
            return -1;
        }
}

xptinterface::SrvAnswer BmdInterface::processMessages(QList<QByteArray> &message)
{
    /*Message shouldn't be empty*/
    if (message.length() < 1) {
        return xptinterface::Error;
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
            return xptinterface::Preamble;
        }
        processedMessages++;
    }

}
    if(!processedMessages){
        return xptinterface::Error;
    }
    else {
        return xptinterface::ACK;
    }

}

xptinterface::SrvAnswer BmdInterface::receive()
{
    /*receive xpt answer*/
       recv_err=recv(sockfd, rxbuffer, MAXDATASIZE,0);
         if (recv_err < 0) {
             return xptinterface::Error;
             }

       QByteArray input;
       input.append(rxbuffer);    //put buffer in a Byte Array from Qt
       qCDebug(xptIo)<< "Received message from BMD device | Message:" << input;
       QList<QByteArray> message = appendInput(input); //put input in a message separated by line feed

         if(!message.empty()){
             xptinterface::SrvAnswer result = processMessages(message); //do something according to message
             qCDebug(xptIo)<< "BMD message processed";
             return result;
         }
         return xptinterface::Error;
}
