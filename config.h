#ifndef CONFIG_H
#define CONFIG_H

#include "events.h"
#include <vector>
#include <map>
#include <utility>
#include <linux/input.h>

/*colors*/
#define PRESET_COLOR 0x555555
#define CAMERA_COLOR 0x550050
#define SPP_COLOR    0x2483D6
#define ACT_PRESET_COLOR 0x00FF00

/*GPIO Tasten*/
#define TASTE1 19
#define TASTE2 13
#define TASTE3 6
#define TASTE4 5
#define TASTE5 22
#define TASTE6 27
#define TASTE7 12
#define TASTE8 25
#define TASTE9 24
#define TASTE10 23
#define TASTE11 18
#define TASTE12 26

#define UPPER_ROW 1
#define LOWER_ROW 0
#define NUMBER_OF_BUTTONS 6

/*SYS Class Paths*/
#define EXPORT ">/sys/class/gpio/export"
#define UNEXPORT ">/sys/class/gpio/unexport"
#define SYSPATH "/sys/class/gpio/gpio"
#define EDGE "/edge"
#define DIRECTION "/direction"
#define VALUE "/value"

/*SPI*/
#define SPI0 "/dev/spidev0.0"
#define SPI1 "/dev/spidev0.1"
#define SPI_MODE 0
#define BITS_PER_W 8
#define SPI_SPEED 2500000

/*Time in us*/
#define DEBOUNCE_T 20
#define CREEP_T    100000
#define TX_T       150000

/*others*/
#define TRUE 1
#define FALSE 0
#define NUMBER_OF_SLOTS 6
#define TIMER_OUT 0

/*property array*/
#define VAL 0
#define MIN 1
#define MAX 2
#define TYP 3

/*properties*/
#define V_HEADNR              0
#define V_IRIS                1
#define V_PED                 2
#define V_FOCUS               3
#define V_W_RED               4
#define V_W_BLUE              5
#define V_B_RED               6
#define V_B_BLUE              7
#define V_GAIN                8
#define V_GAMMA               9
#define V_GAMMA_TAB           10
#define V_DETAIL              11
#define V_COLOR               12
#define V_COL_TEMP            13
#define V_KNEE                14
#define V_KNEE_POINT          15
#define V_ND_FILTER           16
#define V_SHUTTER             17
#define V_PT_SPEED            18
#define V_TRANS_SPEED         19
#define V_RAMP                20
#define V_SPP1                21
#define V_SPP2                22
#define V_SPP_WAIT_TIME       23
#define V_BOUNCE_ZOOM_SPEED   24
#define V_HEAD_POWER          25
#define V_MIRROR              26


#define ROW_ENTRIES           27
#define COLUM_ENTRIES         6
#define NORMAL                0
#define CENTER                1
#define TEXT                  2
#define NAN                   3
#define OFFSET                4
#define REQUESTABLE           1
#define ARRIVED               1

#define NUMBER_OF_FLAGS       15
/*flags*/
#define F_PRST_IN_STORE       0
#define F_BOUNCING            1
#define F_SPP_ON              2
#define F_FASTTRANS           3
#define F_BOUNCE_ENABLE       4
#define F_CAMERA_KNOWN        5
#define F_CONNECTED           6
#define F_PRESET_MOVE         7
#define F_RECEIVED_ALL        8
#define F_BOUNCE_ABORTED      9
#define F_PAN_INVERT          10
#define F_TILT_INVERT         11
#define F_ZOOM_INVERT         12
#define F_FOCUS_INVERT        13
#define F_TRAVELLING_INVERT   14

/*set,get types*/
#define ABS                 1
#define INC                 2
#define DISP                3
#define NORM                4


#define INTERNAL            0
#define SEND                1

#define I_XPT_DESTINATION     0
#define I_XPT_IP_FIELD_1      1
#define I_XPT_IP_FIELD_2      2
#define I_XPT_IP_FIELD_3      3
#define I_XPT_IP_FIELD_4      4
#define I_XPT_SOURCE          5
#define I_XPT_TYPE_BLACKMAGIC 0
#define I_XPT_TYPE_ROSS       1



/*SPP States*/
#define S_SPP_IDLE            0
#define S_SPP_GOTO1           1
#define S_SPP_GOTO2           2
#define S_SPP_WAIT1           3
#define S_SPP_WAIT2           4

typedef enum {
    kAxisPan,
    kAxisTilt,
    kAxisZoom,
    kAxisFocus,
    kAxisTravelling,
    kAxisMax
} axis_t;

typedef enum {
    kControlNone,
    kControlJoystickX,
    kControlJoystickY,
    kControlJoystickZ,
    kControlZoomRocker,
    kControlFocusWheel
} control_t;

static const char* axisStr(axis_t a) {
    return a == kAxisPan ? "Pan" :
            a == kAxisTilt ? "Tilt" :
            a == kAxisZoom ? "Zoom" :
            a == kAxisFocus ? "Focus" :
            a == kAxisTravelling ? "Travelling" :
                "unknown";
}

static const char* controlStr(control_t c) {
    return c == kControlNone ? "none" :
            c == kControlJoystickX ? "Joystick Left/Right" :
            c == kControlJoystickY ? "Joystick Up/Down" :
            c == kControlJoystickZ ? "Joystick Rotation" :
            c == kControlZoomRocker ? "Zoom Rocker" :
            c == kControlFocusWheel ? "Focus Wheel" :
                "unknown";
}

const std::map<int, std::pair<int, std::vector<int>>>  keyboardmap =
{                                           /* key      event             data */
                                           { KEY_1,  { E_CAMERA_CHANGE, { 0 }}},
                                           { KEY_2,  { E_CAMERA_CHANGE, { 1 }}},
                                           { KEY_3,  { E_CAMERA_CHANGE, { 2 }}},
                                           { KEY_4,  { E_CAMERA_CHANGE, { 3 }}},
                                           { KEY_5,  { E_CAMERA_CHANGE, { 4 }}},
                                           { KEY_6,  { E_CAMERA_CHANGE, { 5 }}},
                                           { KEY_F1, { E_PRESET_CHANGE, { 0 }}},
                                           { KEY_F2, { E_PRESET_CHANGE, { 1 }}},
                                           { KEY_F3, { E_PRESET_CHANGE, { 2 }}},
                                           { KEY_F4, { E_PRESET_CHANGE, { 3 }}},
                                           { KEY_F5, { E_PRESET_CHANGE, { 4 }}},
                                           { KEY_F6, { E_PRESET_CHANGE, { 5 }}} };
const std::map<int, std::vector<int>> ocpmap =
{
                                        /* key      value   data */
                                       { KEY_A,  { V_IRIS,    1 }},
                                       { KEY_B,  { V_IRIS,   -1 }},
                                       { KEY_C,  { V_PED,     1 }},
                                       { KEY_D,  { V_PED,    -1 }},
                                       { KEY_E,  { V_B_RED,   1 }},
                                       { KEY_F,  { V_B_RED,  -1 }},
                                       { KEY_G,  { V_B_BLUE,  1 }},
                                       { KEY_H,  { V_B_BLUE, -1 }},
                                       { KEY_I,  { V_W_RED,   1 }},
                                       { KEY_J,  { V_W_RED,  -1 }},
                                       { KEY_K,  { V_W_BLUE,  1 }},
                                       { KEY_L,  { V_W_BLUE, -1 }},
                                       { KEY_M,  { V_GAIN,    1 }},
                                       { KEY_N,  { V_GAIN,   -1 }},
                                       { KEY_O,  { V_SHUTTER, 1 }},
                                       { KEY_P,  { V_SHUTTER,-1 }},};

#define AXES_UPDATE_INTERVAL_MS 20 /* update axis values every 20ms (50Hz) */

#define AXIS_NO_VALUE_REL (INT8_MIN)
#define AXIS_NO_VALUE_ABS (INT16_MIN)

#endif // CONFIG_H
