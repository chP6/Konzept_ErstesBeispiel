#include <QApplication>
#include "view.h"
#include "model.h"
#include "controller.h"
#include "udplistener.h"
#include "poller.h"
#include <iostream>
#include <sstream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    View view;
    Model model;


    QObject::connect(&model, &Model::updateView,            // model signal mit view slot verbinden
                     &view, &View::on_modelUpdate);

    Controller controller(model);
    controller.startQueueProcessThread();

    view.setModelController(model, controller);
    view.show();

    Poller poller(controller);
    poller.startListener();
    UdpListener udpListener(controller);
    udpListener.startListener();

    return a.exec();
}
