#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

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
//#define HeadNr              0
//#define Iris                1
//#define Ped                 2
//#define Focus               3
//#define WhiteRed               4
//#define WhiteBlue              5
//#define BlackRed               6
//#define BlackBlue              7
//#define Gain                8
//#define Gamma               9
//#define GammaTable           10
//#define Detail              11
//#define Color               12
//#define ColorTemp            13
//#define Knee                14
//#define KneePoint          15
//#define NdFilter           16
//#define Shutter             17
//#define PtSpeed            18
//#define TransSpeed         19
//#define Ramp                20
//#define Spp1                21
//#define Spp2                22
//#define SppwWaitTime       23
//#define BounceZoomSpeed   24
//#define HeadPower          25
//#define Mirror              26


#define ROW_ENTRIES           27
#define COLUM_ENTRIES         6
//#define Normal                0
//#define CenterVal                1
//#define Text                  2
//#define Nan                   3
//#define Offset                4
#define REQUESTABLE           1
#define ARRIVED               1

#define NUMBER_OF_FLAGS       15

/*flags*/
//#define StoringPreset       0
//#define Bouncing            1
//#define SppEnabled              2
//#define FastTransEnabled           3
//#define BounceEnabled       4
//#define CameraFamiliar        5
//#define CameraConnected           6
//#define PresetMoving         7
//#define ReceivedAll        8
//#define BounceAborted      9
//#define PanInverted          10
//#define TiltInverted         11
//#define ZoomInverted         12
//#define FocusInverted        13
//#define TravellingInverted   14

/*set,get types*/
//#define Absolute                 1
//#define Incremental                 2
//#define Display                3
//#define Normalized                4


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

namespace Config
{
    Q_NAMESPACE

    typedef enum Property {
        None = -1,
        HeadNr,
        Iris,
        Ped,
        Focus,
        WhiteRed,
        WhiteBlue,
        BlackRed,
        BlackBlue,
        Gain,
        Gamma,
        GammaTable,
        Detail,
        Color,
        ColorTemp,
        Knee,
        KneePoint,
        NdFilter,
        Shutter,
        PtSpeed,
        TransSpeed,
        Ramp,
        Spp1,
        Spp2,
        SppwWaitTime,
        BounceZoomSpeed,
        HeadPower,
        Mirror
    } properties_t;
    Q_ENUM_NS(Property)


    typedef enum {
        Normal,
        CenterVal,
        Nan,
        Text,
        Offset,
        NotReady
    } display_t;

    typedef enum XptTypes {
        Blackmagic,
        Ross
    } xptTypes_t;
        Q_ENUM_NS(XptTypes)

    typedef enum {
        Absolute,
        Incremental,
        Display,
        Normalized
    } value_t;

    typedef enum Flag {
        StoringPreset,
        Bouncing,
        SppEnabled,
        FastTransEnabled,
        BounceEnabled,
        CameraFamiliar,
        CameraConnected,
        PresetMoving,
        ReceivedAll,
        BounceAborted,
        PanInverted,
        TiltInverted,
        ZoomInverted,
        FocusInverted,
        TravellingInverted
    } flags_t;
        Q_ENUM_NS(Flag)

    enum Dial{
        Min,
        Max,
        Value,
        TextList,
        State
    };
    Q_ENUM_NS(Dial)

    static const char* dialStateStr(display_t a) {
        return a == Normal ? "" :
                a == CenterVal ? "center" :
                a == Nan ? "notAvailable" :
                a == Text ? "text" :
                a == Offset ? "" :
                a == NotReady ? "notReady" :
                    "";
    }
    static const char* xptStr(xptTypes_t a) {
        return a == Blackmagic ? "BlackMagic" :
                a == Ross ? "Ross" :
                    "unknown";
    }
    typedef enum Axis {
        KAxisPan,
        KAxisTilt,
        KAxisZoom,
        KAxisFocus,
        KAxisTravelling,
        KAxisMax
    } axis_t;
    Q_ENUM_NS(Axis)

    typedef enum Control {
        KControlNone,
        KControlJoystickX,
        KControlJoystickY,
        KControlJoystickZ,
        KControlZoomRocker,
        KControlFocusWheel
    } control_t;
    Q_ENUM_NS(Control)

    static const char* axisStr(axis_t a) {
        return a == KAxisPan ? "Pan" :
                a == KAxisTilt ? "Tilt" :
                a == KAxisZoom ? "Zoom" :
                a == KAxisFocus ? "Focus" :
                a == KAxisTravelling ? "Travelling" :
                    "unknown";
    }

    static const char* controlStr(control_t c) {
        return c == KControlNone ? "none" :
                c == KControlJoystickX ? "Joystick Left/Right" :
                c == KControlJoystickY ? "Joystick Up/Down" :
                c == KControlJoystickZ ? "Joystick Rotation" :
                c == KControlZoomRocker ? "Zoom Rocker" :
                c == KControlFocusWheel ? "Focus Wheel" :
                    "unknown";
    }
}





#define AXES_UPDATE_INTERVAL_MS 20 /* update axis values every 20ms (50Hz) */

#define AXIS_NO_VALUE_REL (INT8_MIN)
#define AXIS_NO_VALUE_ABS (INT16_MIN)

#endif // CONFIG_H
