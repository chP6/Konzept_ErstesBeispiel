#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <vector>
#include <mutex>
#include <condition_variable>

struct event_s{
  int evt;
  std::vector<int> data;
  bool sta;
};

class EventQueue
{
public:
    bool isEmpty();
    void qeueEvent(int evt, std::vector<int> data);
    void qeueEvent(int evt, bool sta);
    void qeueEvent(int evt, unsigned char number);
    void qeueEvent(int evt);
    void pullEvent(event_s& entry);
    void initCleanup();
private:
    std::vector<event_s> queue;
    std:: mutex mtx;
    std::condition_variable cv;
    bool init = true;
    struct timeval  tv1, tv2;
};

#endif // EVENTQUEUE_H
