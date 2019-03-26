#include "eventqueue.h"

bool EventQueue::isEmpty(){
    return queue.empty();
}

void EventQueue::qeueEvent(int evt, int val){
    eventStruct eventEntry;
    eventEntry.evt = evt;
    eventEntry.val = val;
    mtx.lock();
    queue.push_back(eventEntry);
    mtx.unlock();
}

//overload with bool
void EventQueue::qeueEvent(int evt, bool sta){
    eventStruct eventEntry;
    eventEntry.evt = evt;
    eventEntry.sta = sta;
    mtx.lock();
    queue.push_back(eventEntry);
    mtx.unlock();
}

void EventQueue::pullEvent(eventStruct& entry){
    mtx.lock();
    entry = queue.front();
    queue.erase(queue.begin());
    mtx.unlock();
}
