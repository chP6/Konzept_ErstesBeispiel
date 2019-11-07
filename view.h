#ifndef VIE_H
#define VIE_H

#include <QObject>
#include "controller.h"
#include "model.h"
#include <csignal>
#include "events.h"
#include "config.h"
#include "backend.h"


class View : public QObject
{
    Q_OBJECT

public slots:
    void on_modelUpdate();
    void on_serverConnectionStatusChanged();
    void on_xptConnectionStatusChanged(bool connected);
    void on_modelUpdateProperty(properties_t property);
    void on_modelUpdateFlag(flags_t flag);
    void on_newError();

signals:
    void stackChanged();

public:
    View(Model& model, Controller& controller);
    ~View();

    Home homeBackend;
    CameraView cameraBackend;
    Others othersBackend;
    XptControl xptBackend;
    Controls controlsBackend;

private:
    Controller* controller;
    Model* model;
    static void signalHandler(int signum);
};

#endif // VIE_H
