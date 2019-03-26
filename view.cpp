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

// Es ist nur erlaubt direkt Werte vom Model zu LESEN! Alles andere muss über den Controller laufen.
void View::updateAll(){
    ui->label->setText(QString::number(model->getData()));
}
