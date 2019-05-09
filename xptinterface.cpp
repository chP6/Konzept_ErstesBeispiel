#include "xptinterface.h"
#include <sys/socket.h>

XptInterface::XptInterface()
{

}

XptInterface::~XptInterface()
{

}

int XptInterface::init(int port, char *ipAdress)
{


    xpt_adress.sin_family = AF_INET;
    xpt_adress.sin_port = htons(port);
    inet_aton(ipAdress, &xpt_adress.sin_addr);
    bzero(&(xpt_adress.sin_zero), 8);

    return 0;

}

int XptInterface::connectToXpt()
{
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        return -1;
    }
    connect_err = connect(sockfd,(struct sockaddr *)&xpt_adress,sizeof(struct sockaddr));
    return connect_err;
}

int XptInterface::disconnect()
{
    connect_err=close(sockfd);
    if(connect_err<0){
        return  -1;
    }
    return 0;
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
    sprintf(txBuffer,"VIDEO OUTPUT ROUTING:\n%d %d\n\n",source,destination);
    if(connect_err < 0){
        return -1;
    }
    send_err = send(sockfd,txBuffer,35,MSG_NOSIGNAL);
    if (send_err < 0) {
        return -1;
        }

    return  0;
}

int XptInterface::requestConnection()
{
    memset(txBuffer,0,sizeof(txBuffer));
    sprintf(txBuffer,"PING:\n\n");
    send_err = send(sockfd,txBuffer,10,MSG_NOSIGNAL);
    if (send_err < 0) {
        return -1;
        }
    recv_err=recv(sockfd, rxbuffer, MAXDATASIZE, 0);
    if (recv_err < 0) {
        return -2;
        }


    return  0;
}

