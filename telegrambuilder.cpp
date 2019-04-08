#include "telegrambuilder.h"
#include "bbmcommandtypes.h"

Telegrambuilder::Telegrambuilder()
{

}


void Telegrambuilder::encode(int bbm_dev_no, int bbm_command, std::vector<int> data, uint8_t* telegram){

    datagram[0]=SYNCHRO;
    datagram[1]=TYPED;
    datagram[2]=SYNCHRO;
    datagram[3]=bbm_dev_no;
    datagram[4]=bbm_command;
    datagram[5]= 0;             //default, weil meist 0
    datagram[6]= 0;
    datagram[7]= 0;
    datagram[8]= 0;


    switch (bbm_command) {
    case  WATCHDOG:
        datagram[5]=1;
        datagram[6]=HEAD;
        datagram[7]=0;
        datagram[8]=0;
        break;

    default:
        break;
    }
    datagram[9] = calc_chksum();
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
