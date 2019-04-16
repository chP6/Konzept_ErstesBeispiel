#include "rotary.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


Rotary::Rotary()
{
}

int Rotary::init(int gpioNo){
    this->gpioNo = gpioNo;
    //todo: implement variable gpioNo

    system("echo '4' >/sys/class/gpio/export");
    system("echo 'in' >/sys/class/gpio/gpio4/direction");
    system("echo 'falling' >/sys/class/gpio/gpio4/edge");
    fd = open("/sys/class/gpio/gpio4/value", O_RDONLY);

    return fd;
}

//todo: sensevalue nicht mehr zurückgeben, unnötig für debug schön
int Rotary::readSense(int &res){
    lseek(fd, 0, SEEK_SET);		//Leseposition wieder auf Null setzen
    rot_err = read(fd, &sense_val, 1);
    if (sense_val == '1') {
      res = 1;
    }
    else{
      res = 0;
    }

    return rot_err;
}
