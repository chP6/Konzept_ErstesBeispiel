#ifndef BBMCOMMANDTYPES_H
#define BBMCOMMANDTYPES_H

/*Command Types */
#define TILT_PAN			0
#define IRIS_OPEN			1
#define RAMP				2
#define PAN_TILT_SPEED 		5
#define PAN_TILT_REVERSE 	7
#define ZOOM_SET_ABSOLUTE	8
#define FOCUS_SET_ABSOLUTE	9
#define ZOOM_FOCUS_SET		15
#define WATCHDOG			16
#define CAMERA_GAIN_UP		17
#define AUTOIRIS_ON			19
#define AUTOIRIS_OFF		20
#define WHITE_BALANCE_PRST	31
#define MIRROR_H_V			35
#define SHUTTER_UP			37
#define KNEE_POINT_AUTO		38
#define DETAIL_LEVEL_DP_ADJ	39
#define DETAIL_HV_ADJ		40
#define DETAIL_FREQ_ADj		41
#define DETAIL_CRISP_ADJ	42
#define CALIBRATE_HEAD		43
#define	STORE_PRESET		48
#define GOTO_PRESET			49
#define	RED_GAIN_ADJ_UP		54
#define BLUE_GAIN_ADJ_UP	56
#define DETAIL_LEVEL_ADJ	58
#define DETAIL_FREQ_ADJ		59
#define AUTO_BLACK_BALANCE	60
#define COLOR_UP			61
#define MASTER_PED_UP		64
#define BARS				66
#define	RED_PED_UP			67
#define	GREEN_PED_UP		68
#define BLUE_PED_UP			69
#define GAMMA				71
#define	GAMMA_TABLE			72
#define DYN_NOISE_REDUCTION	74
#define MATRIX_LIN_RG		80
#define MATRIX_LIN_GB		81
#define MATRIX_LIN_BR		82
#define MATRIX_LIN_GR		83
#define MATRIX_LIN_BG		84
#define MATRIX_LIN_RB		85
#define MATRIX_HUE_R		87
#define MATRIX_HUE_Y		88
#define MATRIX_HUE_G		89
#define MATRIX_HUE_C		90
#define MATRIX_HUE_B		91
#define MATRIX_HUE_M		92
#define MATRIX_SAT_R		93
#define MATRIX_SAT_Y		94
#define MATRIX_SAT_G		95
#define MATRIX_SAT_C		96
#define MATRIX_SAT_B		97
#define MATRIX_SAT_M		98
#define HEAD_POWER			100
#define	TILT_UPPER_LIMIT	104
#define	TILT_LOWER_LIMIT	105
#define	TILT_CLEAR_LIMIT	106
#define	SET_FOCUS_PUSH		212
#define	BRIGHT_SET			213
#define	BLC_ON				214
#define	BLC_OFF				215
#define	STABILIZER			216
#define	THREED_DNR			217
#define	FLICKERLESS			218
#define	BNCE_ZOOM_TELE_SET	219
#define	BNCE_ZOOM_START_SET	220
#define BNCE_ZOOM_START_STP	221
#define	BMD_SET_ROUTE		230
#define	BMD_SET_SOLO		231
#define	BNT_TALLY_ON_OFF	232
#define	BNT_TEMPERATURE		233
#define CAMERA_RESET		234
#define HEAD_STARTUP		235
#define	H_DETAIL_LEVEL_ADJ	236
#define	V_DETAIL_LEVEL_ADJ	237
#define	FOG_CORRECT			238


/*Datagram Types*/
#define TYPEA 	65				//0x41
#define TYPEB 	66				//0x42
#define TYPEC 	67				//0x43
#define TYPED 	68				//0x44

/*Synchro Character*/
#define SYNCHRO 42				//0x2A

/*Device Types*/
#define HEAD 	0
#define TALLY 	1
#define MMI 	2

#endif // BBMCOMMANDTYPES_H
