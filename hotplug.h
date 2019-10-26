#ifndef HOTPLUG_H
#define HOTPLUG_H

#include <arpa/inet.h>
#include <linux/input.h>
#include <sys/un.h>
#include <QtDebug>
#include <poll.h>

#define OCP_DEV_PATH        "/dev/ocp"

class Hotplug
{
public:
    Hotplug(const char* sockName);
    ~Hotplug();
    int init();
//Loopback IF
    bool readEvent();
private:
    int m_fd;
    struct sockaddr_un m_programmSockaddr;
    const char* m_sockName;
};

#endif // OCP_H
