#ifndef TASTENFELD_H
#define TASTENFELD_H

#define UPPER_ROW 1
#define LOWER_ROW 0
#define TASTE1 19
#define TASTE2 13
#define TASTE3 6
#define TASTE4 5
#define TASTE5 22
#define TASTE6 27
#define TASTE7 12
#define TASTE8 25
#define TASTE9 24
#define TASTE10 23
#define TASTE11 18
#define TASTE12 26


#define EXPORT ">/sys/class/gpio/export"
#define UNEXPORT ">/sys/class/gpio/unexport"
#define SYSPATH "/sys/class/gpio/gpio"
#define EDGE "/edge"
#define DIRECTION "/direction"
#define VALUE "/value"
#define SPI0 "/dev/spidev0.0"
#define SPI1 "/dev/spidev0.1"
#define SPI_MODE 0
#define BITS_PER_W 8
#define SPI_SPEED 2500000

#define DEBOUNCE_T 10 //in us

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
