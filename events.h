#ifndef EVENTS_H
#define EVENTS_H

#define E_NULLEVENT             0
#define E_INCREASE              1
#define E_CLEAR                 2
#define E_SET_TILT              3
#define E_TX_SERV_WATCHDOG      4
#define E_RX_SERV_WATCHDOG      5
#define E_RX_CAMERA             6
#define E_STORE_PRESET          7
#define E_PRESET_CHANGE         8
#define E_CAMERA_CHANGE         9
#define E_FOCUS_CHANGE          10
#define E_SET_ZOOM              11
#define E_RX_CAMERA_WATCHDOG    12
#define E_IRIS_CHANGE           13
#define E_PED_CHANGE            14
#define E_WBLUE_CHANGE          15
#define E_WRED_CHANGE           16
#define E_BBLUE_CHANGE          17
#define E_BRED_CHANGE           18
#define E_GAIN_CHANGE           19
#define E_SHUTTER_CHANGE        20
#define E_AUTOFOCUS             21
#define E_AUTOFOCUS_ANSWER      22
#define E_FAST_TRANS            23
#define E_BOUNCE                24
#define E_WIDESET               25
#define E_CAMERA_ANSWER         26
#define E_SETUP_HEAD            27
#define E_CALIB_HEAD            28
#define E_SET_UPPER_LIMIT       29
#define E_SET_LOWER_LIMIT       30
#define E_CLEAR_LIMIT           31
#define E_BOUNCE_BLINK          32
#define E_SPP_START             33
#define E_SPP_ABORT             34
#define E_SPP_WAIT_DONE         35
#define E_PRESET_REACHED        36
#define E_XPT_CONNECT           37
#define E_XPT_WATCHDOG          38
#define E_RX_ADJ_RCP_CMD        39
#define E_REQ_SETTINGS_TIMER    40
#define E_REQUEST_SETTINGS      41
#define E_EXT_PRESET_CHANGE     42
#define E_FAST_IRIS             43



#define E_STORE_PRESET_DEBUG        102
#define E_GOTO_PRESET               101
#define E_WRITE_SAVEFILE            103
#define E_LOAD_SAVEFILE             104
#define E_WRITE_AUTOSAVE            105
#define E_LOAD_AUTOSAVE             106

//Debug
#define E_REQ_TEST          111


#endif // EVENTS_H
