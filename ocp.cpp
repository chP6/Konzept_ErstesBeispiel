#include "ocp.h"
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

OCP::OCP()
{
}

OCP::~OCP()
{
    close(loopback_socket_fd);
    close(fd);
}

/*Connect OCP*/
int OCP::connect(){
    fd = open(OCP_DEV_PATH, O_RDONLY );
    if(fd < 0){
        return fd;
    }
    ocp_err = ioctl(fd, EVIOCGRAB, 1);
    if(fd < 0){         //"Couldn't grab"
        return ocp_err;
    }
    return 0;
}

/*Disconnect OCP*/
void OCP::closeFd(){
    close(fd);
}


/*Read and process OCP event*/
int OCP::processEvent(int& event){      // only valid event if not -1
    ocp_err = read(fd, &in, sizeof(struct input_event));
    if(ocp_err<0){
        return ocp_err;
    }

    if(in.value == 1){       // -> code richtiger wert, 1x mit val=0 , 1x mit val=1
        switch(in.code)
        {
        case KEY_A:
            event = OCP_E_IRIS_UP;
            break;
        case KEY_B:
            event = OCP_E_IRIS_DOWN;
            break;
        case KEY_C:
            event = OCP_E_PED_UP;
            break;
        case KEY_D:
            event = OCP_E_PED_DOWN;
            break;
        case KEY_E:
            event = OCP_E_BRED_UP;
            break;
        case KEY_F:
            event = OCP_E_BRED_DOWN;
            break;
        case KEY_G:
            event = OCP_E_BBLUE_UP;
            break;
        case KEY_H:
            event = OCP_E_BBLUE_DOWN;
            break;
        case KEY_I:
            event = OCP_E_WRED_UP;
            break;
        case KEY_J:
            event = OCP_E_WRED_DOWN;
            break;
        case KEY_K:
            event = OCP_E_WBLUE_UP;
            break;
        case KEY_L:
            event = OCP_E_WBLUE_DOWN;
            break;
        case KEY_M:
            event = OCP_E_GAIN_UP;
            break;
        case KEY_N:
            event = OCP_E_GAIN_DOWN;
            break;
        case KEY_O:
            event = OCP_E_SHUTTER_UP;
            break;
        case KEY_P:
            event = OCP_E_SHUTTER_DOWN;
            break;
        default:
            event = -1;
            break;
        }
    }
    else {
        event = -1;
    }
    fflush(stdout);

    return 0;
}

/*Setup loopback interface to trigger interrupt when OCP connected.
Ping on loobpack interface executed from external C-program via udev rule ??*/
int OCP::initLoopbackInterface(){

    /*Open socket on local loopback interface*/
    loopback_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
       if(loopback_socket_fd < 0){
           return -1;
       }

       memset((char *)&destaddr, 0, sizeof(destaddr));
       destaddr.sin_family = AF_INET;
       destaddr.sin_port = htons(10001);

       memset((char *)&myaddr, 0, sizeof(myaddr));
       myaddr.sin_family = AF_INET;
       myaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
       myaddr.sin_port = htons(10000);

       ocp_err = bind(loopback_socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
       if(ocp_err < 0){
           return -2;
       }

       return 0;
}

/*Read data on socket*/
int OCP::receiveLoopback(){
    ocp_err = recvfrom(loopback_socket_fd, (char*)buf, sizeof(buf) , 0, (struct sockaddr *)&destaddr, &len_sender);
    return ocp_err;
}
