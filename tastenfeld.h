#ifndef TASTENFELD_H
#define TASTENFELD_H
#include <QWidget>



class Tastenfeld
{

public:
    Tastenfeld();
    ~Tastenfeld();
    int init();
    int setLed(unsigned int red, unsigned int green, unsigned int blue, int button);
    int setLed(unsigned int color, int button);
    int button[6];
    int readButton(int buttonNr);
    int initSpi();
    void setRow(int row);
    int showStored(int usedPresets, int activePreset);


private:

    int gpioExport(int gpio);
    void gpioUnExport(int gpio);
    void mapTx(unsigned int blue, unsigned int red, unsigned int green, int button);
    int spi_fd;
    unsigned char tx[36];
    int button_err;
    int spi_err;
    char *spi_bus;
    int button_bus;


};

#endif // TASTENFELD_H
