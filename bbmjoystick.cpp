#include "bbmjoystick.h"
#include <string.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


BBMJoystick::BBMJoystick()
{
    val_x = 5000;
    val_x_old = 5000;
    val_y = 5000;

    memset(axis,0,sizeof(axis));
    joystick_fd=open("/dev/input/js0",O_RDONLY);                //Pfad noch anpassen
    if(joystick_fd<0)
        error(joystick_fd,errno,"couldn't open device");

     for (int i=0;i<4;i++) {                                    // Joystick initial auslesen: 1x Button, 3x Axis
         read(joystick_fd, &js, sizeof(struct js_event));
     }
}


joystickData BBMJoystick::processEvent(){

    jsData.button_val = 0;
    readErr=read(joystick_fd, &js, sizeof(struct js_event));
    if(readErr<0)
        error(readErr,errno,"failed at reading device");


    switch(js.type & ~JS_EVENT_INIT) {
    case JS_EVENT_AXIS:
        axis[js.number] = js.value;

        val_x=(int)((0.152587890625*axis[0])+5000);         // conversion joystick values -> camera values
        val_y=(int)((0.152587890625*axis[1])+5000);

        if (val_x_old == 9999 && val_x == 5000) {           // X-Achsen Bug Joystick Workaround
            val_x = 9999;                                   //
        }                                                   //
        else if (val_x_old == 0 && val_x == 5000) {         //
            val_x = 0;                                      //
        }                                                   //
        val_x_old = val_x;                                  //

        jsData.debug[0] = val_x;
        jsData.debug[1] = val_y;
//        jsData.tilt_val[0]=(uint8_t)val_x;
//        jsData.tilt_val[1]=(uint8_t)(val_x>>8);
//        jsData.tilt_val[2]=(uint8_t)val_y;
//        jsData.tilt_val[3]=(uint8_t)(val_y>>8);
        break;

    case JS_EVENT_BUTTON:
        jsData.button_val = (char)js.value;
        break;
    default:
        break;
    }
    return jsData;
}
