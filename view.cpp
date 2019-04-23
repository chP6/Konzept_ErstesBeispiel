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
    errorFenster.setModelController(this->model, this->controller);
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
    errorFenster.update();  //noch entkoppeln vom update ->fokus geht verloren, oder kein fokus erlauben

    int x,y;
    model->getAxis(x,y);
    ui->hSlider->setValue(x);
    ui->vSlider->setValue(y);
}

void View::on_pbErrors_clicked()
{
    errorFenster.show();
}

void View::on_storePreset_clicked()
{
    controller->queueEvent(E_STORE_PRESET);
}
