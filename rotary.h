#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>

class Rotary
{
public:
    Rotary();
    int init(int gpioNo);
    int readSense(int &res);
    int fd;
private:
    int gpioNo;
    char sense_val;
    int rot_err;
};

#endif // ROTARY_H
