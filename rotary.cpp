#include "rotary.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string>


Rotary::Rotary()
{

}

Rotary::~Rotary()
{
    close(fd_sense);
    close(fd_button);
    close(fd_ic2);
    std::string out;
    out = "echo '" + std::to_string(button_gpio) + "' >/sys/class/gpio/unexport";
    system(out.c_str());
    out = "echo '" + std::to_string(sense_gpio) + "' >/sys/class/gpio/unexport";
    system(out.c_str());
}

/*Initialization*/
int Rotary::init(int sense_gpio, int button_gpio, uint8_t slaveAddr){
    this->sense_gpio = sense_gpio;
    this->button_gpio = button_gpio;
    this->slaveAddr = slaveAddr;

    /*GPIO setup sense signal*/
    std::string out;
    out = "echo '" + std::to_string(sense_gpio) + "' >/sys/class/gpio/export";
    system(out.c_str());
    out = "echo 'in' >/sys/class/gpio/gpio" + std::to_string(sense_gpio) + "/direction";
    system(out.c_str());
    out = "echo 'falling' >/sys/class/gpio/gpio" + std::to_string(sense_gpio) + "/edge";
    system(out.c_str());
    out = "/sys/class/gpio/gpio" + std::to_string(sense_gpio) + "/value";

    fd_sense = open(out.c_str(), O_RDONLY);
    if (fd_sense < 0) {
        return fd_sense;
    }

    /*GPIO setup button signal*/
    out = "echo '" + std::to_string(button_gpio) + "' >/sys/class/gpio/export";
    system(out.c_str());
    out = "echo 'in' >/sys/class/gpio/gpio" + std::to_string(button_gpio) + "/direction";
    system(out.c_str());
    out = "echo 'falling' >/sys/class/gpio/gpio" + std::to_string(button_gpio) + "/edge";
    system(out.c_str());
    out = "/sys/class/gpio/gpio" + std::to_string(button_gpio) + "/value";

    fd_button = open(out.c_str(), O_RDONLY);
    if (fd_button < 0) {
        return fd_button;
    }



    /*I2C setup*/
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

/*Read sense GPIO*/
int Rotary::readSense(){
    lseek(fd_sense, 0, SEEK_SET);               //Reset read position
    rot_err = read(fd_sense, &sense_val, 1);

// obsolete ??
//    if (sense_val == '1') {
//      res = 1;
//    }
//    else{
//      res = 0;
//    }
     return rot_err;

}

/*Read button GPIO*/
int Rotary::readButton(){
    lseek(fd_button, 0, SEEK_SET);              //Reset read position
    rot_err = read(fd_button, &button_val, 1);

    if (button_val == '1') {
      return 1;
    }
    else{
      return 0;
    }
}


/*Read rotary over i2c*/
int Rotary::readRotary(int8_t& res){

    char obuf[1] = {0};
    char ibuf[1] = {0};
    rot_err = write(fd_ic2,obuf,1);
        if (rot_err < 0) {
            return rot_err;
            }
    read(fd_ic2,ibuf,1);
    rotary_val = int8_t(ibuf[0]);
        if (rot_err < 0) {
            return rot_err;
           }
    if(rotary_val < -80 || rotary_val > 80){       // i2c error workaround
        rotary_val = 0;
    }
    res = rotary_val;

    return 0;
}
