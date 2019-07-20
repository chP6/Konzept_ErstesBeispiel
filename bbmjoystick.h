#ifndef BBMJOYSTICK_H
#define BBMJOYSTICK_H

#include <linux/joystick.h>
#include <stdint.h>

#define NUM_OF_AXIS 3

struct joystickData {
    int xCoord;
    int yCoord;
    int zCoord;
    int buttonVal;
};


class BBMJoystick
{
public:
    BBMJoystick();
    int init();
    int processEvent(joystickData& jsData);
    void initRead();
    int joystick_fd;
private:
    struct js_event js;
    int val_x, val_x_old, val_y, val_z;
    int axis[NUM_OF_AXIS];
    int readErr;
    /*Snsitivity of Joystick Calculus*/
    float yLowerLimit = 1000;
    float xLowerLimit = 1000;
    float xyOffset = 5000;
    float zOffset = 127;
    float zLowerLimit = 0;
    float mx;
    float my;
    float mz;
    joystickData jsData;
};

#endif // BBMJOYSTICK_H
