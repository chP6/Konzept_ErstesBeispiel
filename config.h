#ifndef CONFIG_H
#define CONFIG_H

/*colors*/
#define PRESET_COLOR 0xFFFFFF
#define CAMERA_COLOR 0xFF00F0
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

/*Debounce Time in us*/
#define DEBOUNCE_T 20 //in us
#define CREEP_T    100000
#define TX_T       150000

/*others*/
#define TRUE 1
#define FALSE 0
#define NUMBER_OF_SLOTS 6

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
#define COLUM_ENTRIES         5
#define NORMAL                0
#define CENTER                1
#define TEXT                  2
#define NAN                   3
#define OFFSET                4

#define NUMBER_OF_FLAGS       11
/*flags*/
#define F_PRST_IN_STORE       0
#define F_BOUNCING            1
#define F_SPP_ON              2
#define F_FASTTRANS           3
#define F_BOUNCE_ENABLE       4
#define F_KNOWN               5
#define F_CONNECTED           6
#define F_X_INVERT            7
#define F_Y_INVERT            8
#define F_Z_INVERT            9
#define F_FOCUSINVERT         10

/*set,get types*/
#define ABS                 1
#define INC                 2
#define DISP                3


#define INTERNAL            0
#define SEND                1

#define I_XPT_DESTINATION     0
#define I_XPT_IP_FIELD_1      1
#define I_XPT_IP_FIELD_2      2
#define I_XPT_IP_FIELD_3      3
#define I_XPT_IP_FIELD_4      4
#define I_XPT_SOURCE          5



/*SPP States*/
#define S_SPP_IDLE            0
#define S_SPP_GOTO1           1
#define S_SPP_GOTO2           2
#define S_SPP_WAIT1           3
#define S_SPP_WAIT2           4

#endif // CONFIG_H
