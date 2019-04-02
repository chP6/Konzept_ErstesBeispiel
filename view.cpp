#include "view.h"
#include "ui_view.h"
#include "controller.h"
#include "model.h"
#include "QString"
#include "events.h"

View::View(QWidget *parent) :
    QWidget(parent),
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
}

void View::on_pbInc_clicked()
{
    controller->queueEvent(E_INCREASE, 1);
}

void View::on_pbClear_clicked()
{
    controller->queueEvent(E_CLEAR, 1);
}

void View::on_modelUpdate(){
    ui->label->setText(QString::number(model->getData()));
}
