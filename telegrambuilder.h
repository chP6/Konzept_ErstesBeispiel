#ifndef TELEGRAMBUILDER_H
#define TELEGRAMBUILDER_H

#include <stdint.h>
#include <vector>

struct answer_s{
  int type;
  int command;
  int from;
  std::vector<int> data;
  bool sta;
};


// *telegram must point to array of size 10
class Telegrambuilder
{
public:
    Telegrambuilder();
    void encode(bool request, int bbm_dev_no, int bbm_command, std::vector<int> data, uint8_t* telegram);
    void decode(uint8_t* telegram, struct answer_s& answer);       //todo: in welcher Form Daten...?
private:
    uint8_t datagram[10];
    uint8_t calc_chksum();
    uint8_t chkSum;
};

#endif // TELEGRAMBUILDER_H
