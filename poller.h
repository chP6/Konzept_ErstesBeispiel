#ifndef POLLER_H
#define POLLER_H

class Controller;
class Poller
{
public:
    Poller(Controller& controller);
    void startMetronom();
private:
    void metronom();
    Controller* controller;
};

#endif // POLLER_H
