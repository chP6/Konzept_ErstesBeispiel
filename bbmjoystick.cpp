#include "bbmjoystick.h"
#include <string.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "config.h"
#include <QtDebug>


BBMJoystick::BBMJoystick()
{
}

BBMJoystick::~BBMJoystick()
{
    close(joystick_fd);
}

/*Initialization*/
int BBMJoystick::init(){
    axis[0] = 0;
    axis[1] = 0;
    axis[2] = 0;

    val_x = 5000;
    val_x_old = 5000;
    val_y = 5000;

    mx = (xLowerLimit - xyOffset)/INT16_MIN;
    my = (yLowerLimit - xyOffset)/INT16_MIN;
    mz = (zLowerLimit - zOffset)/INT16_MIN;

    memset(axis,0,sizeof(axis));
    joystick_fd=open(JOYSTICK_PATH,O_RDONLY | O_NONBLOCK);        //Nonblock, damit beim aufstarten joystick ohne block x-mal abgefragt werden kann. Sinnvoll ??
    if(joystick_fd<0){
        usleep(1000);
        init();        //erst wenn scharf ??
        error(joystick_fd,errno,"couldn't open device Joystick");
        return -1;
    }
     return 0;
}


/*Read and process joystick event*/
int BBMJoystick::processEvent(joystickData& jsData){
    jsData.buttonVal = 0;
    readErr=read(joystick_fd, &js, sizeof(struct js_event));
    if(readErr<0){
        qDebug("Could not process Joystick Event, %d, %d",readErr, errno);
        return -1;
    }

    switch(js.type & ~JS_EVENT_INIT) {
    case JS_EVENT_AXIS:
        /*Axis actuated*/
        axis[js.number] = js.value;

        /*Debug*/
        qDebug("X/Y/Z: %d,%d,%d",axis[0],axis[1],axis[2]);


        val_x = int(((mx*axis[1])+xyOffset));         // conversion joystick values -> camera values
        val_y = int(((my*axis[0])+xyOffset));
        val_z = int(((mz*axis[2])+zOffset));

        /*X-axis bug workaround*/
        {
            if (val_x_old == 9999 && val_x == 5000) {
                val_x = 9999;
            }
            else if (val_x_old == 0 && val_x == 5000) {
                val_x = 0;
            }
            val_x_old = val_x;
        }

        jsData.xCoord = val_x;
        jsData.yCoord = val_y;
        jsData.zCoord = val_z;
        break;

    case JS_EVENT_BUTTON:
        /*Button actuated*/
        jsData.buttonVal = char(js.value);
        break;
    default:
        break;
    }
    return 0;
}

/*Obsolete ??*/
void BBMJoystick::initRead(){
    for (int i=0;i<3;i++) {
        readErr=read(joystick_fd, &js, sizeof(struct js_event));
        if(readErr<0){
            qDebug("Could not process Joystick Event, %d, %d",readErr, errno);
        }
    }
    qDebug("Read Joystick 3x initially");
}
