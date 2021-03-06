#include "home.h"
#include "ui_home.h"
#include <QDebug>
#include <QString>
#include <config.h>
#include "model.h"
#include "controller.h"
#include "events.h"

Home::Home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home)
{
    ui->setupUi(this);

}

Home::~Home()
{
    delete ui;
}

void Home::updateUi()
{
    /*update every button with the respecive value*/
    ui->btPanTiltSpeed->setText(QString::number(model->getValue(DISP,V_PT_SPEED)));
    ui->btTransSpeed->setText(QString::number(model->getValue(DISP,V_TRANS_SPEED)));
    ui->btRamp->setText(QString::number(model->getValue(DISP,V_RAMP)));
    ui->btSpp1->setText(QString::number(model->getValue(DISP,V_SPP1)));
    ui->btSpp2->setText(QString::number(model->getValue(DISP,V_SPP2)));
    ui->btSppWait->setText(QString::number(model->getValue(DISP,V_SPP_WAIT_TIME)));
    ui->btHeadNr->setText(QString::number(model->getValue(DISP,V_HEADNR)));
    ui->lPresetNr->setText(QString::number(model->getActivePreset()+1));
    ui->btAutoZoomSpeed->setText(QString::number(model->getValue(DISP,V_BOUNCE_ZOOM_SPEED)));
    ui->btBounce->setChecked(model->getCamFlag(F_BOUNCE_ENABLE));
    ui->btFasttrans->setChecked(model->getCamFlag(F_FASTTRANS));
    ui->btSppStart->setChecked(model->getCamFlag(F_SPP_ON));
}

void Home::setModelController(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
}

void Home::serverConnectionChanged(bool connection)
{
    /*indicate the server connection according to the connection flag*/
    if(connection){ui->lServer->setText("OK");}
    else {
        ui->lServer->setText("Not OK");
    }
}

void Home::cameraConnectionChanged(bool connection)
{
    /*indicate the camera connection according to the connection flag*/
    if(connection){ui->lCamera->setText("OK");}
    else {
        ui->lCamera->setText("Not OK");
    }
}

/*Qt Slots for button clicks **************/
void Home::on_btBounce_clicked(bool checked)
{
    ui->btWideSet->setEnabled(checked);         //ebable whideSet button only wehn Bounce mode is enabled
    model->setCamFlag(F_BOUNCE_ENABLE,checked); //set Flag
    //controller->queueEvent(E_BOUNCE);           //add the event in the queue
}

void Home::on_btWideSet_clicked()
{
    controller->queueEvent(E_WIDESET);          //add the event in the queue
}

void Home::on_btFasttrans_clicked(bool checked)
{
    model->setCamFlag(F_FASTTRANS,checked);     //add the event in the queue
}

void Home::on_btSppStart_clicked(bool checked)
{

    if(!checked){
        controller->queueEvent(E_SPP_ABORT);    //add the event in the queue
    }
    else{       //toggle
        controller->queueEvent(E_SPP_START);    //add the event in the queue
    }

}
void Home::on_btStorePreset_clicked()
{
    controller->queueEvent(E_STORE_PRESET);
}

/*point to the respective field, the rotary encoder will queue the event
  value can be sent
***********/
void Home::on_btAutoZoomSpeed_clicked()
{

    model->setRotaryField(V_BOUNCE_ZOOM_SPEED,SEND);
}

void Home::on_btTransSpeed_clicked()
{
    model->setRotaryField(V_TRANS_SPEED,SEND);
}

void Home::on_btPanTiltSpeed_clicked()
{
    model->setRotaryField(V_PT_SPEED,SEND);
}

void Home::on_btRamp_clicked()
{
    model->setRotaryField(V_RAMP,SEND);
}

void Home::on_btSpp1_clicked()
{
    model->setRotaryField(V_SPP1,SEND);
}

void Home::on_btSpp2_clicked()
{
    model->setRotaryField(V_SPP2,SEND);
}

void Home::on_btSppWait_clicked()
{
    model->setRotaryField(V_SPP_WAIT_TIME,SEND);
}

void Home::on_btHeadNr_clicked()
{
    model->setRotaryField(V_HEADNR,SEND);
}
/****************/
/*******************/

void Home::stackChanged()
{
    /*Pres the last prest button in this window again*/
    QPushButton *button=ui->btRamp;                                     //dummy
    QList<QPushButton*> allButtons=this->findChildren<QPushButton*>();  //find all buttons and put them in a list
    for(int i=0;i<allButtons.size();i++){
        if(allButtons[i]->isChecked() && allButtons[i]->autoExclusive())    //find the one which was checked there are buttons which aren't autoexclusive, ignore them
        {button=allButtons[i];}         //find the one which was checked

    }
    button->click();                //click it
}

void Home::sppUpdate(bool active)
{
    ui->btSppStart->setChecked(active); //check according to active
}




