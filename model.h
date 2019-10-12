#ifndef MODEL_H
#define MODEL_H

//#include <QWidget>
#include <QStringList>
#include "config.h"
#include "bbmcommandtypes.h"
#include <stdio.h>
#include <QVector>
#include <mutex>
#include <QDebug>
#include "logging.h"


//#include "view.h" //nix gut, circular dependency -> forward declaration.
//class View;         //Make sure each header can be included on its own.
using namespace Config;

struct camera_s{
  int camType;
  int activePreset;
  int usedPresets;
  bool flags[NUMBER_OF_FLAGS];
  int values [ROW_ENTRIES][COLUM_ENTRIES];
  QString *textTable;
  int xptSource;
  std::vector<properties_t> remainingTelegrams;
  std::mutex mtx;

  /* control inputs */
  struct axes_t {
      int16_t relative;
      int16_t absolute;
  } axes[kAxisMax];
};



class Model : public QObject
{
    Q_OBJECT        //qmake vor erstem kompilieren laufen lassen!


signals:
    void updateView();
    void updateViewProperty(properties_t property);
    void updateViewFlag(flags_t flag);
    void setUpView();
    void updateServerConnectionStatus();
    void updateCameraConnectionStatus(bool connected);
    void updateXptConnectionStatus(bool connected);
    void updateXptEnableStatus(bool connected);
    void newSignalReceived(int property);
    void receiveAllNew();

public:
    Model();
    void addError(std::string str);
    void clearErrors();
    void setUsedPreset(int presetNr);
    void setUsedPreset(int slotNr, int presetNr);
    void clearUsedPresets();
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
    void setCamTypeOnly(int slot, int type);
    void setCamTypeWithDefValues(int slotNr, int type);
    void setCamTypeWithDefBorders(int slotNr, int type);
    int getCamtype();
    int getCamtype(int slotNr);
    void setValue(value_t type, properties_t property, int value);
    void setValue(int slotNr, value_t type, properties_t property, int value);
    int getValue(value_t type, properties_t property);
    int getValue(int slotNr, value_t type, properties_t property);
    QString getTextValue(properties_t property);
    QStringList getTextList(properties_t property);
    QString getDialState(properties_t property);
    int getMin(properties_t property);
    int getMin(int slotNr, properties_t property);
    int getMax(properties_t property);
    int getMax(int slotNr, properties_t property);
    void setCamFlag(flags_t flag, bool value);
    void setCamFlag(int slotNr, flags_t flag, bool value);
    bool getCamFlag(flags_t flag);
    bool getCamFlag(int slotNr, flags_t flag);
    int setWatchdogWaitingflag(bool waiting);
    int setCameraWaitingflag(int slotNr, bool waiting);
    bool getServerStatus();
    properties_t getRotaryField();
    void setRotaryField(properties_t field, int destination);
    int getRotaryDestination();
    int getTxCommand(properties_t value);
    properties_t getValueFromBBMCommand(int bbm_command);
    void setTextTable(int slotNr, int type);
    int toggleBlink();
    void setSppState(int slotNr, int state);
    int getSppState(int slotNr);
    bool getRequestSettingsFlag();
    void setRequestSettingsFlag(bool value);
    void setReqPendArr(int pos, bool value);
    bool getReqPendArr(int pos);
    void setCurrReqHeadNr(int headNr);
    int getCurrReqHeadNr();
    int getRequestCommand(int slotNr, properties_t property);
    void setRequestReceived(int slotNr, properties_t property);
    std::vector<properties_t> getRemainingTelegrams();
    std::vector<properties_t> getRemainingTelegrams(int slotNr);
    void clearRemainingTelegrams(int slotNr);

    /* XPT handling */
    void setXptConnected(bool flag);
    bool getXptConnected();
    void setXptSlotSource(int source);
    void setXptSlotSource(int slot, int source);
    int getXptSlotSource(int slotNr);
    void setXptDestination(int destination);
    int getXptDestination();
    void setXptSlot(int slot);
    int getXptSlot();
    void setXptIpField(int type,int field,int value);
    int getXptIpField(int field);
    QString getXptIpAdress();
    void setXptIpAdress(QString ipAdress);
    void setXptNumberOfInputs(int inputs);
    int getXptNumberOfInputs();
    void setXptNumberOfOutputs(int outputs);
    int getXptNumberOfOutputs();
    bool getXptEnabled();
    void setXptEnabled(bool flag);
    void setXptInputLables(QList<QString> inputLables);
    void setXptOutputLables(QList<QString> outputLables);
    QList<QString> getXptInputLables();
    QList<QString> getXptOutputLables();
    void setXptType(int type);
    int getXptType();
    bool getFastIris();
    void setFastIris(bool flag);

    void setControl(axis_t axis, control_t control);
    control_t getControl(axis_t axis);

    void setAxis(axis_t axis, int16_t value, bool absolute);
    bool getAxisUpdates(int slotNr, int16_t (&axes)[kAxisMax], bool absolute);

private:
    QStringList errorList;
    int answerStack;
    int count;
    properties_t rotaryField;
    int rotaryDestination;
    int x = 5000;
    int y = 5000;
    bool watchdogWaitingForAnswerFlag = false;
    int cameraWaitingForAnswerStack[NUMBER_OF_SLOTS] = {0,0,0,0,0,0};
    int sppState[NUMBER_OF_SLOTS] = {S_SPP_IDLE,S_SPP_IDLE,S_SPP_IDLE,S_SPP_IDLE,S_SPP_IDLE,S_SPP_IDLE};
    bool serverConnected = false;
    struct camera_s cameras[NUMBER_OF_SLOTS];
    int activeCameraSlot;     // 0-5
    bool blinkToggle = false;
    bool requestSettingsFlag = false;
    bool reqPendingArr[MAX_NUMBER_OF_CMDS];
    int currReqHeadNr;



    /* used for normalizing, source: arduino.cc */
    long map(long x, long in_min, long in_max, long out_min, long out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    /* control/axis mapping */
    control_t controls[kAxisMax];

    /*XPT handling */
    bool xptConnect=false;
    bool xptEnabled=false;
    int xptDestination=0;
    int xptSlot;
    int xptNumberOfInputs=8;
    int xptNumberOfOutputs=8;
    QString xptIpAddress = "192.168.103.14";
    int xptFields[4];
    QList<QString> xptInputLabels;
    QList<QString> xptOutputLabels;
    int xptType;
    bool fastIris=false;


    // camera type 2 init values
    //todo: not all entries have 5 values!
    int c2Values[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,Normal},     //headnr init_value, min_value, max_value
                       {0x05,0x05,0x15,Text,0,REQUESTABLE}, //Iris
                       {64,0,128,CenterVal,0,REQUESTABLE},  //Pedestal
                       {0,0,8000,Normal,0,REQUESTABLE},  //Focus
                       {127,0,255,CenterVal,0,REQUESTABLE}, //w_Red
                       {127,0,255,CenterVal,0,REQUESTABLE}, //w_Blue
                       {100,0,200,CenterVal,0,REQUESTABLE}, //b_Red
                       {100,0,200,CenterVal,0,REQUESTABLE}, //b_Blue
                       {1,1,13,Text,0,REQUESTABLE},    //Gain
                       {255,1,512,CenterVal,0,REQUESTABLE},   //Gamma
                       {0,0,2,Text,1,REQUESTABLE},      //Gamma-Table
                       {128,1,254,CenterVal,0,REQUESTABLE},  //Detail
                       {127,1,255,CenterVal,0,REQUESTABLE},  //Color
                       {0,0,5,Text,2,REQUESTABLE},      //Color Temp
                       {-1,-1,-1,Nan},    //Knee
                       {-1,-1,-1,Nan},    //Knee Point
                       {0,0,3,Text,3,REQUESTABLE},       //ND Filter
                       {18,18,32,Text,4,REQUESTABLE},    //Shutter
                       {5,1,10,Normal},      //PT Speed
                       {1,1,10,Normal},      //Trans Speed
                       {1,1,10,Normal},      //Ramp
                       {0,0,5,Offset},       //SPP1
                       {1,0,5,Offset},       //SPP2
                       {0,0,30,Normal},      //SPP Wait Time
                       {2,0,7,Normal},    //Bounce Zoom Speed
                       {0,0,2,Text,5},       //Head Power
                       {0,0,3,Text,6}              //Mirror
                      };

    // camera type 1 init values
    int c1Values[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,Normal,0,0},     //headnr init_value, min_value, max_value
                       {0x05,0x05,0x11,Text,0,REQUESTABLE}, //Iris
                       {64,0,128,CenterVal,0,REQUESTABLE},  //Pedestal
                       {0,0,8000,Normal,0,REQUESTABLE},  //Focus
                       {127,0,255,CenterVal,0,REQUESTABLE}, //w_Red
                       {127,0,255,CenterVal,0,REQUESTABLE}, //w_Blue
                       {100,0,200,CenterVal,0,REQUESTABLE}, //b_Red
                       {100,0,200,CenterVal,0,REQUESTABLE}, //b_Blue
                       {1,1,15,Text,0,REQUESTABLE},    //Gain
                       {-1,-1,-1,Nan},   //Gamma
                       {0,0,1,Text,1,REQUESTABLE},      //Gamma-Table
                       {128,0,254,CenterVal,0,REQUESTABLE},  //Detail
                       {4,0,14,Text,0,REQUESTABLE},  //Color
                       {0,0,5,Text,2,REQUESTABLE},      //Color Temp
                       {-1,-1,-1,Nan},    //Knee
                       {-1,-1,-1,Nan},    //Knee Point
                       {-1,-1,-1,Nan},       //ND Filter
                       {6,6,20,Text,3,REQUESTABLE},    //Shutter
                       {1,1,10,Normal},      //PT Speed
                       {1,1,10,Normal},      //Trans Speed
                       {1,1,10,Normal},      //Ramp
                       {0,0,5,Offset},       //SPP1
                       {1,0,5,Offset},       //SPP2
                       {0,0,30,Normal},      //SPP Wait Time
                       {2,0,7,Normal},    //Bounce Zoom Speed
                       {0,0,2,Text,4},        //Head Power
                       {0,0,3,Text,5}              //Mirror
                      };

    // camera type 3&4 init values
    int rValues[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,Normal,0,0},     //headnr init_value, min_value, max_value
                       {2000,0,4000,Normal,0,0}, //Iris
                       {127,0,255,CenterVal,0,0},  //Pedestal
                       {1000,1000,3250,Normal,0,REQUESTABLE},  //Focus
                       {127,0,255,CenterVal,0,REQUESTABLE}, //w_Red
                       {127,0,255,CenterVal,0,REQUESTABLE}, //w_Blue
                       {127,0,255,CenterVal,0,REQUESTABLE}, //b_Red
                       {127,0,255,CenterVal,0,REQUESTABLE}, //b_Blue
                       {0,0,9,Text,0,REQUESTABLE},    //Gain
                       {127,0,255,CenterVal,0,REQUESTABLE},   //Gamma
                       {0,0,2,Text,1,REQUESTABLE},      //Gamma-Table
                       {127,0,255,CenterVal,0,REQUESTABLE},  //Detail
                       {64,0,127,CenterVal,0,REQUESTABLE},  //Color
                       {0,0,4,Text,2,REQUESTABLE},    //Color Temp
                       {0,0,2,Text,3,REQUESTABLE},    //Knee
                       {127,0,254,CenterVal,0,REQUESTABLE},    //Knee Point
                       {-1,-1,-1,Nan},       //ND Filter
                       {0,0,6,Text,5,REQUESTABLE},    //Shutter
                       {5,1,10,Normal},      //PT Speed
                       {1,1,10,Normal},      //Trans Speed
                       {1,1,10,Normal},      //Ramp
                       {0,0,5,Offset},       //SPP1
                       {1,0,5,Offset},       //SPP2
                       {0,0,30,Normal},      //SPP Wait Time
                       {12,1,127,Normal},    //Bounce Zoom Speed
                       {0,0,2,Text,6},        //Head Power
                       {0,0,3,Text,7}               //Mirror
                      };

    // camera type 3&4 init values
    /*Problem with color temperature*/
    int ursaValues[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,Normal,0,0},     //headnr init_value, min_value, max_value
                       {2000,0,4095,Normal,0,0}, //Iris
                       {127,0,400,CenterVal,0,0},  //Pedestal
                       {-1,-1,-1,Nan},  //Focus
                       {400,1,800,CenterVal,0,REQUESTABLE}, //w_Red
                       {400,1,800,CenterVal,0,REQUESTABLE}, //w_Blue
                       {100,0,200,CenterVal,0,REQUESTABLE}, //b_Red
                       {100,0,200,CenterVal,0,REQUESTABLE}, //b_Blue
                       {1,1,4,Normal,0,REQUESTABLE},    //Gain
                       {0,0,2,Normal,0,REQUESTABLE},   //Gamma
                       {-1,-1,-1,Nan},      //Gamma-Table
                       {0,0,3,Normal,0,REQUESTABLE},  //Detail
                       {100,0,200,CenterVal,0,REQUESTABLE},  //Color
                       {0,0,0,Normal,0,REQUESTABLE},    //Color Temp
                       {-1,-1,-1,Nan},    //Knee
                       {-1,-1,-1,Nan},    //Knee Point
                       {-1,-1,-1,Nan},       //ND Filter
                       {100,1,200,Normal,0,REQUESTABLE},    //Shutter
                       {5,1,10,Normal},      //PT Speed
                       {1,1,10,Normal},      //Trans Speed
                       {1,1,10,Normal},      //Ramp
                       {0,0,5,Offset},       //SPP1
                       {1,0,5,Offset},       //SPP2
                       {0,0,30,Normal},      //SPP Wait Time
                       {12,1,127,Normal},    //Bounce Zoom Speed
                       {0,0,2,Text,6},        //Head Power
                       {0,0,3,Text,7}               //Mirror
                      };
    int commandtype[ROW_ENTRIES]{-1,IRIS_OPEN,MASTER_PED_UP,FOCUS_SET_ABSOLUTE,
    RED_GAIN_ADJ_UP,BLUE_GAIN_ADJ_UP,RED_PED_UP,BLUE_PED_UP,CAMERA_GAIN_UP,GAMMA,
    GAMMA_TABLE,DETAIL_LEVEL_ADJ,COLOR_UP,WHITE_BALANCE_PRST,KNEE_POINT_AUTO,KNEE_POINT,ND_FILTER,
    SHUTTER_UP,PAN_TILT_SPEED,-1,RAMP,-1,-1,BNCE_WAIT_TIME,-1,HEAD_POWER,MIRROR_H_V};


    QString c1TextTable[ROW_ENTRIES][256]={
        {}, // head nr
        {"f14","f11","f9.6","f8","f6.8","f5.6","f4.8","f4","f3.4","f2.8","f2.4","f2","f1.6"}, // iris
        {}, // pedestal
        {}, // focus
        {}, // w red
        {}, // w blue
        {}, // b red
        {}, // b blue
        {"0dB","3dB","6dB","9dB","12dB","15dB","18dB","21dB","24dB","27dB","30dB","33dB","36dB","39dB","42dB"}, // gain
        {}, // gamma
        {"High","Low"}, // gamma table
        {}, // detail
        {"60%", "70%", "80%", "90%", "100%", "110%", "120%", "130%", "140%", "150%", "160%", "170%", "180%", "190%", "200%", }, // color
        {"Auto","Indoor","Outdoor","One Push WB","ATW","Manual"}, // wb preset
        {}, // knee
        {}, // knee point
        {}, // ND filter
        {"1/50","1/75","1/100","1/120","1/150","1/215","1/300","1/425","1/600","1/1000","1/1250","1/1750","1/2500","1/3500","1/6000"}, // shutter
        {}, // PT speed
        {}, // trans speed
        {}, // ramp
        {}, // SPP 1
        {}, // SPP 2
        {}, // SPP wait time
        {}, // bounce zoom speed
        {"Low","Mid","High"}, // head power
        {"Normal","H-Inverted","V-Inverted","HV-Inverted"} // mirror
    };
    QString c2TextTable[ROW_ENTRIES][256]={
        {}, // head nr
        {"f11","f10","f9.6","f8.7","f8.0","f7.3","f6.8","f6.2","f5.6","f5.2","f4.8","f4.4","f4.0","f3.7","f3.4","f3.1","f2.8"}, // iris
        {}, // pedestal
        {}, // focus
        {}, // w red
        {}, // w blue
        {}, // b red
        {}, // b blue
        {"0dB","3dB","6dB","9dB","12dB","15dB","18dB","21dB","24dB","27dB","30dB","33dB","36dB","39dB","42dB","45dB","48dB"}, // gain
        {}, // gamma
        {"Standard","Straight","Pattern"}, // gamma table
        {}, // detail
        {}, // color
        {"Auto","Indoor","Outdoor","One Push WB","ATW","Manual"}, // wb preset
        {}, // knee
        {}, // knee point
        {"Clear","1/4","1/16","1/64"}, // ND filter
        {"1/50","1/100","1/120","1/150","1/215","1/300","1/425","1/600","1/1000","1/1250","1/1750","1/2500","1/3500","1/6000"}, // shutter
        {}, // PT speed
        {}, // trans speed
        {}, // ramp
        {}, // SPP 1
        {}, // SPP 2
        {}, // SPP wait time
        {}, // bounce zoom speed
        {"Low","Mid","High"}, // head power
        {"Normal","H-Inverted","V-Inverted","HV-Inverted"} // mirror
    };
    QString rTextTable[ROW_ENTRIES][256]={
        {}, // head nr
        {}, // iris
        {}, // pedestal
        {}, // focus
        {}, // w red
        {}, // w blue
        {}, // b red
        {}, // b blue
        {"-5dB","-4dB","-3dB","-2dB","-1dB","0dB","-1dB","2dB","3dB","4dB","5dB"}, // gain
        {}, // gamma
        {"Low","Mid","High"}, // gamma table
        {}, // detail
        {}, // color
        {"2800K","3200K","4000K","4600K","5600K"}, // wb preset
        {"Off","On","Auto"}, // knee
        {}, // knee point
        {"Clear","1/4","1/16","1/64"}, // ND filter
        {"1/50","1/75","1/100","1/120","1/150","1/215","1/300"}, // shutter
        {}, // PT speed
        {}, // trans speed
        {}, // ramp
        {}, // SPP 1
        {}, // SPP 2
        {}, // SPP wait time
        {}, // bounce zoom speed
        {"Low","Mid","High"}, // head power
        {"Normal","H-Inverted","V-Inverted","HV-Inverted"} // mirror
    };
    QString ursaTextTable[ROW_ENTRIES][256]={
        {}, // head nr
        {}, // iris
        {}, // pedestal
        {}, // focus
        {}, // w red
        {}, // w blue
        {}, // b red
        {}, // b blue
        {"-5dB","-4dB","-3dB","-2dB","-1dB","0dB","-1dB","2dB","3dB","4dB","5dB"}, // gain
        {}, // gamma
        {"Low","Mid","High"}, // gamma table
        {}, // detail
        {}, // color
        {"2800K","3200K","4000K","4600K","5600K"}, // wb preset
        {"Off","On","Auto"}, // knee
        {}, // knee point
        {"Clear","1/4","1/16","1/64"}, // ND filter
        {"1/50","1/75","1/100","1/120","1/150","1/215","1/300"}, // shutter
        {}, // PT speed
        {}, // trans speed
        {}, // ramp
        {}, // SPP 1
        {}, // SPP 2
        {}, // SPP wait time
        {}, // bounce zoom speed
        {"Low","Mid","High"}, // head power
        {"Normal","H-Inverted","V-Inverted","HV-Inverted"} // mirror
    };

};

#endif // MODEL_H
