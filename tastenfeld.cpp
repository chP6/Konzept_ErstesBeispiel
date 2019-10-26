#include "tastenfeld.h"
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
#include "tastenfeld.h"




Tastenfeld::Tastenfeld()
{
}



Tastenfeld::~Tastenfeld()
{

    close(spi_fd);
}

void Tastenfeld::setRow(int row){
    /*distinguish between SPI Chip Selects*/
    switch (row) {
    case UPPER_ROW:
        spi_bus=SPI0;       //path is /dev/spidev0.1
        break;
    case LOWER_ROW:
        spi_bus=SPI1;       //path is /dev/spidev0.0
        break;
    }
}




int Tastenfeld::initSpi(){
    /*Init Spi for LEDs*/
    unsigned char mode = SPI_MODE;      //Mode 0
    unsigned char bits = BITS_PER_W;    //8 bits per word
    unsigned int speed = SPI_SPEED;     //speed 2.5 MHz

    spi_fd = open(spi_bus, O_RDWR);
       if(spi_fd<0){return -1;}

    spi_err=ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
        if(spi_err<0){return -1;}

    /*8 Bits per Word*/
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if(spi_err<0){return -1;}

    /*Speed 2.5 MHz*/
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if(spi_err<0){return -1;}

   return 0;
}



void Tastenfeld::mapTx(unsigned int blue, unsigned int red, unsigned int green, int button)
{
    /*invert direction*/
    button=7-button;
    /*The PWM Chip requires 12Bit per color per button, means 36 bit per button*/
    /*The buttons are mapped as follows*/

    /*button  |         0              |        1               | ...
    *         | bb | br | rr | gg | g  | b  | bb | rr | rg | gg | ...
      Tx byte | 0  | 1  | 2  | 4  |     5   |  6 |  7 |  8 |  9 | ...*/

    std::swap(red, green);
    /*An odd button number needs to be treated differently than an even button number*/
    static char button_map[]= {0,4,9,13,18,22,27,31};
    //static char button_map[]= {0,4,9,15,20,25,30};
        if(button % 2 == 0){				//Buttonnumber even
            /*mapping*/
            //button=button*4.5;
            *(tx+button_map[button])=((blue&0xFF0)>>4);
            *(tx+button_map[button]+1)=(((blue&0xF)<<4)|((red&0xF00)>>8))&(0xff);
            *(tx+button_map[button]+2)=red&0x0FF;
            *(tx+button_map[button]+3)=((green&0xFF0)>>4);
            *(tx+button_map[button]+4)=(((green&0x00F)<<4)|((tx[button_map[button]+4]&0x00f)))&(0xFF);
        }
        else{						//Buttonnumber odd
            /*mapping*/
            *(tx+button_map[button])=((blue&0xF00)>>8)&0xff;
            *(tx+button_map[button]+1)=(blue&0xFF);
            *(tx+button_map[button]+2)=((red&0xFF0)>>4);
            *(tx+button_map[button]+3)=(((red&0x0F)<<4)|((green&0xF00)>>8))&(0xff);
            *(tx+button_map[button]+4)=(green&0xFF);

        }
}

int Tastenfeld::setLed(unsigned int blue, unsigned int red, unsigned int green, int button)
{
    /*set led with color values*/
    memset(tx,0,sizeof(tx));                //clear tx buffer such that only one button is lit
    mapTx(blue,red,green,button);           //map it
    spi_err=write(spi_fd, &tx, sizeof(tx)); //write it on spi
        if(spi_err<0){return -1;}
    return 0;
}

int Tastenfeld::setLed(unsigned int color, int button)
{
    /*set led with hex color values*/
    memset(tx,0,sizeof(tx));    //clear tx buffer such that only one button is lit
    /*extract the color values*/
    /*PWM driver needs 12 bits per color, hex color has 8 bits per color
      therefore the lowest nibble is set to f*/
//    unsigned int red=((color&0xff0000)>>12)|0x00f;
//    unsigned int green=((color&0x00ff00)>>4)|0x00f;
//    unsigned int blue=((color&0x0000ff)<<4)|0x00f;
    unsigned int red=((color&0xff0000)>>12);
    unsigned int green=((color&0x00ff00)>>4);
    unsigned int blue=((color&0x0000ff)<<4);
    mapTx(blue,red,green,button);           //map it
    spi_err=write(spi_fd, &tx, sizeof(tx)); //write it on spi
        if(spi_err<0){return -1;}
    return 0;
}


int Tastenfeld::showStored(int usedPresets, int activePreset)
{
    /*Show free preset slots in green and occupied slots in red*/
    memset(tx,0,sizeof (tx));               //clear tx buffer
    mapTx(0xfff,0xfff,0xfff,activePreset); //show active preset in white

    /*used presets looks like follows 0b00100100 -> preset is occupied at slot 2 and 5*/
    if(!usedPresets){                      //all presets are free
        for(unsigned char i=0;i<6;i++){
            if(i!=activePreset){
            mapTx(0,0,0xfff,i);}           // show all in green except the active preset, this is white
        }
    }
        else {                              //some presets are occupied
            for(unsigned char i=0;i<6;i++){
               if(i!=activePreset){
                if(usedPresets&(1<<i)){     //bit = 1 at position i
                    mapTx(0,0xfff,0,i);     //show red
                }
                else {                      //preset is free
                    mapTx(0,0,0xfff,i);    //show green
                }}
            }
        }
    spi_err=write(spi_fd, &tx, sizeof(tx)); //write it to spi bus
            if(spi_err<0){return -1;}
            return 0;
}






