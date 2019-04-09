#ifndef TELEGRAMBUILDER_H
#define TELEGRAMBUILDER_H

#include <stdint.h>
#include <vector>

// *telegram must point to array of size 10
class Telegrambuilder
{
public:
    Telegrambuilder();
    void encode(int bbm_dev_no, int bbm_command, std::vector<int> data, uint8_t* telegram);
    void decode(uint8_t* telegram, int bbm_command, int dataOut[4]);       //todo: in welcher Form Daten...?
private:
    uint8_t datagram[10];
    uint8_t calc_chksum();
    uint8_t chkSum;
};

#endif // TELEGRAMBUILDER_H
