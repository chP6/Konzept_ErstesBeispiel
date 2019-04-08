#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H


#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "bbmcommandtypes.h"
#include "telegrambuilder.h"

#define SERVER 	"192.168.0.10"	//Server IP-Address
#define PORT 	8000   			//The port on which to send data
#define BUFLEN 	512  			//Max length of buffer

//struct bbm_datagram {
//	char time;	/* event timestamp in milliseconds */
//	__s16 value;	/* value */
//	__u8 type;	/* event type */
//	__u8 number;	/* axis/button number */
//};

class Networkinterface
{
public:
    Networkinterface();
    ~Networkinterface();
    int init(void);
    int send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t d1);
    int send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t d1, uint8_t d2);
    int send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t d1, uint8_t d2, uint8_t d3);
    int send(uint8_t bbm_dev_no, uint8_t bbm_command, uint8_t data[4]);
    int receive(uint8_t* rec_buffer);

private:
    int lowLevelSend();
    int socket_fd;
    int send_err;
    int recv_err;
    struct sockaddr_in si_other;
    Telegrambuilder builder;
    socklen_t len;
    uint8_t buffer[10];
    std::vector<int> data;
};

#endif // NETWORKINTERFACE_H
