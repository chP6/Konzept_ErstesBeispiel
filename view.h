#ifndef VIE_H
#define VIE_H

#include <QObject>
#include "controller.h"
#include "model.h"
#include <csignal>
#include "events.h"
#include "config.h"
#include "home.h"
#include "cameraview.h"
#include "xptcontrol.h"
#include "others.h"

class View : public QObject
{
    Q_OBJECT

public slots:
    void on_modelUpdate();
    void on_modelSetUp();
    void on_sppUpdate(bool active);
    void on_serverConnectionStatusChanged();
    void on_cameraConnectionStatusChanged(bool connected);
    void on_xptConnectionStatusChanged(bool connected);
    void on_xptEnableStatusChanged(bool connected);
    void on_loadButtonCleared();
    void on_newReceive(int property);
    void on_newRequest();
    void on_modelUpdateProperty(properties_t property);
    void on_modelUpdateFlag(flags_t flag);

private slots:
    void on_btHome_clicked();
    void on_btCamCtrl_clicked();
    void on_btXptControl_clicked();
    void on_btOthers_clicked();
    void on_btControls_clicked();

signals:
    void stackChanged();

public:
    explicit View(Model& model, Controller& controller);
    ~View();
    Home homeBackend;
    CameraView cameraBackend;
    Others othersBackend;
    XptControl xptBackend;

private:

    Controller* controller;
    Model* model;
    static void signalHandler(int signum);
};

#endif // VIE_H
