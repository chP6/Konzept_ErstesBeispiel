#include "xptcontrol.h"
#include "ui_xptcontrol.h"
#include "model.h"
#include "controller.h"
#include "events.h"
#include <QString>

XptControl::XptControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XptControl)
{
    ui->setupUi(this);
}

XptControl::~XptControl()
{
    delete ui;
}

void XptControl::updateUi()
{

    ui->btSource1->setText(QString::number(model->getXptSlotSource(0)));
    ui->btSource2->setText(QString::number(model->getXptSlotSource(1)));
    ui->btSource3->setText(QString::number(model->getXptSlotSource(2)));
    ui->btSource4->setText(QString::number(model->getXptSlotSource(3)));
    ui->btSource5->setText(QString::number(model->getXptSlotSource(4)));
    ui->btSource6->setText(QString::number(model->getXptSlotSource(5)));
    ui->btDestination->setText(QString::number(model->getXptDestination()));
    ui->btXPTIp_1->setText(QString::number(model->getXptIpField(0)));
    ui->btXPTIp_2->setText(QString::number(model->getXptIpField(1)));
    ui->btXPTIp_3->setText(QString::number(model->getXptIpField(2)));
    ui->btXPTIp_4->setText(QString::number(model->getXptIpField(3)));


}

void XptControl::xptStatusChanged(bool connected)
{
    if(connected){
        ui->lStatus->setText("Connected");
    }
    else{
       ui->lStatus->setText("Not Connected");
    }

}

void XptControl::setModelController(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
}
void XptControl::on_btSource1_clicked()
{
    model->setXptSlot(0);
    model->setRotaryField(I_XPT_SOURCE,INTERNAL);

}

void XptControl::on_btSource2_clicked()
{
    model->setXptSlot(1);
    model->setRotaryField(I_XPT_SOURCE,INTERNAL);
}

void XptControl::on_btSource3_clicked()
{
    model->setXptSlot(2);
    model->setRotaryField(I_XPT_SOURCE,INTERNAL);
}

void XptControl::on_btSource4_clicked()
{
    model->setXptSlot(3);
    model->setRotaryField(I_XPT_SOURCE,INTERNAL);
}

void XptControl::on_btSource5_clicked()
{
    model->setXptSlot(4);
    model->setRotaryField(I_XPT_SOURCE,INTERNAL);
}

void XptControl::on_btSource6_clicked()
{
    model->setXptSlot(5);
    model->setRotaryField(I_XPT_SOURCE,INTERNAL);
}

void XptControl::on_btDestination_clicked()
{
    model->setRotaryField(I_XPT_SOURCE,INTERNAL);
}

void XptControl::on_btXPTIp_1_clicked()
{
    model->setRotaryField(I_XPT_IP_FIELD_1,INTERNAL);
}

void XptControl::on_btXPTIp_2_clicked()
{
    model->setRotaryField(I_XPT_IP_FIELD_2,INTERNAL);
}

void XptControl::on_btXPTIp_3_clicked()
{
    model->setRotaryField(I_XPT_IP_FIELD_3,INTERNAL);
}

void XptControl::on_btXPTIp_4_clicked()
{
    model->setRotaryField(I_XPT_IP_FIELD_4,INTERNAL);
}



void XptControl::on_btConnect_clicked(bool checked)
{
    model->setXptEnabled(checked);
    if(!checked){
        ui->btConnect->setText("Connect");
        ui->btXPTIp_1->setEnabled(true);
        ui->btXPTIp_2->setEnabled(true);
        ui->btXPTIp_3->setEnabled(true);
        ui->btXPTIp_4->setEnabled(true);
        controller->queueEvent(E_XPT_CONNECT);

    }
    else{
        ui->btConnect->setText("Disconnect");
        ui->btXPTIp_1->setEnabled(false);
        ui->btXPTIp_2->setEnabled(false);
        ui->btXPTIp_3->setEnabled(false);
        ui->btXPTIp_4->setEnabled(false);
        controller->queueEvent(E_XPT_CONNECT);
    }
}

void XptControl::stackChanged()
{
    QPushButton *button=ui->btSource1;
    QList<QPushButton*> allButtons=this->findChildren<QPushButton*>();
    for(int i=0;i<allButtons.size();i++){
        if(allButtons[i]->isChecked() && allButtons[i]->autoExclusive())
        {button=allButtons[i];}

    }
    button->click();
}
