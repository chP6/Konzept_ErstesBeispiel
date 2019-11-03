#ifndef HOTPLUG_H
#define HOTPLUG_H

#include <arpa/inet.h>
#include <linux/input.h>
#include <sys/un.h>
#include <QtDebug>
#include <poll.h>
#include <input.h>

#define OCP_DEV_PATH        "/dev/ocp"

class Hotplug
{
public:
    Hotplug(std::list<class InputDevice*> *hotplugDevices);
    ~Hotplug();
    int init(struct pollfd *fd);
    bool eventReceived();
    void readEvent();
private:
    struct sockaddr_un m_programmSockaddr;
    const char* m_sockName;
    const char* m_deviceName;
    std::list<class InputDevice*> *m_hotplugdevices;
    struct pollfd* m_fd;
};

#endif // OCP_H
