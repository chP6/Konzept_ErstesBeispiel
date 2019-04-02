#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <eventqueue.h>

class Model;        //forward declaration

class Controller
{
public:
    Controller(Model& model);
    void setModel(Model& model);
    void startQueueProcessThread();
    void queueEvent(int evt, int val);
    void queueEvent(int evt, bool sta);

private:
    Model* model;
    EventQueue eventQueue;
    void processQeue();
    void increment(int inc);
    void clear();
};

#endif // CONTROLLER_H
