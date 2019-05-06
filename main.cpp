#include <QApplication>
#include "view.h"
#include "model.h"
#include "controller.h"
#include "udplistener.h"
#include "poller.h"
#include <iostream>
#include <sstream>
#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    View view;
    Model model;


    QObject::connect(&model, &Model::updateView,            // model signal mit view slot verbinden
                     &view, &View::on_modelUpdate);
    QObject::connect(&model, &Model::updateServerConnectionStatus,
                     &view, &View::on_serverConnectionStatusChanged);


    Controller controller(model);
    controller.startQueueProcessThread();

    view.setModelController(model, controller);
    view.show();

    Poller poller(controller);
    //UdpListener udpListener(controller);
    poller.startListener();
    //udpListener.startListener();

    return a.exec();
}
