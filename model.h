#ifndef MODEL_H
#define MODEL_H

#include <QWidget>
#include <QStringList>
#include "config.h"
#include "bbmcommandtypes.h"
#include <stdio.h>
#include <QVector>
#include <mutex>
//#include "view.h" //nix gut, circular dependency -> forward declaration.
//class View;         //Make sure each header can be included on its own.

struct camera_s{
  int camType;
  int activePreset;
  int usedPresets;
  bool flags[NUMBER_OF_FLAGS];
  int values [ROW_ENTRIES][COLUM_ENTRIES];
  QString *textTable;
  int xptSource;
  std::vector<int> remainingTelegrams;
  std::mutex mtx;
};



class Model : public QObject
{
    Q_OBJECT        //qmake vor erstem kompilieren laufen lassen!

signals:
    void updateView();
    void setUpView();
    void updateServerConnectionStatus(bool connected);
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
    void setRotaryField(int field, int destination);
    int getRotaryDestination();
    int getTxCommand(int value);
    int getValueFromBBMCommand(int bbm_command);
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
    int getRequestCommand(int slotNr, int property);
    void setRequestReceived(int slotNr, int property);
    std::vector<int> getRemainingTelegrams();
    std::vector<int> getRemainingTelegrams(int slotNr);
    void clearRemainingTelegrams(int slotNr);

    /* XPT handling */
    void setXptConnected(bool flag);
    bool getXptConnected();
    void setXptSlotSource(int source);
    int getXptSlotSource(int slotNr);
    void setXptDestination(int destination);
    int getXptDestination();
    void setXptSlot(int slot);
    int getXptSlot();
    void setXptIpField(int type,int field,int value);
    int getXptIpField(int field);
    char* getXptIpAdress();
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

private:
    QStringList errorList;
    int answerStack;
    int count;
    int rotaryField;
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

    /* control/axis mapping */
    control_t controls[kAxisMax];

    /*XPT handling */
    bool xptConnect=false;
    bool xptEnabled=false;
    int xptDestination=0;
    int xptSlot;
    int xptNumberOfInputs;
    int xptNumberOfOutputs;
    char xptIpAddress[20];
    int xptFields[4];
    QList<QString> xptInputLabels;
    QList<QString> xptOutputLabels;
    int xptType;
    bool fastIris=false;


    // camera type 2 init values
    //todo: not all entries have 5 values!
    int c2Values[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,NORMAL},     //headnr init_value, min_value, max_value
                       {127,0,255,NORMAL,0,0}, //Iris
                       {64,0,128,CENTER,0,REQUESTABLE},  //Pedestal
                       {0,0,8000,NORMAL,0,REQUESTABLE},  //Focus
                       {127,0,255,CENTER,0,REQUESTABLE}, //w_Red
                       {127,0,255,CENTER,0,REQUESTABLE}, //w_Blue
                       {100,0,200,CENTER,0,REQUESTABLE}, //b_Red
                       {100,0,200,CENTER,0,REQUESTABLE}, //b_Blue
                       {0,0,14,TEXT,0,REQUESTABLE},    //Gain
                       {255,0,512,CENTER,0,REQUESTABLE},   //Gamma
                       {0,0,2,TEXT,1,REQUESTABLE},      //Gamma-Table
                       {128,1,254,CENTER,0,REQUESTABLE},  //Detail
                       {127,1,255,CENTER,0,REQUESTABLE},  //Color
                       {0,0,5,TEXT,2,REQUESTABLE},      //Color Temp
                       {-1,-1,-1,NAN},    //Knee
                       {-1,-1,-1,NAN},    //Knee Point
                       {0,0,3,TEXT,3,REQUESTABLE},       //ND Filter
                       {18,18,32,TEXT,4,REQUESTABLE},    //Shutter
                       {5,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {0,0,5,OFFSET},       //SPP1
                       {1,0,5,OFFSET},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {2,0,7,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT,5},       //Head Power
                       {0,0,3,TEXT,6}              //Mirror
                      };

    // camera type 1 init values
    int c1Values[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,NORMAL,0,0},     //headnr init_value, min_value, max_value
                       {127,0,255,NORMAL,0,0}, //Iris
                       {64,0,127,CENTER,0,REQUESTABLE},  //Pedestal
                       {0,0,8000,NORMAL,0,REQUESTABLE},  //Focus
                       {127,0,255,CENTER,0,REQUESTABLE}, //w_Red
                       {127,0,255,CENTER,0,REQUESTABLE}, //w_Blue
                       {100,0,200,CENTER,0,REQUESTABLE}, //b_Red
                       {100,0,200,CENTER,0,REQUESTABLE}, //b_Blue
                       {0,0,14,TEXT,0,REQUESTABLE},    //Gain
                       {-1,-1,-1,NAN},   //Gamma
                       {0,0,1,TEXT,1,REQUESTABLE},      //Gamma-Table
                       {128,0,254,CENTER,0,REQUESTABLE},  //Detail
                       {127,1,255,CENTER,0,REQUESTABLE},  //Color
                       {0,0,5,TEXT,2,REQUESTABLE},      //Color Temp
                       {-1,-1,-1,NAN},    //Knee
                       {-1,-1,-1,NAN},    //Knee Point
                       {-1,-1,-1,NAN},       //ND Filter
                       {6,6,20,TEXT,3,REQUESTABLE},    //Shutter
                       {1,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {0,0,5,OFFSET},       //SPP1
                       {1,0,5,OFFSET},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {2,0,7,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT,4},        //Head Power
                       {0,0,3,TEXT,5}              //Mirror
                      };

    // camera type 3&4 init values
    int rValues[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,NORMAL,0,0},     //headnr init_value, min_value, max_value
                       {2000,0,4000,NORMAL,0,0}, //Iris
                       {127,0,255,CENTER,0,0},  //Pedestal
                       {1000,1000,3250,NORMAL,0,REQUESTABLE},  //Focus
                       {127,0,255,CENTER,0,REQUESTABLE}, //w_Red
                       {127,0,255,CENTER,0,REQUESTABLE}, //w_Blue
                       {127,0,255,CENTER,0,REQUESTABLE}, //b_Red
                       {127,0,255,CENTER,0,REQUESTABLE}, //b_Blue
                       {0,0,9,TEXT,0,REQUESTABLE},    //Gain
                       {127,0,255,CENTER,0,REQUESTABLE},   //Gamma
                       {0,0,2,TEXT,1,REQUESTABLE},      //Gamma-Table
                       {127,0,255,CENTER,0,REQUESTABLE},  //Detail
                       {64,0,127,CENTER,0,REQUESTABLE},  //Color
                       {0,0,4,TEXT,2,REQUESTABLE},    //Color Temp
                       {0,0,2,TEXT,3,REQUESTABLE},    //Knee
                       {127,0,254,CENTER,0,REQUESTABLE},    //Knee Point
                       {-1,-1,-1,NAN},       //ND Filter
                       {0,0,6,TEXT,5,REQUESTABLE},    //Shutter
                       {5,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {0,0,5,OFFSET},       //SPP1
                       {1,0,5,OFFSET},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT,6},        //Head Power
                       {0,0,3,TEXT,7}               //Mirror
                      };

    // camera type 3&4 init values
    /*Problem with color temperature*/
    int ursaValues[ROW_ENTRIES][COLUM_ENTRIES]=
                      {{1,1,49,NORMAL,0,0},     //headnr init_value, min_value, max_value
                       {2000,0,4095,NORMAL,0,0}, //Iris
                       {127,0,400,CENTER,0,0},  //Pedestal
                       {-1,-1,-1,NAN},  //Focus
                       {400,1,800,CENTER,0,REQUESTABLE}, //w_Red
                       {400,1,800,CENTER,0,REQUESTABLE}, //w_Blue
                       {100,0,200,CENTER,0,REQUESTABLE}, //b_Red
                       {100,0,200,CENTER,0,REQUESTABLE}, //b_Blue
                       {1,1,4,NORMAL,0,REQUESTABLE},    //Gain
                       {0,0,2,NORMAL,0,REQUESTABLE},   //Gamma
                       {-1,-1,-1,NAN},      //Gamma-Table
                       {0,0,3,NORMAL,0,REQUESTABLE},  //Detail
                       {100,0,200,CENTER,0,REQUESTABLE},  //Color
                       {0,0,0,NORMAL,0,REQUESTABLE},    //Color Temp
                       {-1,-1,-1,NAN},    //Knee
                       {-1,-1,-1,NAN},    //Knee Point
                       {-1,-1,-1,NAN},       //ND Filter
                       {100,1,200,NORMAL,0,REQUESTABLE},    //Shutter
                       {5,1,10,NORMAL},      //PT Speed
                       {1,1,10,NORMAL},      //Trans Speed
                       {1,1,10,NORMAL},      //Ramp
                       {0,0,5,OFFSET},       //SPP1
                       {1,0,5,OFFSET},       //SPP2
                       {0,0,30,NORMAL},      //SPP Wait Time
                       {12,1,127,NORMAL},    //Bounce Zoom Speed
                       {0,0,2,TEXT,6},        //Head Power
                       {0,0,3,TEXT,7}               //Mirror
                      };
    int commandtype[ROW_ENTRIES]{-1,IRIS_OPEN,MASTER_PED_UP,FOCUS_SET_ABSOLUTE,
    RED_GAIN_ADJ_UP,BLUE_GAIN_ADJ_UP,RED_PED_UP,BLUE_PED_UP,CAMERA_GAIN_UP,GAMMA,
    GAMMA_TABLE,DETAIL_LEVEL_ADJ,COLOR_UP,WHITE_BALANCE_PRST,KNEE_POINT_AUTO,KNEE_POINT,ND_FILTER,
    SHUTTER_UP,PAN_TILT_SPEED,-1,RAMP,-1,-1,BNCE_WAIT_TIME,-1,HEAD_POWER,MIRROR_H_V};

    QString c1TextTable[6][15]={{"-10.5dB","-9dB","-7.5dB","-6dB","-4.5dB","-3dB","-1.5dB","0dB","1.5dB","3dB","4.5dB","6dB","7.5dB","9dB","10.5dB"},
                               {"High","Low"},
                                {"2800K","3200K","4000K","4600K","5600K","6300K"},
                                {"1/50","1/75","1/100","1/120","1/150","1/215","1/300","1/425","1/600","1/1000","1/1250","1/1750","1/2500","1/3500","1/6000"},
                                {"Low","Mid","High"},
                                {"Normal","H-Inverted","V-Inverted","HV-Inverted"}
                               };
    QString c2TextTable[7][15]={{"-10.5dB","-9dB","-7.5dB","-6dB","-4.5dB","-3dB","-1.5dB","0dB","1.5dB","3dB","4.5dB","6dB","7.5dB","9dB","10.5dB"},
                               {"Low","Mid","High"},
                                {"2800K","3200K","4000K","4600K","5600K","6300K"},
                                {"Clear","1/4","1/16","1/64"},
                                {"1/50","1/100","1/120","1/150","1/215","1/300","1/425","1/600","1/1000","1/1250","1/1750","1/2500","1/3500","1/6000"},
                                {"Low","Mid","High"},
                                {"Normal","H-Inverted","V-Inverted","HV-Inverted"}
                               };
    QString rTextTable[8][15]={{"-5dB","-4dB","-3dB","-2dB","-1dB","0dB","-1dB","2dB","3dB","4dB","5dB"},
                               {"Low","Mid","High"},
                                {"2800K","3200K","4000K","4600K","5600K"},
                                {"Off","On","Auto"},
                                {"Clear","1/4","1/16","1/64"},
                                {"1/50","1/75","1/100","1/120","1/150","1/215","1/300"},
                                {"Low","Mid","High"},
                               {"Normal","H-Inverted","V-Inverted","HV-Inverted"}
                               };
    QString ursaTextTable[8][15]={{"-5dB","-4dB","-3dB","-2dB","-1dB","0dB","-1dB","2dB","3dB","4dB","5dB"},
                               {"Low","Mid","High"},
                                {"2800K","3200K","4000K","4600K","5600K"},
                                {"Off","On","Auto"},
                                {"Clear","1/4","1/16","1/64"},
                                {"1/50","1/75","1/100","1/120","1/150","1/215","1/300"},
                                {"Low","Mid","High"},
                               {"Normal","H-Inverted","V-Inverted","HV-Inverted"}
                               };

};

#endif // MODEL_H
