#include "eventqueue.h"


bool EventQueue::isEmpty(){
    return queue.empty();
}

void EventQueue::qeueEvent(int evt, std::vector<int> data){
    //gettimeofday(&tv1, NULL);

    event_s eventEntry;
    eventEntry.evt = evt;
    eventEntry.data = data;
    std::unique_lock<std::mutex> lock(mtx);     //kreiert unique_lock, lockt mtx beim erstellen, gibt frei bei zerstörung
    queue.push_back(eventEntry);
    cv.notify_one();                            //ankicken

//    gettimeofday(&tv2, NULL);
//    qDebug("Push: %f seconds",
//         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//         (double) (tv2.tv_sec - tv1.tv_sec));
}

//overload with bool
void EventQueue::qeueEvent(int evt, bool sta){
    event_s eventEntry;
    eventEntry.evt = evt;
    eventEntry.sta = sta;
    std::unique_lock<std::mutex> lock(mtx);
    queue.push_back(eventEntry);
    cv.notify_one();
}


//no data eg. Watchdog
void EventQueue::qeueEvent(int evt){

//    gettimeofday(&tv1, NULL);

    event_s eventEntry;
    eventEntry.evt = evt;
    std::unique_lock<std::mutex> lock(mtx);
    queue.push_back(eventEntry);
    cv.notify_one();

//    gettimeofday(&tv2, NULL);
//    qDebug("Push: %f seconds",
//         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//         (double) (tv2.tv_sec - tv1.tv_sec));
}

void EventQueue::pullEvent(event_s& entry){
//    gettimeofday(&tv1, NULL);

    std::unique_lock<std::mutex> lock(mtx);
    if(queue.empty()){                  //wenn queue leer, warten bis wieder angekickt, sonst alles abarbeiten
        cv.wait(lock);
    }
    entry = queue.front();
    queue.erase(queue.begin());
//    gettimeofday(&tv2, NULL);
//    qDebug("Pull: %f seconds",
//         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//         (double) (tv2.tv_sec - tv1.tv_sec));
}

void EventQueue::initCleanup(){
    //clear queue from any pan/tilt/zoom events (unwanted from init)
    for (unsigned int i=0;i<queue.size();i++) {
        if(queue[i].evt == E_SET_TILT || queue[i].evt == E_SET_ZOOM){
            queue[i].evt = E_NULLEVENT;
            qDebug("Cleared one");
        }
    }
}
