#include "telegrambuilder.h"
#include "bbmcommandtypes.h"
#include "config.h"

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
            datagram[5] = (uint8_t)(data[0]>>8);
            datagram[6] = (uint8_t)data[0];
            datagram[7] = (uint8_t)(data[0]>>8);
            datagram[8] = (uint8_t)data[0];
            break;
        case RAMP:
            datagram[5] = 1;
            datagram[6] = (uint8_t)data[0];
            break;
        case PAN_TILT_SPEED:
            datagram[5] = 1;
            datagram[6] = (uint8_t)data[0];
            break;
        case ZOOM_SET_ABSOLUTE:
            // only for answer?
            // all zeros
            break;
        case FOCUS_SET_ABSOLUTE:
            // only for focus
            datagram[5] = 0;                                //(ignored) zoom value
            datagram[6] = 0;
            datagram[7] = (uint8_t)(data[0]>>8);            //focus value
            datagram[8] = (uint8_t)data[0];                 //focus value
            break;
        case ZOOM_FOCUS_SET:
            // only for zoom
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
            datagram[5] = 1;
            datagram[8] = (uint8_t)data[0];
            break;
        case AUTOIRIS_ON:
            datagram[5] = 1;
            datagram[6] = 1;
            break;
        case AUTOIRIS_OFF:
            datagram[5] = 1;
            datagram[6] = 0;
            break;
        case WHITE_BALANCE_PRST:
            datagram[5] = (uint8_t)(data[0]>>8);
            datagram[6] = (uint8_t)data[0];
            break;
        case MIRROR_H_V:
            datagram[5] = 1;
            datagram[6] = (uint8_t)data[0];
            break;
        case SHUTTER_UP:
            datagram[5] = (uint8_t)(data[0]>>8);
            datagram[6] = (uint8_t)data[0];
            break;
        case KNEE_POINT_AUTO:
            datagram[5] = 1;
            datagram[6] = (uint8_t)data[0];
            break;
        case CALIBRATE_HEAD:
            // all zeros
            break;
        case STORE_PRESET:
            datagram[8] = (uint8_t)data[0];
            break;
        case GOTO_PRESET:
            datagram[8] = (uint8_t)data[0];
            break;
        case RED_GAIN_ADJ_UP:
            datagram[7] = (uint8_t)(data[0]>>8);
            datagram[8] = (uint8_t)data[0];
            break;
        case BLUE_GAIN_ADJ_UP:
            datagram[7] = (uint8_t)(data[0]>>8);
            datagram[8] = (uint8_t)data[0];
            break;
        case COLOR_UP:
            datagram[5] = 1;
            datagram[6] = (uint8_t)data[0];
            break;
        case MASTER_PED_UP:
            datagram[7] = (uint8_t)data[0]>>8;
            datagram[8] = (uint8_t)data[0];
            break;
        case RED_PED_UP:
            datagram[7] = (uint8_t)data[0]>>8;
            datagram[8] = (uint8_t)data[0];
            break;
        case GREEN_PED_UP:
            datagram[5] = (uint8_t)(data[0]>>8);
            datagram[6] = (uint8_t)data[0];
            break;
        case BLUE_PED_UP:
            datagram[7] = (uint8_t)data[0]>>8;
            datagram[8] = (uint8_t)data[0];
            break;
        case GAMMA:
            datagram[5] = (uint8_t)(data[0]>>8);
            datagram[6] = (uint8_t)data[0];
            break;
        case GAMMA_TABLE:
            datagram[5] = 1;
            datagram[6] = (uint8_t)data[0];
            break;
        case HEAD_POWER:
            datagram[5] = 1;
            datagram[6] = (uint8_t)data[0];
            break;
        case TILT_UPPER_LIMIT:
            // all zeros
            break;
        case TILT_LOWER_LIMIT:
            // all zeros
            break;
        case TILT_CLEAR_LIMIT:
            // all zeros
            break;
        case SET_FOCUS_PUSH:
            // all zeros
            break;
        case BNCE_ZOOM_TELE_SET:
            datagram[5] = 1;
            break;
        case BNCE_ZOOM_WIDE_SET:
            datagram[5] = 1;
            break;
        case BNCE_ZOOM_START:
            datagram[5] = 1;
            break;
        case BNT_TEMPERATURE:
            datagram[7] = (uint8_t)(data[0]>>8);
            datagram[8] = (uint8_t)data[0];
            break;
        case CAMERA_RESET:
            datagram[5] = 1;
            break;
        case HEAD_STARTUP:
            // all zeros
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

    //watchdog answer from server
    if(answer.from == SERVER){
        //NOP
    }
    else{ //watchdog answer from camera/head
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
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [0]focus value
            answer.data.push_back(temp);
            break;
        case ZOOM_FOCUS_SET:
            break;
        case WATCHDOG:
            answer.data.push_back(telegram[5]);             // [0]headStatus
            answer.data.push_back(telegram[6]);             // [1]cameraType
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [2]voltage
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
