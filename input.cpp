#include "input.h"
#include "events.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <linux/input.h>

#include <QDebug>

InputDevice::InputDevice(const char* fileName)
    : m_fileName(fileName), m_fd(0) {}

InputDevice::~InputDevice() {
    if (m_fd)
        close(m_fd->fd);
}

int InputDevice::init(struct pollfd *fd) {
    if (fd) {
        m_fd = fd;
        m_fd->fd = open(m_fileName, O_RDONLY);
        if (m_fd->fd < 0) {
            qDebug("failed to open %s: %s", m_fileName, strerror(errno));
            return -1;
        }
        if (ioctl(m_fd->fd, EVIOCGRAB, 1)) {
            qDebug("failed to grab %s: %s", m_fileName, strerror(errno));
            close(m_fd->fd);
            return -1;
        }
        m_fd->events = POLLIN | POLLPRI;
    }
    return 0;
}

bool InputDevice::eventReceived() {
    return m_fd ? (m_fd->revents & POLLIN) || (m_fd->revents & POLLPRI) : false;
}

bool InputDevice::readEvent(struct input_event &event) {
    if (m_fd) {
        size_t len = read(m_fd->fd, &event, sizeof(struct input_event));
        if (len == sizeof(struct input_event))
            return true;
        else
            qDebug("failed to read from %s: %s", m_fileName, strerror(errno));
    }
    return false;
}

/* ------------------------------------------------------------------------- */

RotaryEncoder::RotaryEncoder(const char* fileName)
    : InputDevice(fileName) {}

int RotaryEncoder::getEvent(std::vector<int> &data) {
    if (eventReceived()) {
        struct input_event event;
        if (readEvent(event)) {
            if (event.value == 1 || event.value == -1) {
                data.push_back(event.value);
                return E_INCREASE;
            }
        }
    }
    return E_NULLEVENT;
}

/* ------------------------------------------------------------------------- */

Keyboard::Keyboard(const char* fileName,
                       const std::map<int, std::pair<int, std::vector<int>>> keymap)
    : InputDevice(fileName), m_keymap(keymap) {}

int Keyboard::getEvent(std::vector<int> &data) {
    if (eventReceived()) {
        struct input_event event;
        if (readEvent(event)) {
            if (event.type == EV_KEY && event.value == 1 &&
                    m_keymap.find(event.code) != m_keymap.end()) {
                data = m_keymap.at(event.code).second;
                return m_keymap.at(event.code).first;
            }
        }
    }
    return E_NULLEVENT;
}

/* ------------------------------------------------------------------------- */

ZoomFocusJoystick::ZoomFocusJoystick(const char* fileName)
    : InputDevice(fileName) {}

int ZoomFocusJoystick::getEvent(std::vector<int> &data) {
    if (eventReceived()) {
        struct input_event event;
        if (readEvent(event)) {
            if (event.type == EV_ABS) {
                if (event.code == ABS_Y) {
                    data.push_back(event.value / 256); // -127..+127
                    return E_SET_ZOOM;
                } else if (event.code == ABS_X) {
                    data.push_back(event.value - INT16_MIN); // 0..65'535
                    return E_FOCUS_CHANGE;
                }
            }
        }
    }
    return E_NULLEVENT;
}
