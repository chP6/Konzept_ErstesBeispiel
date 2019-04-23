#ifndef TASTENFELD_H
#define TASTENFELD_H



class Tastenfeld
{
public:
    Tastenfeld();
    ~Tastenfeld();
    int init();
    int setLed(unsigned int red, unsigned int green, unsigned int blue, unsigned char button);
    int setLed(unsigned int color,unsigned char button);
    int button[6];
    int readButton(int buttonNr);
    int initSpi();
    void setRow(int row);
    int showStored(int usedPresets);

private:

    int gpioExport(int gpio);
    void gpioUnExport(int gpio);
    void mapTx(unsigned int blue, unsigned int red, unsigned int green, unsigned char button);
    int spi_fd;
    unsigned char tx[36];
    int button_err;
    int spi_err;
    char *spi_bus;
    int button_bus;

};

#endif // TASTENFELD_H
