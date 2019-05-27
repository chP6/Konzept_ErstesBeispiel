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
#define BMDPORT 9990
#define ROSSPORT 7788

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
    enum XptType{
        BalckMagic,
        Ross
    };

    XptInterface();
    int init(XptType type, char* ipAdress);
    int changeIP(char* ipAdress);
    int sendChange(int source, int destination);
    int checkConnection();
    int connectToXpt();
    int disconnect();
    int getNumberOfInputs();
    int getNumberOfOutputs();
    QList<QString> getOutputLabels();
    QList<QString> getInputLabels();
    void setXptType(XptType type);


private:
    SrvAnswer processBmdMessages(QList<QByteArray> &message);
    QList<QByteArray> appendInput(QByteArray &input);
    SrvAnswer bmdReceive();
    SrvAnswer processRossMessages(QList<QByteArray> &message);
    SrvAnswer rossReceive();

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
    XptType xptType;




signals:
    void inputLabelsChanged();


};

#endif // XPTINTERFACE_H
