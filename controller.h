#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "eventqueue.h"
#include "networkinterface.h"
#include "tastenfeld.h"
#include "generictimer.h"
#include "xptinterface.h"
#include "config.h"



#define SAVEFILE_PATH   "/opt/savefile"

class Model;        //forward declaration
class Poller;        //forward declaration

class Controller : public QObject
{
    Q_OBJECT        //qmake vor erstem kompilieren laufen lassen!

signals:
    void clearLoadButon();
public slots:
    void onXptLableChanged();
public:
    Controller(Model& model);
    void setModel(Model& model);
    void setPoller(Poller& poller);
    void startQueueProcessThread();
    void queueEvent(int evt);
    void queueEvent(int evt, int dataA);
    void queueEvent(int evt, int dataA, int dataB);
    void queueEvent(int evt, int dataA, int dataB, int dataC);
    void queueEvent(int evt, std::vector<int> data);
    void queueEvent(int evt, bool sta);
    void logSystemError(int err_no, std::string msg);
    void logError(std::string msg);
    void clearErrors();
    int writeSavefile();
    int loadSavefile();
    void alignSlots(int value);
    void requestCameraSettings(int slot);
    void checkSettingsRequest(int slotNr);


private:
    Model* model;
    Poller* poller;
    EventQueue eventQueue;
    Networkinterface txSocket;
    Tastenfeld presetbus;
    Tastenfeld camerabus;
    XptInterface xptSocket;
    GenericTimer blinkTimer, sppTimer[NUMBER_OF_SLOTS], xptWatchdog, reqSettingsTimer;


    void processQeue();
    void increment(int inc);
    void clear();
    void setAxis(int x, int y);
    int contr_err;

};

#endif // CONTROLLER_H
