#include "hotplug.h"
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

Hotplug::Hotplug(const char* sockName)
{
    m_sockName = sockName;
}

Hotplug::~Hotplug()
{
    close(m_fd);
}

/*Setup loopback interface to trigger interrupt when OCP connected.
Ping on loobpack interface executed from external C-program via udev rule ??*/
int Hotplug::init(){

        memset((char *)&m_programmSockaddr, 0, sizeof(m_programmSockaddr));
        /*Open socket on local loopback interface*/
        int hotplug_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        if(hotplug_socket < 0){
                return -1;
            }
        m_fd = hotplug_socket;

        m_programmSockaddr.sun_family = AF_UNIX;
        strcpy(m_programmSockaddr.sun_path,m_sockName);
        unlink(m_sockName);
        socklen_t len = sizeof (m_programmSockaddr);
        if (bind(m_fd, (struct sockaddr *) &m_programmSockaddr, len) < 0) {
            qDebug("failed bind socket %d: %s", m_fd, strerror(errno));
            return -1;
        }
        //m_fd->events = POLLIN | POLLPRI;

       return m_fd;
}


/*Read data on socket*/
bool Hotplug::readEvent(){
    struct sockaddr_un peer;
    memset((char *)&peer, 0, sizeof(peer));
    size_t len = sizeof (m_programmSockaddr);
    char buf[256];
    if(recvfrom(m_fd, buf, sizeof (buf)/sizeof (buf[0]), 0, (struct sockaddr *) &peer, &len) < 0){
        qDebug("failed read from socket %d: %s", m_fd, strerror(errno));
    }
    qDebug("received Device: %s",buf);

    return strcmp(m_sockName,&buf[0])==0 ? true : false;
}
