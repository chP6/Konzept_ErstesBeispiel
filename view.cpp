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

    ui->home->setModelController(this->model, this->controller);
    ui->cameraView->setModelController(this->model, this->controller);
    ui->others->setModelController(this->model, this->controller);
}



void View::on_modelUpdate()
{

    ui->home->updateUi();
    ui->cameraView->updateUi();
    ui->others->updateUi();
}

void View::on_modelSetUp()
{
    ui->cameraView->setUpUi();
}

void View::on_sppUpdate(bool active)
{
    ui->home->sppUpdate(active);
}


void View::on_serverConnectionStatusChanged(bool connected)
{
    ui->home->serverConnectionChanged(connected);
}

void View::on_cameraConnectionStatusChanged(bool connected)
{
    ui->home->cameraConnectionChanged(connected);
}




void View::on_btHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->home->stackChanged();

}

void View::on_btCamCtrl_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->cameraView->stackChanged();

}

void View::on_btXptControl_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);

}

void View::on_btOthers_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->others->stackChanged();

}
