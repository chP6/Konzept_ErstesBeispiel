#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <map>
#include <utility>
#include "config.h"
using namespace Config;

class InputDevice {
public:
    InputDevice(const char* fileName);
    virtual ~InputDevice();

    virtual int init(struct pollfd *fd);
    bool eventReceived();
    bool readEvent(struct input_event &event);

    virtual int getEvent(std::vector<int> &data) = 0;

protected:
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

private:
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

#endif
