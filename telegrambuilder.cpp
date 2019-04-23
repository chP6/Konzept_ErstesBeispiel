#include "telegrambuilder.h"
#include "bbmcommandtypes.h"

Telegrambuilder::Telegrambuilder()
{

}

// Byte 6...9 Data -> [5...8]
void Telegrambuilder::encode(bool request, int bbm_dev_no, int bbm_command, std::vector<int> data, uint8_t* telegram){

    datagram[0]=SYNCHRO;
    datagram[1]=TYPED_TO_HC;
    datagram[2]=SYNCHRO;
    datagram[3]=(uint8_t)bbm_dev_no;
    datagram[4]=(uint8_t)bbm_command;
    datagram[5]= 0;             //default, weil meist 0
    datagram[6]= 0;
    datagram[7]= 0;
    datagram[8]= 0;
  //datagram[9]= chkSum;

    if(request){
        datagram[1]=TYPEI_REQUEST;
    }
    else{
        switch (bbm_command) {
        case TILT_PAN:
            datagram[5] = (uint8_t)data[0];
            datagram[6] = (uint8_t)(data[0]>>8);
            datagram[7] = (uint8_t)data[1];
            datagram[8] = (uint8_t)(data[1]>>8);
            break;
        case IRIS_OPEN:
            break;
        case RAMP:
            break;
        case PAN_TILT_SPEED:
            break;
        case ZOOM_SET_ABSOLUTE:
            datagram[5] = 0;
            datagram[6] = 0;
            datagram[7] = 0;
            datagram[8] = 0;
            break;
        case FOCUS_SET_ABSOLUTE:
            datagram[5] = 0;                                //(ignored) zoom value
            datagram[6] = 0;
            datagram[7] = (uint8_t)(data[0]>>8);            //focus value
            datagram[8] = (uint8_t)data[0];                 //focus value
            break;
        case ZOOM_FOCUS_SET:
            datagram[5] = 0;
            datagram[6] = (uint8_t)data[0];                 //zoom value
            datagram[7] = 0;                                //(ignored) focus value
            datagram[8] = 0;                                //(ignored) focus value
            break;
        case WATCHDOG:
            datagram[5] = 1;
            datagram[6] = HEAD;
            datagram[7] = 0;
            datagram[8] = 0;
            break;
        case CAMERA_GAIN_UP:
            break;
        case AUTOIRIS_ON:
            break;
        case AUTOIRIS_OFF:
            break;
        case WHITE_BALANCE_PRST:
            break;
        case MIRROR_H_V:
            break;
        case SHUTTER_UP:
            break;
        case KNEE_POINT_AUTO:
            break;
        case CALIBRATE_HEAD:
            break;
        case STORE_PRESET:
            datagram[8] = 1;    //preset number
            break;
        case GOTO_PRESET:
            datagram[8] = 1;    //preset number
            break;
        case RED_GAIN_ADJ_UP:
            break;
        case BLUE_GAIN_ADJ_UP:
            break;
        case COLOR_UP:
            break;
        case MASTER_PED_UP:
            break;
        case RED_PED_UP:
            break;
        case GREEN_PED_UP:
            break;
        case BLUE_PED_UP:
            break;
        case GAMMA:
            break;
        case GAMMA_TABLE:
            break;
        case HEAD_POWER:
            break;
        case TILT_UPPER_LIMIT:
            break;
        case TILT_LOWER_LIMIT:
            break;
        case TILT_CLEAR_LIMIT:
            break;
        case SET_FOCUS_PUSH:
            break;
        case BNCE_ZOOM_TELE_SET:
            break;
        case BNCE_ZOOM_START_SET:
            break;
        case BNCE_ZOOM_START_STP:
            break;
        case BNT_TALLY_ON_OFF:
            break;
        case BNT_TEMPERATURE:
            break;
        case CAMERA_RESET:
            break;
        case HEAD_STARTUP:
            break;
        default:
            break;
        }
    }

    datagram[9] = calc_chksum();

    for(int i = 0; i < 10; ++i) {
        telegram[i] = datagram[i];
    }
}


void Telegrambuilder::decode(uint8_t* telegram, struct answer_s& answer){
    int temp;
    answer.type = telegram[1];
    answer.command = telegram[4];
    answer.from = telegram[3];

    //Server Watchdog only answer not from camera/head:
    if(answer.from == SERVER){
        //NOP
    }
    else{
        switch (answer.command) {
        case TILT_PAN:
            break;
        case IRIS_OPEN:
            break;
        case RAMP:
            break;
        case ZOOM_SET_ABSOLUTE:
            break;
        case FOCUS_SET_ABSOLUTE:
            break;
        case ZOOM_FOCUS_SET:
            break;
        case WATCHDOG:
            answer.data.push_back(telegram[5]);             // headStatus
            answer.data.push_back(telegram[6]);             // cameraType
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // voltage
            answer.data.push_back(temp);
            break;
        case CAMERA_GAIN_UP:
            break;
        case AUTOIRIS_ON:
        case AUTOIRIS_OFF:
        case WHITE_BALANCE_PRST:
        case MIRROR_H_V:
        case SHUTTER_UP:
        case KNEE_POINT_AUTO:
        case STORE_PRESET:
        case GOTO_PRESET:
        case RED_GAIN_ADJ_UP:
        case BLUE_GAIN_ADJ_UP:
        case COLOR_UP:
        case MASTER_PED_UP:
        case BARS:
        case RED_PED_UP:
        case GREEN_PED_UP:
        case BLUE_PED_UP:
        case GAMMA:
        case GAMMA_TABLE:
        case HEAD_POWER:
        case BNT_TEMPERATURE:
        case HEAD_STARTUP:
        default:
            break;
        }
    }
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
