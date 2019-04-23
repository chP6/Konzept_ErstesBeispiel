#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <eventqueue.h>
#include "networkinterface.h"
#include "tastenfeld.h"

class Model;        //forward declaration

class Controller
{
public:
    Controller(Model& model);
    void setModel(Model& model);
    void startQueueProcessThread();
    void queueEvent(int evt);
    void queueEvent(int evt, std::vector<int> data);
    void queueEvent(int evt, int singleData);
    void queueEvent(int evt, bool sta);
    void logSystemError(int err_no, std::string msg);
    void logError(std::string msg);
    void clearErrors();

private:
    Model* model;
    EventQueue eventQueue;
    Networkinterface txSocket;
    Tastenfeld presetbus;
    Tastenfeld camerabus;
    void processQeue();
    void increment(int inc);
    void clear();
    void setAxis(int x, int y);
    int contr_err;
};

#endif // CONTROLLER_H
