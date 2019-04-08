#ifndef BBMJOYSTICK_H
#define BBMJOYSTICK_H

#include <linux/joystick.h>
#include <stdint.h>

#define NUM_OF_AXIS 3

struct joystickData {
    int debug[2];
    uint8_t tilt_val[4];
    uint8_t button_val;
};


class BBMJoystick
{
public:
    BBMJoystick();
    joystickData processEvent();
    int joystick_fd;
private:
    struct js_event js;
    int val_x, val_x_old, val_y;
    int axis[NUM_OF_AXIS-1];
    int readErr;
    joystickData jsData;
};

#endif // BBMJOYSTICK_H
