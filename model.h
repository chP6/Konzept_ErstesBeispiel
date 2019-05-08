#ifndef MODEL_H
#define MODEL_H

#include <QWidget>
#include <QStringList>
#include "config.h"
#include "bbmcommandtypes.h"
//#include "view.h" //nix gut, circular dependency -> forward declaration.
//class View;         //Make sure each header can be included on its own.

struct camera_s{
  int camType;
  int activePreset;
  int usedPresets;
  bool flags[NUMBER_OF_FLAGS];
  int values [ROW_ENTRIES][COLUM_ENTRIES];
  QString *textTable;
};



class Model : public QObject
{
    Q_OBJECT        //qmake vor erstem kompilieren laufen lassen!

signals:
    void updateView();
    void setUpView();
    void updateServerConnectionStatus(bool connected);
    void updateCameraConnectionStatus(bool connected);

public:
    Model();
    void setData(int data);
    int getData();
    void setAxis(int x, int y);
    void getAxis(int &x, int &y);
    void addError(std::string str);
    void clearErrors();
    void setUsedPreset(int presetNr);
    void setUsedPreset(int slotNr, int presetNr);
    int getUsedPreset();
    int getUsedPreset(int slotNr);
    void setActivePreset(int actPreset);
    void setActivePreset(int slotNr, int actPreset);
    int getActivePreset();
    int getActivePreset(int slotNr);
    int setActiveCameraSlot(int slotNr);
    int getActiveCameraSlot();
    QStringList* getErrorList();
    void setCamType(int slotNr, int type);
    void setCamTypeWithDefValues(int slotNr, int type);
    unsigned char getCamtype();
    unsigned char getCamtype(int slotNr);
    void setValue(int type, int property, int value);
    void setValue(int slotNr, int type, int property, int value);
    int getValue(int type, int property);
    int getValue(int slotNr, int type, int property);
    QString getTextValue(int property);
    int getMin(int property);
    int getMax(int property);
    void setCamFlag(int flag, bool value);
    void setCamFlag(int slotNr, int flag, bool value);
    bool getCamFlag(int flag);
    bool getCamFlag(int slotNr, int flag);
    int setWatchdogWaitingflag(bool waiting);
    int setCameraWaitingflag(int slotNr, bool waiting);
    int getRotaryField();
    void setRotaryField(int field);
    int getTxCommand(int value);
    int getValueFromBBMCommand(int bbm_command);
    void setTextTable(int slotNr, int type);
    int toggleBlink();


private:
    QStringList errorList;
    int answerStack;
    int count;
    int rotaryField;
    int x = 5000;
    int y = 5000;
    bool watchdogWaitingForAnswerFlag = false;
    int cameraWaitingForAnswerStack[NUMBER_OF_SLOTS] = {0,0,0,0,0,0};
    bool serverConnected = false;
    struct camera_s cameras[NUMBER_OF_SLOTS];
    int activeCameraSlot;     // 0-5
    bool blinkToggle = false;
    // camera type 2 init values
    int c2Values[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,0,49,NORMAL},     //headnr init_value, min_value, max_value
                       {127,0,255,NORMAL}, //Iris
                       {64,0,128,CENTER},  //Pedestal
                       {0,0,8000,NORMAL},  //Focus
                       {127,0,255,CENTER}, //w_Red
                       {127,0,255,CENTER}, //w_Blue
                       {100,0,200,CENTER}, //b_Red
                       {100,0,200,CENTER}, //b_Blue
                       {0,0,14,TEXT,0},    //Gain
                       {255,0,512,CENTER},   //Gamma
                       {0,0,2,TEXT,1},      //Gamma-Table
                       {128,1,254,CENTER},  //Detail
                       {127,1,255,CENTER},  //Color
                       {0,0,5,TEXT,2},      //Color Temp
                       {-1,-1,-1,NAN},    //Knee
                       {-1,-1,-1,NAN},    //Knee Point
                       {0,0,3,TEXT,3},       //ND Filter
                       {17,17,32,TEXT,4},    //Shutter
                       {5,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {1,1,6,NORMAL},       //SPP1
                       {2,1,6,NORMAL},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT,5}        //Head Power
                      };

    // camera type 1 init values
    int c1Values[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,NORMAL},     //headnr init_value, min_value, max_value
                       {127,0,255,NORMAL}, //Iris
                       {64,0,127,CENTER},  //Pedestal
                       {0,0,8000,NORMAL},  //Focus
                       {127,0,255,CENTER}, //w_Red
                       {127,0,255,CENTER}, //w_Blue
                       {100,0,200,CENTER}, //b_Red
                       {100,0,200,CENTER}, //b_Blue
                       {0,0,14,TEXT,0},    //Gain
                       {-1,-1,-1,NAN},   //Gamma
                       {0,0,1,TEXT,1},      //Gamma-Table
                       {128,0,254,CENTER},  //Detail
                       {127,1,255,CENTER},  //Color
                       {0,0,5,TEXT,2},      //Color Temp
                       {-1,-1,-1,NAN},    //Knee
                       {-1,-1,-1,NAN},    //Knee Point
                       {-1,-1,-1,NAN},       //ND Filter
                       {6,6,20,TEXT,3},    //Shutter
                       {1,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {1,1,6,NORMAL},       //SPP1
                       {2,1,6,NORMAL},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT,0}        //Head Power
                      };

    // camera type 3&4 init values
    int rValues[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,NORMAL},     //headnr init_value, min_value, max_value
                       {2000,0,4000,NORMAL}, //Iris
                       {127,0,255,CENTER},  //Pedestal
                       {1000,1000,3250,NORMAL},  //Focus
                       {127,0,255,CENTER}, //w_Red
                       {127,0,255,CENTER}, //w_Blue
                       {127,0,255,CENTER}, //b_Red
                       {127,0,255,CENTER}, //b_Blue
                       {0,0,9,TEXT,0},    //Gain
                       {127,0,255,CENTER},   //Gamma
                       {0,0,2,TEXT,1},      //Gamma-Table
                       {127,0,255,CENTER},  //Detail
                       {64,0,127,CENTER},  //Color
                       {0,0,4,TEXT,2},      //Color Temp
                       {0,0,2,TEXT,3},    //Knee
                       {127,0,254,CENTER},    //Knee Point
                       {-1,-1,-1,NAN},       //ND Filter
                       {0,0,7,TEXT,5},    //Shutter
                       {5,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {1,1,6,NORMAL},       //SPP1
                       {2,1,6,NORMAL},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT,6}        //Head Power
                      };
    int commandtype[ROW_ENTRIES]{-1,IRIS_OPEN,MASTER_PED_UP,FOCUS_SET_ABSOLUTE,
    RED_GAIN_ADJ_UP,BLUE_GAIN_ADJ_UP,RED_PED_UP,BLUE_PED_UP,CAMERA_GAIN_UP,GAMMA,
    GAMMA_TABLE,DETAIL_LEVEL_ADJ,COLOR_UP,WHITE_BALANCE_PRST,KNEE_POINT_AUTO,-1,-1,
    SHUTTER_UP,PAN_TILT_SPEED,-1,RAMP,-1,-1,-1,-1,HEAD_POWER};

    QString c1TextTable[5][15]={{"-10.5dB","-9dB","-7.5dB","-6dB","-4.5dB","-3dB","-1.5dB","0dB","1.5dB","3dB","4.5dB","6dB","7.5dB","9dB","10.5dB"},
                               {"High","Low"},
                                {"2800K","3200K","4000K","4600K","5600K","6300K"},
                                {"1/50","1/75","1/100","1/120","1/150","1/215","1/300","1/425","1/600","1/1000","1/1250","1/1750","1/2500","1/3500","1/6000"},
                                {"Low","Mid","High"}
                               };
    QString c2TextTable[6][15]={{"-10.5dB","-9dB","-7.5dB","-6dB","-4.5dB","-3dB","-1.5dB","0dB","1.5dB","3dB","4.5dB","6dB","7.5dB","9dB","10.5dB"},
                               {"Low","Mid","High"},
                                {"2800K","3200K","4000K","4600K","5600K","6300K"},
                                {"Clear","1/4","1/16","1/64"},
                                {"1/50","1/75","1/100","1/120","1/150","1/215","1/300","1/425","1/600","1/1000","1/1250","1/1750","1/2500","1/3500","1/6000"},
                                {"Low","Mid","High"}
                               };
    QString rTextTable[7][15]={{"-5dB","-4dB","-3dB","-2dB","-1dB","0dB","-1dB","2dB","3dB","4dB","5dB"},
                               {"Low","Mid","High"},
                                {"2800K","3200K","4000K","4600K","5600K"},
                                {"Off","On","Auto"},
                                {"Clear","1/4","1/16","1/64"},
                                {"1/50","1/75","1/100","1/120","1/150","1/215","1/300","1/425"},
                                {"Low","Mid","High"}
                               };
};

#endif // MODEL_H
