#ifndef XPTCONTROL_H
#define XPTCONTROL_H


#include "model.h"
#include "controller.h"
#include "events.h"
#include <QObject>

class XptControl : public QObject
{
    Q_OBJECT

public:
    explicit XptControl(Model* model, Controller* controller);
    ~XptControl();

private slots:


private:

    Model* model;
    Controller* controller;

};

#endif // XPTCONTROL_H
