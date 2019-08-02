#include "view.h"
#include "ui_view.h"
#include "controller.h"
#include "model.h"
#include "QString"
#include "events.h"
#include "config.h"
#include <csignal>

View::View(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::View)
{
    ui->setupUi(this);
    std::signal(SIGINT,View::signalHandler);
}

View::~View()
{
    delete ui;
}

void View::setModelController(Model& model, Controller& controller){
    /*give the view and the sub views the model and the controller such that they can interact*/
    this->controller = &controller;
    this->model = &model;

    ui->home->setModelController(this->model, this->controller);
    ui->cameraView->setModelController(this->model, this->controller);
    ui->others->setModelController(this->model, this->controller);
    ui->xptControl->setModelController(this->model,this->controller);
}

void View::signalHandler(int signum)
{
    printf("Why do you kill me %d \n",signum);
    QApplication::quit();
}



void View::on_modelUpdate()
{
    /*QtSlot: update all views  - connected to Model: updateView()*/
    ui->home->updateUi();
    ui->cameraView->updateUi();
    ui->others->updateUi();
    ui->xptControl->updateUi();
}

void View::on_modelSetUp()
{
    /*QtSlot: set up camera view -> see setUpUi  - connected to Model: setUpView()*/
    ui->cameraView->setUpUi();
}

void View::on_sppUpdate(bool active)
{
    /*QtSlot: start or stop at spp - connected to Model: ?*/
    ui->home->sppUpdate(active);
}


void View::on_serverConnectionStatusChanged(bool connected)
{
    /*QtSlot: server connected indicator  - connected to Model: updateServerConnectionStatus()*/
    ui->home->serverConnectionChanged(connected);
}

void View::on_cameraConnectionStatusChanged(bool connected)
{
    /*QtSlot: camera connected indicator  - connected to Model: updateCameraConnectionStatus()*/
    ui->home->cameraConnectionChanged(connected);
}


void View::on_xptConnectionStatusChanged(bool connected)
{

    /*QtSlot: xpt connected indicator  - connected to Model: updateXptConnectionStatus()*/
    ui->xptControl->xptStatusChanged(connected);
}

void View::on_xptEnableStatusChanged(bool connected)
{
    /*QtSlot: to release connect button if connection fails  - connected to Model: updateXptEnableStatus()*/
    ui->xptControl->xptEnableChanged(connected);
}


void View::on_loadButtonCleared()
{
    /*?*/
    ui->others->clearLoadButton();

}

void View::on_newReceive(int property)
{
    /*QtSlot: to indicate a new received value from camera - connected to Model: newSignalReceived()*/
    ui->cameraView->signalRequest(property);
}

void View::on_newRequest()
{
  /*QtSlot: to indicate everything will be requested again - connected to Model: receiveAllNew()*/
 ui->cameraView->newRequest();
}




void View::on_btHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);  //change window to home
    ui->home->stackChanged();               //push last pressed button on this page again
}

void View::on_btCamCtrl_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);  //change window to camera control
    ui->cameraView->stackChanged();         //push last pressed button on this page again

}

void View::on_btXptControl_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);  //change window to xpt control
    ui->xptControl->stackChanged();        //push last pressed button on this page again

}

void View::on_btOthers_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);  //change window to others
    ui->others->stackChanged();             //push last pressed button on this page again

}


