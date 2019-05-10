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
#include <QList>


#define MAXDATASIZE 512

class XptInterface
{
public:
    enum SrvAnswer {
        Error=-2,
        NACK=-1,
        ACK=0,
        NOP,
        DeviceType
    };
    XptInterface();
    ~XptInterface();
    int init(int port, char* ipAdress);
    int changeIP(char* ipAdress);
    int sendChange(int source, int destination);
    int checkConnection();
    int connectToXpt();
    int disconnect();
    SrvAnswer processMessage(QList<QByteArray> &message);
    QList<QByteArray> appendInput(QByteArray &input);
    int getNumberOfInputs();
    int getNumberOfOutputs();
    


private:
    struct sockaddr_in xpt_adress;
    int sockfd;
    int send_err;
    int recv_err;
    int connect_err;
    char rxbuffer[2048];
    char txBuffer[35];
    int numberOfInputs;
    int numberOfOutputs;


};

#endif // XPTINTERFACE_H
