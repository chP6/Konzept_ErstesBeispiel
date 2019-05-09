#ifndef XPTINTERFACE_H
#define XPTINTERFACE_H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <stdio.h>


#define MAXDATASIZE 512

class XptInterface
{
public:
    XptInterface();
    ~XptInterface();
    int init(int port, char* ipAdress);
    int changeIP(char* ipAdress);
    int sendChange(int source, int destination);
    int requestConnection();
    int connectToXpt();
    int disconnect();


private:
    struct sockaddr_in xpt_adress;
    int sockfd;
    int send_err;
    int recv_err;
    int connect_err;
    char rxbuffer[MAXDATASIZE];
    char txBuffer[35];
};

#endif // XPTINTERFACE_H
