#include "hotplug.h"

Hotplug::Hotplug(std::list<class InputDevice*> *hotplugDevices)
{
    m_sockName = "/tmp/hotplug";
    m_hotplugdevices = hotplugDevices;
}

Hotplug::~Hotplug()
{
   if(m_fd)
    close(m_fd->fd);
}

int Hotplug::init(struct pollfd *fd){

    if (fd) {
        m_fd = fd;
        memset((char *)&m_programmSockaddr, 0, sizeof(m_programmSockaddr));

        /*Open socket on local loopback interface*/
        int hotplug_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        if(hotplug_socket < 0){
                return -1;
            }
        m_fd->fd = hotplug_socket;
        m_programmSockaddr.sun_family = AF_UNIX;
        strcpy(m_programmSockaddr.sun_path,m_sockName);
        unlink(m_sockName);
        socklen_t len = sizeof (m_programmSockaddr);
        if (bind(m_fd->fd, (struct sockaddr *) &m_programmSockaddr, len) < 0) {
            qDebug("failed bind socket %d: %s", m_fd->fd, strerror(errno));
            return -1;
        }
        m_fd->events = POLLIN | POLLPRI;
    }
       return 0;
}

bool Hotplug::eventReceived() {
    return m_fd ? (m_fd->revents & POLLIN) || (m_fd->revents & POLLPRI) : false;
}

/*Read data on socket*/
void Hotplug::readEvent(){
   if(eventReceived()){
    struct sockaddr_un peer;
    memset((char *)&peer, 0, sizeof(peer));
    size_t len = sizeof (m_programmSockaddr);
    char buf[256];
    memset(buf,0,sizeof(buf)/sizeof (buf[0]));
    if(recvfrom(m_fd->fd, buf, sizeof (buf)/sizeof (buf[0]), 0, (struct sockaddr *) &peer, &len) < 0){
        qDebug("failed read from socket %d: %s", m_fd->fd, strerror(errno));
    }
    for (InputDevice* d : *m_hotplugdevices){
        if(strcmp(d->m_fileName,&buf[0])==0){
            d->init(d->m_fd);
            qInfo(user,"new Device: %s",d->name);
        }
    }
}

}
