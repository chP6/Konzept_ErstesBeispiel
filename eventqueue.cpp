#include "eventqueue.h"

bool EventQueue::isEmpty(){
    return queue.empty();
}

void EventQueue::qeueEvent(int evt, int val){
    eventStruct eventEntry;
    eventEntry.evt = evt;
    eventEntry.val = val;
    std::unique_lock<std::mutex> lock(mtx);     //kreiert unique_lock, lockt mtx beim erstellen, gibt frei bei zerstörung
    queue.push_back(eventEntry);
    cv.notify_one();                            //ankicken
}

//overload with bool
void EventQueue::qeueEvent(int evt, bool sta){
    eventStruct eventEntry;
    eventEntry.evt = evt;
    eventEntry.sta = sta;
    std::unique_lock<std::mutex> lock(mtx);
    queue.push_back(eventEntry);
    cv.notify_one();
}

void EventQueue::pullEvent(eventStruct& entry){
    std::unique_lock<std::mutex> lock(mtx);
    if(queue.empty()){                  //wenn queue leer, warten bis wieder angekickt, sonst alles abarbeiten
        cv.wait(lock);
    }
    entry = queue.front();
    queue.erase(queue.begin());
}
