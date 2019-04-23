#ifndef OCP_H
#define OCP_H

#include <arpa/inet.h>
#include <linux/input.h>


#define OCP_DEV_PATH        "/dev/ocp"
#define OCP_E_IRIS_UP       1
#define OCP_E_IRIS_DOWN     2
#define OCP_E_PED_UP        3
#define OCP_E_PED_DOWN      4
#define OCP_E_WRED_UP       5
#define OCP_E_WRED_DOWN     6
#define OCP_E_WBLUE_UP      7
#define OCP_E_WBLUE_DOWN    8
#define OCP_E_BRED_UP       9
#define OCP_E_BRED_DOWN     10
#define OCP_E_BBLUE_UP      11
#define OCP_E_BBLUE_DOWN    12
#define OCP_E_GAIN_UP       13
#define OCP_E_GAIN_DOWN     14
#define OCP_E_SHUTTER_UP    15
#define OCP_E_SHUTTER_DOWN  16


class OCP
{
public:
    OCP();
    ~OCP();
    int connect();
    void closeFd();
    int processEvent(int& event);
    int fd;
//Loopback IF
    int initLoopbackInterface();
    int receiveLoopback();
    int loopback_socket_fd;

private:
    int ocp_err;
    struct input_event in;
//Loopback IF
    struct sockaddr_in myaddr;
    struct sockaddr_in destaddr;
    char buf[1];
    socklen_t len_sender;
};

#endif // OCP_H
