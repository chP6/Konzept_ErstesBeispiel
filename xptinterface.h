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
#include <fcntl.h>

#define MAXDATASIZE 2048

class XptInterface : public QObject
{
     Q_OBJECT
public:
    enum SrvAnswer {
        Error=-2,
        NACK=-1,
        ACK=0,
        NOP,
        DeviceType,
        Preamble
    };
    XptInterface();
    int init(int port, char* ipAdress);
    int changeIP(char* ipAdress);
    int sendChange(int source, int destination);
    int checkConnection();
    int connectToXpt();
    int disconnect();
    SrvAnswer processMessage(QList<QByteArray> &message);
    QList<QByteArray> appendInput(QByteArray &input);
    SrvAnswer receive();
    int getNumberOfInputs();
    int getNumberOfOutputs();
    QList<QString> getOutputLabels();
    QList<QString> getInputLabels();


private:
    struct sockaddr_in xpt_adress;
    int sockfd;
    int send_err;
    int recv_err;
    int connect_err;
    char rxbuffer[MAXDATASIZE];
    char txBuffer[35];
    int numberOfInputs;
    int numberOfOutputs;
    QList<QString> inputLabels;
    QList<QString> outputLabels;

signals:
    void inputLabelsChanged();


};

#endif // XPTINTERFACE_H
