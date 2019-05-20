#include "rossinterface.h"

RossInterface::RossInterface()
{
    numberOfInputs=24;
    numberOfInputs=8;
}

RossInterface::~RossInterface()
{

}


int RossInterface::sendChange(int source, int destination)
{
    memset(txBuffer,0,sizeof (txBuffer));
    sprintf(txBuffer,"XPT AUX:%d:IN:%d",source,destination);
    if( connect_err < 0 ){
        return -1;
    }
    send_err = send(sockfd,txBuffer,sizeof (txBuffer)/sizeof (txBuffer[0]),MSG_NOSIGNAL);
    if( send_err < 0){
        return -1;
    }
    return 0;

}

int RossInterface::checkConnection()
{
    send_err=send(sockfd,"0",1,MSG_NOSIGNAL);
    if(send_err < 0){
        connect_err = -1;
        return -1;
    }
    return 1;

}

int RossInterface::connectToXpt()
{
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        return -1;
    }

    connect_err = connect(sockfd,(struct sockaddr *)&xpt_adress,sizeof(struct sockaddr));
    if(connect_err < 0){
        return -1;
    }
    return 1;

}


