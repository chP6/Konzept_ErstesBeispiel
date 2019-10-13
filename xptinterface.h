
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
#include <QObject>

#define MAXDATASIZE 2048

class xptinterface : public QObject

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

    xptinterface();
    virtual ~xptinterface();
    virtual int init(QString ipAdress) = 0;
    virtual int sendChange(int source, int destination) = 0;
    virtual int checkConnection() = 0;
    virtual int connectToXpt() = 0;
    int disconnect();
    QList<QString> getOutputLabels();
    QList<QString> getInputLabels();
    int getNumberOfInputs();
    int getNumberOfOutputs();
    int changeIP(char *ipAdress);


protected:
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
    QList<QByteArray> appendInput(QByteArray &input);
    virtual SrvAnswer processMessages(QList<QByteArray> &message) = 0;
    virtual SrvAnswer receive() = 0;
signals:
    void inputLabelsChanged();
};

class BmdInterface : public xptinterface {
public:
    BmdInterface();
    int init(QString ipAdress);
    int connectToXpt();
    int sendChange(int source, int destination);
    int checkConnection();

protected:
    SrvAnswer processMessages(QList<QByteArray> &message);
    SrvAnswer receive();
};

class RossInterface : public xptinterface {
public:
    RossInterface();
    int init(QString ipAdress);
    int connectToXpt();
    int sendChange(int source, int destination);
    int checkConnection();

private:
    int numberOfBanks = 4;

protected:
    SrvAnswer processMessages(QList<QByteArray> &message);
    SrvAnswer receive();
};

#endif // XPTINTERFACE_H
