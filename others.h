#ifndef OTHERS_H
#define OTHERS_H

#include "model.h"
#include "controller.h"
#include "events.h"
#include <QObject>

class Others : public QObject
{
    Q_OBJECT

public:
    explicit Others(Model* model, Controller* controller);
    ~Others();

private slots:



private:

    Model* model;
    Controller* controller;
};

#endif // OTHERS_H
