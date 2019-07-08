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
    for(unsigned int i=0;i<(sizeof(button)/sizeof(button[0]));i++){

        close(button[i]);
    }

    switch(button_bus){

        case UPPER_ROW:
            gpioUnExport(TASTE1);
            gpioUnExport(TASTE2);
            gpioUnExport(TASTE3);
            gpioUnExport(TASTE4);
            gpioUnExport(TASTE5);
            gpioUnExport(TASTE6);
        break;

        case LOWER_ROW:
            gpioUnExport(TASTE7);
            gpioUnExport(TASTE8);
            gpioUnExport(TASTE9);
            gpioUnExport(TASTE10);
            gpioUnExport(TASTE11);
            gpioUnExport(TASTE12);

        break;

    }


}

void Tastenfeld::setRow(int row){

    switch (row) {
    case UPPER_ROW:
        spi_bus=SPI1;
        button_bus=row;
        break;
    case LOWER_ROW:
        spi_bus=SPI0;
        button_bus=row;
        break;
    }
}



int Tastenfeld::init(){



    switch(button_bus){

        case UPPER_ROW:
            button_err=button[0]=gpioExport(TASTE1);
                if(button_err<0){return -1;}
            button_err=button[1]=gpioExport(TASTE2);
                if(button_err<0){return -1;}
            button_err=button[2]=gpioExport(TASTE3);
                if(button_err<0){return -1;}
            button_err=button[3]=gpioExport(TASTE4);
                if(button_err<0){return -1;}
            button_err=button[4]=gpioExport(TASTE5);
                if(button_err<0){return -1;}
            button_err=button[5]=gpioExport(TASTE6);
                if(button_err<0){return -1;}
        break;

        case LOWER_ROW:
            button_err=button[0]=gpioExport(TASTE7);
                if(button_err<0){return -1;}
            button_err=button[1]=gpioExport(TASTE8);
                if(button_err<0){return -1;}
            button_err=button[2]=gpioExport(TASTE9);
                if(button_err<0){return -1;}
            button_err=button[3]=gpioExport(TASTE10);
                if(button_err<0){return -1;}
            button_err=button[4]=gpioExport(TASTE11);
                if(button_err<0){return -1;}
            button_err=button[5]=gpioExport(TASTE12);
                if(button_err<0){return -1;}

        break;

    }
    /*Mode 0*/

    return 0;
}

int Tastenfeld::initSpi(){
    unsigned char mode = SPI_MODE;
    unsigned char bits = BITS_PER_W;
    unsigned int speed = SPI_SPEED;

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

int Tastenfeld::gpioExport(int gpio){

    char buffer[60];
    sprintf(buffer,"echo '%d' %s",gpio,EXPORT);
    system(buffer);
    memset(buffer,0,sizeof(buffer));

    sprintf(buffer,"echo 'in' >%s%d%s",SYSPATH,gpio,DIRECTION);
    system(buffer);
    memset(buffer,0,sizeof(buffer));

    sprintf(buffer,"echo 'falling' >%s%d%s",SYSPATH,gpio,EDGE);
    system(buffer);
    memset(buffer,0,sizeof(buffer));

    sprintf(buffer,"%s%d%s",SYSPATH,gpio,VALUE);
    int fd = open(buffer,O_RDONLY);
    memset(buffer,0,sizeof(buffer));
    return fd;
}

void Tastenfeld::gpioUnExport(int gpio){

    char buffer[60];
    sprintf(buffer,"echo '%d' %s",gpio,UNEXPORT);
    system(buffer);
    memset(buffer,0,sizeof(buffer));

}

void Tastenfeld::mapTx(unsigned int blue, unsigned int red, unsigned int green, int button)
{
    button=5-button;
    static char button_map[]= {0,4,9,13,18,22,27};
    //static char button_map[]= {0,4,9,15,20,25,30};
        if(button % 2 == 0){				//Buttonnumber gerade
            /*mapping*/
            //button=button*4.5;
            *(tx+button_map[button])=((blue&0xFF0)>>4);
            *(tx+button_map[button]+1)=(((blue&0xF)<<4)|((red&0xF00)>>8))&(0xff);
            *(tx+button_map[button]+2)=red&0x0FF;
            *(tx+button_map[button]+3)=((green&0xFF0)>>4);
            *(tx+button_map[button]+4)=(((green&0x00F)<<4)|((tx[button_map[button]+4]&0x00f)))&(0xFF);
        }
        else{						//Buttonnumber ungerade
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
    memset(tx,0,sizeof(tx));
    mapTx(blue,red,green,button);
    spi_err=write(spi_fd, &tx, sizeof(tx));
        if(spi_err<0){return -1;}
    return 0;
}

int Tastenfeld::setLed(unsigned int color, int button)
{
    memset(tx,0,sizeof(tx));
    unsigned int red=((color&0xff0000)>>12)|0x00f;
    unsigned int green=((color&0x00ff00)>>4)|0x00f;
    unsigned int blue=((color&0x0000ff)<<4)|0x00f;
    mapTx(blue,red,green,button);
    spi_err=write(spi_fd, &tx, sizeof(tx));
        if(spi_err<0){return -1;}
    return 0;
}


int Tastenfeld::showStored(int usedPresets, int activePreset)
{
    memset(tx,0,sizeof (tx));
    mapTx(0xfff,0xfff,0xfff,activePreset);
    if(!usedPresets){
        for(unsigned char i=0;i<6;i++){
            if(i!=activePreset){
            mapTx(0,0,0xfff,i);}
        }
    }
        else {
            for(unsigned char i=0;i<6;i++){
               if(i!=activePreset){
                if(usedPresets&(1<<i)){
                    mapTx(0,0xfff,0,i);
                }
                else {

                    mapTx(0,0,0xfff,i);
                }}
            }
        }
    spi_err=write(spi_fd, &tx, sizeof(tx));
            if(spi_err<0){return -1;}
            return 0;
}


int Tastenfeld::readButton(int buttonNr){
    char buffer[2];
    button_err=read(button[buttonNr],buffer,1);
    if(button_err<0){
        return button_err;
    }
    lseek(button[buttonNr],0,SEEK_SET);
    button_err=atoi(buffer);
    return button_err;
}




