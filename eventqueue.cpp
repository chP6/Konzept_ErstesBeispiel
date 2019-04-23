#include "eventqueue.h"
#include "config.h"

bool EventQueue::isEmpty(){
    return queue.empty();
}

void EventQueue::qeueEvent(int evt, std::vector<int> data){
    Event_s eventEntry;
    eventEntry.evt = evt;
    eventEntry.data = data;
    std::unique_lock<std::mutex> lock(mtx);     //kreiert unique_lock, lockt mtx beim erstellen, gibt frei bei zerstörung
    queue.push_back(eventEntry);
    cv.notify_one();                            //ankicken
}

//overload with bool
void EventQueue::qeueEvent(int evt, bool sta){
    Event_s eventEntry;
    eventEntry.evt = evt;
    eventEntry.sta = sta;
    std::unique_lock<std::mutex> lock(mtx);
    queue.push_back(eventEntry);
    cv.notify_one();
}

void EventQueue::qeueEvent(int evt, unsigned char number)
{
    Event_s eventEntry;
    eventEntry.evt = evt;
    eventEntry.number = number;
    std::unique_lock<std::mutex> lock(mtx);
    queue.push_back(eventEntry);
    cv.notify_one();
}

//no data eg. Watchdog
void EventQueue::qeueEvent(int evt){
    Event_s eventEntry;
    eventEntry.evt = evt;
    std::unique_lock<std::mutex> lock(mtx);
    queue.push_back(eventEntry);
    cv.notify_one();
}

void EventQueue::pullEvent(Event_s& entry){
    std::unique_lock<std::mutex> lock(mtx);
    if(queue.empty()){                  //wenn queue leer, warten bis wieder angekickt, sonst alles abarbeiten
        cv.wait(lock);
    }
    entry = queue.front();
    queue.erase(queue.begin());
}
