#include "telegrambuilder.h"
#include "bbmcommandtypes.h"
#include "config.h"

Telegrambuilder::Telegrambuilder()
{

}


/*Encodes BBMNet telegrams*/
// Byte 6...9 Data -> [5...8]
void Telegrambuilder::encode(bool request, int bbm_dev_no, int bbm_command, std::vector<int> data, uint8_t* telegram){

    datagram[0]=SYNCHRO;
    datagram[1]=TYPED_TO_HC;
    datagram[2]=SYNCHRO;
    datagram[3]=uint8_t(bbm_dev_no);
    datagram[4]=uint8_t(bbm_command);
    datagram[5]= 0;             //default, mostly 0
    datagram[6]= 0;
    datagram[7]= 0;
    datagram[8]= 0;
  //datagram[9]= chkSum;

    /*Telegram is a request*/
    if(request){
        datagram[1]=TYPEI_REQUEST;
    }
    /*Telegram is a command*/
    else{
        switch (bbm_command) {
        case TILT_PAN:
            datagram[5] = uint8_t(data[0]);
            datagram[6] = uint8_t((data[0]>>8));
            datagram[7] = uint8_t(data[1]);
            datagram[8] = uint8_t((data[1]>>8));
            break;
        case IRIS_OPEN:
            datagram[5] = uint8_t((data[0]>>8));
            datagram[6] = uint8_t(data[0]);
            datagram[7] = uint8_t((data[0]>>8));
            datagram[8] = uint8_t(data[0]);
            break;
        case RAMP:
            datagram[8] = uint8_t(data[0]);
            break;
        case PAN_TILT_SPEED:
            datagram[8] = uint8_t(data[0]);
            break;
        case ZOOM_SET_ABSOLUTE:
            // only for answer ??
            // all zeros
            break;
        case FOCUS_SET_ABSOLUTE:
            // only for focus
            datagram[5] = 0;                                //(ignored) zoom value
            datagram[6] = 0;
            datagram[7] = uint8_t((data[0]>>8));            //focus value
            datagram[8] = uint8_t(data[0]);                 //focus value
            break;
        case ZOOM_FOCUS_SET:
            // only for zoom
            datagram[5] = 0;
            datagram[6] = uint8_t(data[0]);                 //zoom value
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
            datagram[8] = uint8_t(data[0]);
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
            datagram[7] = uint8_t((data[0]>>8));
            datagram[8] = uint8_t(data[0]);
            break;
        case MIRROR_H_V:
            datagram[5] = 1;
            datagram[8] = uint8_t(data[0]);
            break;
        case SHUTTER_UP:
            datagram[7] = uint8_t((data[0]>>8));
            datagram[8] = uint8_t(data[0]);
            break;
        case KNEE_POINT_AUTO:
            datagram[5] = 1;
            datagram[8] = uint8_t(data[0]);
            break;
        case CALIBRATE_HEAD:
            // all zeros
            break;
        case STORE_PRESET:
            datagram[8] = uint8_t(data[0]);
            break;
        case GOTO_PRESET:
            datagram[5] = uint8_t(data[1]);
            datagram[8] = uint8_t(data[0]);
            break;
        case RED_GAIN_ADJ_UP:
            datagram[7] = uint8_t((data[0]>>8));
            datagram[8] = uint8_t(data[0]);
            break;
        case BLUE_GAIN_ADJ_UP:
            datagram[7] = uint8_t((data[0]>>8));
            datagram[8] = uint8_t(data[0]);
            break;
        case COLOR_UP:
            datagram[7] = 1;
            datagram[8] = uint8_t(data[0]);
            break;
        case MASTER_PED_UP:
            datagram[7] = uint8_t(data[0]>>8);
            datagram[8] = uint8_t(data[0]);
            break;
        case RED_PED_UP:
            datagram[7] = uint8_t((data[0]>>8));
            datagram[8] = uint8_t(data[0]);
            break;
        case GREEN_PED_UP:
            datagram[5] = uint8_t((data[0]>>8));
            datagram[6] = uint8_t(data[0]);
            break;
        case BLUE_PED_UP:
            datagram[7] = uint8_t(data[0]>>8);
            datagram[8] = uint8_t(data[0]);
            break;
        case GAMMA:
            datagram[7] = uint8_t(data[0]>>8);
            datagram[8] = uint8_t(data[0]);
            break;
        case GAMMA_TABLE:
            datagram[7] = 1;
            datagram[8] = uint8_t(data[0]);
            break;
        case HEAD_POWER:
            //datagram[5] = 1;
            datagram[8] = uint8_t(data[0]);
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
            datagram[8] = uint8_t(data[0]);

            break;
        case BNT_TEMPERATURE:
            datagram[7] = uint8_t((data[0]>>8));
            datagram[8] = uint8_t(data[0]);
            break;
        case CAMERA_RESET:
            datagram[5] = 1;
            break;
        case HEAD_STARTUP:
            // all zeros
            break;
        case DETAIL_LEVEL_ADJ:
            //datagram[7] = (uint8_t)(data[0]>>8);

            datagram[8] = uint8_t(data[0]);
            break;
        case ND_FILTER:
            datagram[8] = uint8_t(data[0]);
            break;
        case BNCE_WAIT_TIME:
            datagram[8] = uint8_t(data[0]);
            break;
        case KNEE_POINT:
            datagram[8] = uint8_t(data[0]);
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

/*Decodes BBMNet telegrams*/
void Telegrambuilder::decode(uint8_t* telegram, struct answer_s& answer){
    int temp;
    answer.type = telegram[1];
    answer.command = telegram[4];
    answer.from = telegram[3];

    /*Watchdog answer from server*/
    if(answer.from == SERVER){
        //NOP
    }
     /*Watchdog answer from camera/head*/
    else{
        switch (answer.command) {
        case TILT_PAN:

            break;
        case IRIS_OPEN:
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [0]iris value
            answer.data.push_back(temp);
            break;
        case PAN_TILT_SPEED:
            temp = telegram[8];
            answer.data.push_back(temp);
            break;
        case RAMP:
            temp = telegram[8];
            answer.data.push_back(temp);
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
            answer.data.push_back(telegram[8]);             // [0]Gain
            break;
        case AUTOIRIS_ON:
        case AUTOIRIS_OFF:
        case WHITE_BALANCE_PRST:
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [0]color temp value
            answer.data.push_back(temp);
            break;
        case MIRROR_H_V:
            temp = (telegram[8]) ;
            answer.data.push_back(temp);
            break;
        case ND_FILTER:
            temp = (telegram[8]) ;
            answer.data.push_back(temp);
            break;
        case SHUTTER_UP:
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [0]shutter value
            answer.data.push_back(temp);
            break;
        case KNEE_POINT_AUTO:
            temp = telegram[8];
            answer.data.push_back(temp);
            break;
        case KNEE_POINT:
            temp = telegram[8];
            answer.data.push_back(temp);
            break;
        case STORE_PRESET:
        case GOTO_PRESET:
            temp = telegram[8];
            answer.data.push_back(temp);
            break;
        case RED_GAIN_ADJ_UP:
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [0]w_red value
            answer.data.push_back(temp);
            break;
        case BLUE_GAIN_ADJ_UP:
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [0]w_red value
            answer.data.push_back(temp);
            break;
        case COLOR_UP:
            answer.data.push_back(telegram[8]);             // [0]saturation value
            break;
        case MASTER_PED_UP:
            temp = ( (telegram[7]<<8) | (telegram[8]) );    // [0]ped value
            answer.data.push_back(temp);
            break;
        case BARS:
        case RED_PED_UP:
            temp = ( (telegram[7]<<8) | (telegram[8]) );
            answer.data.push_back(temp);
            break;
        case GREEN_PED_UP:
        case BLUE_PED_UP:
            temp = ( (telegram[7]<<8) | (telegram[8]) );
            answer.data.push_back(temp);
            break;
        case GAMMA:
            temp = ( (telegram[7]<<8) | (telegram[8]) );
            answer.data.push_back(temp);
            break;
        case GAMMA_TABLE:
            answer.data.push_back(telegram[8]);             // [0]gamma table on/off
            break;
        case HEAD_POWER:
            temp = telegram[8];
            answer.data.push_back(temp);
            break;
        case BNT_TEMPERATURE:
        case HEAD_STARTUP:
        case DETAIL_LEVEL_ADJ:
            temp = telegram[8];
            answer.data.push_back(temp);
            break;
        case PRESET_REACHED:
            answer.data.push_back(telegram[8]);
            break;
        default:
            break;
        }
    }
}


/*Calculates telegram checksum*/
uint8_t Telegrambuilder::calc_chksum(){     // cross total bytes 4-8
    chkSum = 0;
    for(int i=3;i<9;i++)
        {
            chkSum+=datagram[i];
        }
    chkSum=(chkSum ^ 0xff);                 // XOR -> two's complement
    return chkSum;
}
