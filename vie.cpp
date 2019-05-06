#include "vie.h"
#include "ui_vie.h"
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
    errorFenster.setModelController(this->model, this->controller);
}

void View::on_modelUpdate()
{

}

void View::on_serverConnectionStatusChanged(bool connected)
{

}



void View::on_btHome_2_clicked()
{

}
