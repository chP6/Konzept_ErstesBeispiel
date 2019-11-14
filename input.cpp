#include "input.h"

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
            return  -1;
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

XYZJoystick::XYZJoystick(const char* fileName)
    : InputDevice(fileName) {}

int XYZJoystick::getEvent(std::vector<int> &data) {
    if (eventReceived()) {
        struct input_event event;
        if (readEvent(event)) {
            if (event.type == EV_ABS) {
                switch (event.code) {
                case ABS_X: data.push_back(KControlJoystickX); break;
                case ABS_Y: data.push_back(KControlJoystickY); break;
                case ABS_Z: data.push_back(KControlJoystickZ); break;
                default:    data.push_back(KControlNone);      break;
                }
                data.push_back((event.value - 2047) * 16); // -32'767..32'767
                return E_CONTROL_INPUT;
            } else if (event.type == EV_KEY) {
                if (event.code == BTN_TRIGGER && event.value == 1)
                    return E_AUTOFOCUS;
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
                    data.push_back(KControlZoomRocker);
                    data.push_back(event.value);
                    return E_CONTROL_INPUT;
                } else if (event.code == ABS_X) {
                    data.push_back(KControlFocusWheel);
                    data.push_back(event.value);
                    return E_CONTROL_INPUT;
                }
            }
        }
    }
    return E_NULLEVENT;
}



/* ------------------------------------------------------------------------- */


UsbOcp::UsbOcp(const char *fileName, const std::map<int, std::vector<int> >  keymap)
    : InputDevice(fileName), m_keymap(keymap) { name = "USB Operational Control Panel";}

int UsbOcp::getEvent(std::vector<int> &data)
{

    if (eventReceived()) {
        struct input_event event;
        if (readEvent(event)) {
            if (event.type == EV_KEY && event.value == 1 &&
                    m_keymap.find(event.code) != m_keymap.end()) {
                    data = m_keymap.at(event.code);
                    return E_USB_OCP_CHANGE;
            }
        }
      }
    return E_NULLEVENT;
}

/* ------------------------------------------------------------------------- */

I2cRotaryEncoder::I2cRotaryEncoder(const char* fileName, const std::map<int, std::pair<int, uint8_t>> keymap)
    : InputDevice(fileName), m_keymap(keymap){}

I2cRotaryEncoder::~I2cRotaryEncoder()
{
    close(m_i2c_fd);
}


int I2cRotaryEncoder::init(pollfd *fd)
{
    if (fd) {
        m_fd = fd;
        m_fd->fd = open(m_fileName, O_RDONLY);
        if (m_fd->fd < 0) {
            qDebug("failed to open %s: %s", m_fileName, strerror(errno));
            return  -1;
        }
        if (ioctl(m_fd->fd, EVIOCGRAB, 1)) {
            qDebug("failed to grab %s: %s", m_fileName, strerror(errno));
            close(m_fd->fd);
            return -1;
        }
        m_fd->events = POLLIN | POLLPRI;

        m_i2c_fd = open("/dev/i2c-1", O_RDWR);
        if(m_i2c_fd < 0){
           qDebug("failed to open i2c Bus: %s",strerror(errno));
           return  -1;
       }

    }
    return 0;
}



int I2cRotaryEncoder::getEvent(std::vector<int> &data)
{
    if (eventReceived()) {
        struct input_event event;
        if (readEvent(event)) {
            if (event.type == EV_KEY && event.value == 1 &&
                m_keymap.find(event.code) != m_keymap.end()) {
                data.push_back(readI2cBus(m_keymap.at(event.code).second));
                return m_keymap.at(event.code).first;
            }
        }
    }
    return E_NULLEVENT;
}


int8_t I2cRotaryEncoder::readI2cBus(uint8_t adress)
{
    char obuf[1] = {0};char ibuf[1] = {0};
    if (ioctl(m_i2c_fd, I2C_SLAVE,adress)) {
        qDebug("failed to change slave adress on i2c Bus, %s",strerror(errno));
        return 0;
    }
    if (write(m_i2c_fd,obuf,1) < 0 ) {
        qDebug("failed to write to i2c Bus, %s",strerror(errno));
        return 0;
    }
    if (read(m_i2c_fd,ibuf,1) < 0) {
        qDebug("failed to read from i2c Bus, %s",strerror(errno));
        return 0;
    }
    int8_t value = int8_t(ibuf[0]);
    return (value < -80 || value > 80) ? 0 : value;
}
