#include "others.h"
#include "ui_others.h"
#include <QDebug>
#include <QString>
#include <config.h>
#include "model.h"
#include "controller.h"
#include "events.h"


Others::Others(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Others)
{
    ui->setupUi(this);
    errorWindow=new ErrorWindow(this);
}

Others::~Others()
{
    delete ui;
}

void Others::updateUi()
{
    errorWindow->updateUi();
    ui->btHeadPower->setText(model->getTextValue(V_HEAD_POWER));
    ui->btMirror->setText(model->getTextValue(V_MIRROR));
}

void Others::setModelController(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
    errorWindow->setModelController(model,controller);

}

void Others::on_btPanInvert_clicked(bool checked)
{

}

void Others::on_btTiltInvert_clicked(bool checked)
{

}

void Others::on_btZoomInvert_clicked(bool checked)
{

}

void Others::on_btFocusInvert_clicked(bool checked)
{

}

void Others::on_btMirror_clicked()
{
    model->setRotaryField(V_MIRROR);
}

void Others::on_btUpperLimit_clicked()
{
    controller->queueEvent(E_SET_UPPER_LIMIT);
}

void Others::on_btLowerLimit_clicked()
{
    controller->queueEvent(E_SET_LOWER_LIMIT);
}

void Others::on_btClearLimit_clicked()
{
    controller->queueEvent(E_CLEAR_LIMIT);
}

void Others::on_btCalibrate_clicked()
{
    controller->queueEvent(E_CALIB_HEAD);
}

void Others::on_btHeadPower_clicked()
{
    model->setRotaryField(V_HEAD_POWER);
}

void Others::on_btRequest_clicked()
{
    controller->requestCameraSettings();
}

void Others::on_btSave_clicked()
{
    controller->writeSavefile();
}

void Others::on_btLoad_clicked()
{
    controller->loadSavefile();
}

void Others::on_btErrorWindow_clicked()
{
    errorWindow->show();
}

void Others::stackChanged()
{
    QPushButton *button=ui->btMirror;
    QList<QPushButton*> allButtons=this->findChildren<QPushButton*>();
    for(int i=0;i<allButtons.size();i++){
        if(allButtons[i]->isChecked())
        {button=allButtons[i];}

    }
    button->click();
}



