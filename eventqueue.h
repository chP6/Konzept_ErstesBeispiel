#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <vector>
#include <mutex>

struct eventStruct{
  int evt;
  int val;
  bool sta;
};

class EventQueue
{
public:
    bool isEmpty();
    void qeueEvent(int evt, int val);
    void qeueEvent(int evt, bool sta);
    void pullEvent(eventStruct& entry);
private:
    std::vector<eventStruct> queue;
    std:: mutex mtx;
};

#endif // EVENTQUEUE_H
