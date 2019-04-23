#include "rotary.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "smbus.h"
#include <string>


Rotary::Rotary()
{
}

Rotary::~Rotary()
{
    close(fd_sense);
    close(fd_ic2);
}

int Rotary::init(int gpioNo, uint8_t slaveAddr){
    this->gpioNo = gpioNo;
    this->slaveAddr = slaveAddr;

    // Sense Leitung
    std::string out;
    out = "echo '" + std::to_string(gpioNo) + "' >/sys/class/gpio/export";
    system(out.c_str());    // system("echo '4' >/sys/class/gpio/export");

    out = "echo 'in' >/sys/class/gpio/gpio" + std::to_string(gpioNo) + "/direction";
    system(out.c_str());    // system("echo 'in' >/sys/class/gpio/gpio4/direction");

    out = "echo 'falling' >/sys/class/gpio/gpio" + std::to_string(gpioNo) + "/edge";
    system(out.c_str());    // system("echo 'falling' >/sys/class/gpio/gpio4/edge");

    out = "/sys/class/gpio/gpio" + std::to_string(gpioNo) + "/value";
    fd_sense = open(out.c_str(), O_RDONLY);
    if (fd_sense < 0) {
        return fd_sense;
    }

    // I2C
    char *i2cFilename = I2C_DEV_PATH;
    fd_ic2 = open(i2cFilename, O_RDWR);
    if (fd_ic2 < 0) {
        return fd_ic2;
    }
    rot_err = ioctl(fd_ic2, I2C_SLAVE, slaveAddr);
    if (rot_err < 0) {
        return rot_err;
    }

    return 0;
}


int Rotary::readSense(){
    lseek(fd_sense, 0, SEEK_SET);		//Leseposition wieder auf Null setzen
    rot_err = read(fd_sense, &sense_val, 1);

//    if (sense_val == '1') {
//      res = 1;
//    }
//    else{
//      res = 0;
//    }
     return rot_err;

}

int Rotary::readRotary(int8_t& res){
    rotary_val = (int8_t)i2c_smbus_read_byte_data(fd_ic2 ,0x00);
    if (rot_err < 0) {
      return rot_err;
    }
    if(rotary_val < -80){       // i2c error workaround
        rotary_val = 0;
    }
    res = rotary_val;

    return 0;
}
