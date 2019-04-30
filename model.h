#ifndef MODEL_H
#define MODEL_H

#include <QWidget>
#include <QStringList>
#include "config.h"
//#include "view.h" //nix gut, circular dependency -> forward declaration.
//class View;         //Make sure each header can be included on its own.

struct Camera_s{
  unsigned char camType;
  unsigned char activePreset;
  unsigned char usedPresets;
  bool flags[3];
  int values [ROW_ENTRIES][COLUM_ENTRIES];

};

class Model : public QObject
{
    Q_OBJECT        //qmake vor erstem kompilieren laufen lassen!

signals:
    void updateView();
    void updateServerConnectionStatus(bool connected);

public:
    Model();
    void setData(int data);
    int getData();
    void setAxis(int x, int y);
    void getAxis(int &x, int &y);
    void addError(std::string str);
    void clearErrors();
    void setUsedPreset(int presetNr);
    int getUsedPreset();
    void setActivePreset(unsigned char actPreset);
    unsigned char getActivePreset();
    void setActiveCamera(unsigned char camNr);
    unsigned char getActiveCamera();
    QStringList* getErrorList();
    void setCamType(unsigned char type);
    unsigned char getCamtype();
    void setValue(int property, int value);
    int getValue(int property);
    int getMin(int property);
    int getMax(int property);
    void setCamFlag(int flag, bool value);
    bool getCamFlag(int flag);
    int setWatchdogWaitingflag(bool waiting);

private:
    QStringList errorList;
    int answerStack;
    int count;
    int x = 5000;
    int y = 5000;
    bool watchdogWaitingForAnswerFlag = false;
    bool serverConnected = false;
    struct Camera_s cameras[6];
    unsigned char activeCamera;
    int c2Values[ROW_ENTRIES][COLUM_ENTRIES]={{1,0,49,NORMAL},     //headnr init_value, min_value, max_value
                       {127,0,255,NORMAL}, //Iris
                       {64,0,128,NORMAL},  //Pedestal
                       {0,0,8000,NORMAL},  //Focus
                       {127,0,255,CENTER}, //w_Red
                       {127,0,255,CENTER}, //w_Blue
                       {127,0,255,CENTER}, //w_Red
                       {100,0,200,CENTER}, //b_Blue
                       {100,0,200,CENTER}, //b_Red
                       {0,0,14,TEXT},    //Gain
                       {255,0,512,CENTER},   //Gamma
                       {0,0,2,TEXT},      //Gamma-Table
                       {128,1,254,CENTER},  //Detail
                       {127,1,255,CENTER},  //Color
                       {0,0,5,TEXT},      //Color Temp
                       {-1,-1,-1,NAN},    //Knee
                       {-1,-1,-1,NAN},    //Knee Point
                       {0,0,3,TEXT},       //ND Filter
                       {17,17,33,TEXT},    //Shutter
                       {1,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {1,1,6,NORMAL},       //SPP1
                       {2,1,6,NORMAL},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT}        //Head Power
                      };
    int c1Values[ROW_ENTRIES][COLUM_ENTRIES]={{1,0,49,NORMAL},     //headnr init_value, min_value, max_value
                       {127,0,255,NORMAL}, //Iris
                       {64,0,128,NORMAL},  //Pedestal
                       {0,0,8000,NORMAL},  //Focus
                       {127,0,255,CENTER}, //w_Red
                       {127,0,255,CENTER}, //w_Blue
                       {127,0,255,CENTER}, //w_Red
                       {100,0,200,CENTER}, //b_Blue
                       {100,0,200,CENTER}, //b_Red
                       {0,0,14,TEXT},    //Gain
                       {-1,-1,-1,NAN},   //Gamma
                       {0,0,1,TEXT},      //Gamma-Table
                       {128,0,254,CENTER},  //Detail
                       {127,1,255,CENTER},  //Color
                       {0,0,5,TEXT},      //Color Temp
                       {-1,-1,-1,NAN},    //Knee
                       {-1,-1,-1,NAN},    //Knee Point
                       {-1,-1,-1,NAN},       //ND Filter
                       {0,0,7,TEXT},    //Shutter
                       {1,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {1,1,6,NORMAL},       //SPP1
                       {2,1,6,NORMAL},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT}        //Head Power
                      };
    int rValues[ROW_ENTRIES][COLUM_ENTRIES]={{1,0,49,NORMAL},     //headnr init_value, min_value, max_value
                       {2000,0,4000,NORMAL}, //Iris
                       {127,0,255,NORMAL},  //Pedestal
                       {1000,1000,3250,NORMAL},  //Focus
                       {127,0,255,CENTER}, //w_Red
                       {127,0,255,CENTER}, //w_Blue
                       {127,0,255,CENTER}, //w_Red
                       {127,0,255,CENTER}, //b_Blue
                       {127,0,255,CENTER}, //b_Red
                       {0,0,9,TEXT},    //Gain
                       {127,0,255,CENTER},   //Gamma
                       {0,0,2,TEXT},      //Gamma-Table
                       {127,0,255,CENTER},  //Detail
                       {64,0,127,CENTER},  //Color
                       {0,0,4,TEXT},      //Color Temp
                       {0,0,2,TEXT},    //Knee
                       {127,0,254,CENTER},    //Knee Point
                       {-1,-1,-1,NAN},       //ND Filter
                       {0,0,7,TEXT},    //Shutter
                       {1,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {1,1,6,NORMAL},       //SPP1
                       {2,1,6,NORMAL},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT}        //Head Power
                      };

};

#endif // MODEL_H
