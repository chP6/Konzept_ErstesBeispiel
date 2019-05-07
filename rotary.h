#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>

#define I2C_DEV_PATH    "/dev/i2c-1"        // mount point of i2c kernel module

class Rotary
{
public:
    Rotary();
    ~Rotary();
    int init(int sense_gpio, int button_gpio, uint8_t slaveAddr);
    int readSense();
    int readRotary(int8_t& res);
    int readButton();
    int fd_sense, fd_ic2, fd_button;
private:
    int sense_gpio, button_gpio;
    uint8_t sense_val, button_val, slaveAddr;
    int8_t rotary_val;
    int rot_err;
};

#endif // ROTARY_H
