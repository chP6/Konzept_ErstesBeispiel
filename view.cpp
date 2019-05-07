#include "view.h"
#include "ui_view.h"
#include "controller.h"
#include "model.h"
#include "QString"
#include "events.h"
#include "config.h"

View::View(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::View)
{
    ui->setupUi(this);
}

View::~View()
{
    delete ui;
}

void View::setModelController(Model& model, Controller& controller){
    this->controller = &controller;
    this->model = &model;
    ui->errorWindow->setModelController(this->model, this->controller);
    ui->home->setModelController(this->model, this->controller);
    ui->cameraView->setModelController(this->model, this->controller);
}



void View::on_modelUpdate()
{
    ui->errorWindow->update();
    ui->home->update();
    ui->cameraView->update();
}

void View::on_modelSetUp()
{
    ui->cameraView->setUpUi();
}



void View::on_serverConnectionStatusChanged(bool connected)
{
    ui->home->serverConnectionChanged(connected);
}




void View::on_btHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void View::on_btCamCtrl_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void View::on_btXptControl_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void View::on_btOthers_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
