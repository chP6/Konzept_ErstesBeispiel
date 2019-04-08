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
    void queueEvent(int evt, std::vector<int> data);
    void queueEvent(int evt, int singleData);
    void queueEvent(int evt, bool sta);

private:
    Model* model;
    EventQueue eventQueue;
    void processQeue();
    void increment(int inc);
    void clear();
    void setAxis(int x, int y);
};

#endif // CONTROLLER_H
