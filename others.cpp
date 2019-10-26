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
    errorWindow->close();
}

Others::~Others()
{
    delete ui;
}

void Others::updateUi()
{
    /*update every button with the respecive value*/
    errorWindow->updateUi();
    ui->btHeadPower->setText(model->getTextValue(V_HEAD_POWER));
    ui->btMirror->setText(model->getTextValue(V_MIRROR));
    ui->btPanInvert->setChecked(model->getCamFlag(F_PAN_INVERT));
    ui->btTiltInvert->setChecked(model->getCamFlag(F_TILT_INVERT));
    ui->btZoomInvert->setChecked(model->getCamFlag(F_ZOOM_INVERT));
    ui->btFocusInvert->setChecked(model->getCamFlag(F_FOCUS_INVERT));
}

void Others::clearLoadButton(){
    ui->btLoad->setEnabled(true);
    //ui->btLoad->setChecked(false);
}

void Others::setModelController(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
    errorWindow->setModelController(model,controller);

}

void Others::on_btPanInvert_clicked(bool checked)
{
     model->setCamFlag(F_PAN_INVERT,checked);
}

void Others::on_btTiltInvert_clicked(bool checked)
{
    model->setCamFlag(F_TILT_INVERT,checked);
}

void Others::on_btZoomInvert_clicked(bool checked)
{
    model->setCamFlag(F_ZOOM_INVERT,checked);
}

void Others::on_btFocusInvert_clicked(bool checked)
{
    model->setCamFlag(F_FOCUS_INVERT,checked);
}

void Others::on_btMirror_clicked()
{
    model->setRotaryField(V_MIRROR,SEND);
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
    model->setRotaryField(V_HEAD_POWER,SEND);
}

void Others::on_btRequest_clicked()
{
    //controller->requestCameraSettings();
    controller->queueEvent(E_REQUEST_SETTINGS);
}

void Others::on_btSave_clicked()
{
    QApplication::quit();
   // controller->queueEvent(E_WRITE_SAVEFILE);
}

void Others::on_btLoad_clicked()
{
    controller->queueEvent(E_LOAD_SAVEFILE);
    ui->btLoad->setEnabled(false);
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
        if(allButtons[i]->isChecked() && allButtons[i]->autoExclusive() )
        {button=allButtons[i];}

    }
    button->click();
    updateUi();
}
