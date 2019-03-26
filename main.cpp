#include <QApplication>
#include "view.h"
#include "model.h"
#include "controller.h"
#include "poller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    View view;
    Model model(view);

    Controller controller(model);
    controller.startQueueProcessThread();

    view.setModelController(model, controller);
    view.show();

    Poller poller(controller);
    poller.startMetronom();

    return a.exec();
}
