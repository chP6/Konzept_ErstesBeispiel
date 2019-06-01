#include <QApplication>
#include "view.h"
#include "model.h"
#include "controller.h"
#include "udplistener.h"
#include "poller.h"
#include <iostream>
#include <sstream>
#include "config.h"
#include <QFile>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    View view;
    Model model;
    Q_INIT_RESOURCE(styles);
    QFile File(":/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());

    view.setStyleSheet(StyleSheet);
    QObject::connect(&model, &Model::updateView,            // model signal mit view slot verbinden
                     &view, &View::on_modelUpdate);
    QObject::connect(&model, &Model::setUpView,            // model signal mit view slot verbinden
                     &view, &View::on_modelSetUp);
    QObject::connect(&model, &Model::updateServerConnectionStatus,
                     &view, &View::on_serverConnectionStatusChanged);
    QObject::connect(&model, &Model::updateCameraConnectionStatus,
                     &view, &View::on_cameraConnectionStatusChanged);
    QObject::connect(&model, &Model::updateXptConnectionStatus,
                     &view, &View::on_xptConnectionStatusChanged);
    QObject::connect(&model, &Model::updateXptEnableStatus,
                     &view, &View::on_xptEnableStatusChanged);
    QObject::connect(&model, &Model::newSignalReceived,
                     &view, &View::on_newReceive);
    QObject::connect(&model, &Model::receiveAllNew,
                     &view, &View::on_newRequest);


    Controller controller(model);
    //start queue was here

    QObject::connect(&controller, &Controller::clearLoadButon,
                     &view, &View::on_loadButtonCleared);


    view.setModelController(model, controller);
    view.show();

    Poller poller(controller);
    controller.setPoller(poller);
    UdpListener udpListener(controller);


    controller.startQueueProcessThread();
    poller.startListener();
    udpListener.startListener();

    return a.exec();
}
