#ifndef TASTENFELD_H
#define TASTENFELD_H
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "config.h"

class Tastenfeld
{

public:
    Tastenfeld();
    ~Tastenfeld();
    int setLed(unsigned int red, unsigned int green, unsigned int blue, int button);
    int setLed(unsigned int color, int button);
    int initSpi();
    void setRow(int row);
    int showStored(int usedPresets, int activePreset);

private:
    void mapTx(unsigned int blue, unsigned int red, unsigned int green, int button);
    int spi_fd;
    unsigned char tx[36];
    int button_err;
    int spi_err;
    char *spi_bus;
};

#endif // TASTENFELD_H
