#include "watchdog.h"

Watchdog::Watchdog(int period, int event)
{
    m_period = period;
    m_event = event;
}

Watchdog::~Watchdog()
{
    if (m_fd)
        close(m_fd->fd);
}

/*Initialization*/
int Watchdog::init(struct pollfd *fd){
    if(fd){
        m_fd = fd;
        m_fd->fd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (m_fd->fd < 0){
            qDebug("failed to create Timer: %s", strerror(errno));
            return  -1;
        }
        m_fd->events = POLLIN;

        m_timeout.it_value.tv_sec = m_period;
        m_timeout.it_value.tv_nsec = 0;
        m_timeout.it_interval.tv_sec = m_period;
        m_timeout.it_interval.tv_nsec = 0;

        if (timerfd_settime(m_fd->fd, 0, &m_timeout, nullptr) < 0){
            qDebug("failed to set Timer time: %s", strerror(errno));
            return -1;
        }

    }
    return 0;
}

int Watchdog::getEvent()
{
    if (eventReceived()) {
        if (readTimer()) {
                return m_event;
        }
    }
    return E_NULLEVENT;
}

bool Watchdog::eventReceived()
{
    return m_fd ? (m_fd->revents & POLLIN) : false;
}

bool Watchdog::readTimer()
{
    struct itimerspec timerElapsed;
    if(read(m_fd->fd, &timerElapsed, sizeof (struct itimerspec)) > 0){
        return true;
    }
    return false;
}




