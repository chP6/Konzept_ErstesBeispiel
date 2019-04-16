#include "telegrambuilder.h"
#include "bbmcommandtypes.h"

Telegrambuilder::Telegrambuilder()
{

}

// Byte 6...9 Data -> [5...8]
void Telegrambuilder::encode(int bbm_dev_no, int bbm_command, std::vector<int> data, uint8_t* telegram){

    datagram[0]=SYNCHRO;
    datagram[1]=TYPED;
    datagram[2]=SYNCHRO;
    datagram[3]=(uint8_t)bbm_dev_no;
    datagram[4]=(uint8_t)bbm_command;
    datagram[5]= 0;             //default, weil meist 0
    datagram[6]= 0;
    datagram[7]= 0;
    datagram[8]= 0;
  //datagram[9]= chkSum;


    switch (bbm_command) {
    case TILT_PAN:
        datagram[5] = (uint8_t)data[0];
        datagram[6] = (uint8_t)(data[0]>>8);
        datagram[7] = (uint8_t)data[1];
        datagram[8] = (uint8_t)(data[1]>>8);
        break;
    case IRIS_OPEN:
    case RAMP:
    case PAN_TILT_SPEED:
    case PAN_TILT_REVERSE:
    case ZOOM_SET_ABSOLUTE:
    case FOCUS_SET_ABSOLUTE:
    case ZOOM_FOCUS_SET:
    case WATCHDOG:
        datagram[5] = 1;
        datagram[6] = HEAD;
        datagram[7] = 0;
        datagram[8] = 0;
        break;
    case CAMERA_GAIN_UP:
    case AUTOIRIS_ON:
    case AUTOIRIS_OFF:
    case WHITE_BALANCE_PRST:
    case MIRROR_H_V:
    case SHUTTER_UP:
    case KNEE_POINT_AUTO:
    case DETAIL_LEVEL_DP_ADJ:
    case DETAIL_HV_ADJ:
    case DETAIL_FREQ_ADj:
    case DETAIL_CRISP_ADJ:
    case CALIBRATE_HEAD:
    case STORE_PRESET:
        datagram[8] = (uint8_t)data[0];
        break;
    case GOTO_PRESET:
        datagram[8] = (uint8_t)data[0];
        break;
    case RED_GAIN_ADJ_UP:
    case BLUE_GAIN_ADJ_UP:
    case DETAIL_LEVEL_ADJ:
    case DETAIL_FREQ_ADJ:
    case AUTO_BLACK_BALANCE:
    case COLOR_UP:
    case MASTER_PED_UP:
    case BARS:
    case RED_PED_UP:
    case GREEN_PED_UP:
    case BLUE_PED_UP:
    case GAMMA:
    case GAMMA_TABLE:
    case DYN_NOISE_REDUCTION:
    case MATRIX_LIN_RG:
    case MATRIX_LIN_GB:
    case MATRIX_LIN_BR:
    case MATRIX_LIN_GR:
    case MATRIX_LIN_BG:
    case MATRIX_LIN_RB:
    case MATRIX_HUE_R:
    case MATRIX_HUE_Y:
    case MATRIX_HUE_G:
    case MATRIX_HUE_C:
    case MATRIX_HUE_B:
    case MATRIX_HUE_M:
    case MATRIX_SAT_R:
    case MATRIX_SAT_Y:
    case MATRIX_SAT_G:
    case MATRIX_SAT_C:
    case MATRIX_SAT_B:
    case MATRIX_SAT_M:
    case HEAD_POWER:
    case TILT_UPPER_LIMIT:
    case TILT_LOWER_LIMIT:
    case TILT_CLEAR_LIMIT:
    case SET_FOCUS_PUSH:
    case BRIGHT_SET:
    case BLC_ON:
    case BLC_OFF:
    case STABILIZER:
    case THREED_DNR:
    case FLICKERLESS:
    case BNCE_ZOOM_TELE_SET:
    case BNCE_ZOOM_START_SET:
    case BNCE_ZOOM_START_STP:
    case BMD_SET_ROUTE:
    case BMD_SET_SOLO:
    case BNT_TALLY_ON_OFF:
    case BNT_TEMPERATURE:
    case CAMERA_RESET:
    case HEAD_STARTUP:
    case H_DETAIL_LEVEL_ADJ:
    case V_DETAIL_LEVEL_ADJ:
    case FOG_CORRECT:
    default:
        break;
    }
    datagram[9] = calc_chksum();

    for(int i = 0; i < 10; ++i) {
        telegram[i] = datagram[i];
    }
}


void decode(uint8_t* telegram, int bbm_command, int dataOut[4]){

}



uint8_t Telegrambuilder::calc_chksum(){     // quersumme bytes 4-8
    chkSum = 0;
    for(int i=3;i<9;i++)
        {
            chkSum+=datagram[i];
        }
    chkSum=(chkSum ^ 0b11111111);     // XOR -> 2er Komplement
    return chkSum;
}
