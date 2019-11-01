#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <map>
#include <utility>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "hotplug.h"
#include <fcntl.h>

class InputDevice {
public:
    InputDevice(const char* fileName);
    virtual ~InputDevice();

    virtual int init(struct pollfd *fd);
    bool eventReceived();
    bool readEvent(struct input_event &event);

    virtual int getEvent(std::vector<int> &data) = 0;

    const char* m_fileName;
    struct pollfd* m_fd;
};

class RotaryEncoder : public InputDevice {
public:
    RotaryEncoder(const char* fileName);
    int getEvent(std::vector<int> &data) override;
};

class Keyboard : public InputDevice {
public:
    Keyboard(const char* fileName, const std::map<int, std::pair<int, std::vector<int>>> keymap);
    int getEvent(std::vector<int> &data) override;

protected:
    const std::map<int /* key */, std::pair<int /* event */, std::vector<int> /* data */>> m_keymap;
};

class XYZJoystick : public InputDevice {
public:
    XYZJoystick(const char* fileName);
    int getEvent(std::vector<int> &data) override;
};

class ZoomFocusJoystick : public InputDevice {
public:
    ZoomFocusJoystick(const char* fileName);
    int getEvent(std::vector<int> &data) override;
};

class I2cRotaryEncoder : public InputDevice {
public:
    I2cRotaryEncoder(const char* fileName, const std::map<int, std::pair<int, uint8_t>> keymap);
    ~I2cRotaryEncoder() override;
    int init(struct pollfd *fd) override;
    int getEvent(std::vector<int> &data) override;
private:
    int8_t readI2cBus(uint8_t adress);
    int m_i2c_fd;
    const std::map<int /* key */, std::pair<int /* event */, uint8_t /* adress */>> m_keymap;

};

class UsbOcp : public InputDevice {
public:
    UsbOcp(const char* fileName, const std::map<int, std::vector<int>> keymap);
    int getEvent(std::vector<int> &data) override;
private:
    const std::map<int /* key */, std::vector<int>> m_keymap;
};

#endif
