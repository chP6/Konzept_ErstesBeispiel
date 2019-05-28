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
    blackMagic =new QAction(this);
    blackMagic->setText("Blackmagic");
    ross = new QAction(this);
    ross->setText("Ross");
    blackMagic->setCheckable(true);
    blackMagic->setChecked(true);
    ross->setCheckable(true);
    ross->setChecked(false);


    menu = new QMenu(this);
    menu->addAction(blackMagic);
    menu->addSeparator();
    menu->addAction(ross);
    ui->setupUi(this);


    ui->toolButton->setMenu(menu);
    ui->toolButton->setText(blackMagic->text());
    connect(blackMagic,SIGNAL(triggered()),this,SLOT(on_blackMagic_triggered()));
    connect(ross,SIGNAL(triggered()),this,SLOT(on_ross_triggered()));



}

XptControl::~XptControl()
{
    delete ui;
}

void XptControl::updateUi()
{
    QList<QString> inputLabels = model->getXptInputLables();
    QList<QString> outputLabels = model->getXptOutputLables();
    if(inputLabels.empty()){
    ui->btSource1->setText(QString::number(model->getXptSlotSource(0)));
    ui->btSource2->setText(QString::number(model->getXptSlotSource(1)));
    ui->btSource3->setText(QString::number(model->getXptSlotSource(2)));
    ui->btSource4->setText(QString::number(model->getXptSlotSource(3)));
    ui->btSource5->setText(QString::number(model->getXptSlotSource(4)));
    ui->btSource6->setText(QString::number(model->getXptSlotSource(5)));
    }
    else {
        ui->btSource1->setText(inputLabels[model->getXptSlotSource(0)-1]);
        ui->btSource2->setText(inputLabels[model->getXptSlotSource(1)-1]);
        ui->btSource3->setText(inputLabels[model->getXptSlotSource(2)-1]);
        ui->btSource4->setText(inputLabels[model->getXptSlotSource(3)-1]);
        ui->btSource5->setText(inputLabels[model->getXptSlotSource(4)-1]);
        ui->btSource6->setText(inputLabels[model->getXptSlotSource(5)-1]);
    }
    if(outputLabels.empty()){
        ui->btDestination->setText(QString::number(model->getXptDestination()));
    }
    else {
        ui->btDestination->setText(outputLabels[model->getXptDestination()-1]);
    }
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

void XptControl::xptEnableChanged(bool connected)
{
    ui->btConnect->setChecked(connected);
    model->setXptEnabled(connected);
    if(!connected){
        ui->btConnect->setText("Connect");
        ui->btXPTIp_1->setEnabled(true);
        ui->btXPTIp_2->setEnabled(true);
        ui->btXPTIp_3->setEnabled(true);
        ui->btXPTIp_4->setEnabled(true);
        ui->toolButton->setEnabled(true);

    }
    else{
        ui->btConnect->setText("Disconnect");
        ui->btXPTIp_1->setEnabled(false);
        ui->btXPTIp_2->setEnabled(false);
        ui->btXPTIp_3->setEnabled(false);
        ui->btXPTIp_4->setEnabled(false);
        ui->toolButton->setEnabled(false);
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
    model->setRotaryField(I_XPT_DESTINATION,INTERNAL);
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
        ui->toolButton->setEnabled(true);
        controller->queueEvent(E_XPT_CONNECT);

    }
    else{
        ui->btConnect->setText("Disconnect");
        ui->btXPTIp_1->setEnabled(false);
        ui->btXPTIp_2->setEnabled(false);
        ui->btXPTIp_3->setEnabled(false);
        ui->btXPTIp_4->setEnabled(false);
        ui->toolButton->setEnabled(false);
        controller->queueEvent(E_XPT_CONNECT);
    }
}

void XptControl::on_blackMagic_triggered()
{
    blackMagic->setChecked(true);
    ross->setChecked(false);
    ui->toolButton->setText(blackMagic->text());;
    model->setXptType(I_XPT_TYPE_BLACKMAGIC);
}

void XptControl::on_ross_triggered()
{
    blackMagic->setChecked(false);
    ross->setChecked(true);
    ui->toolButton->setText(ross->text());
    model->setXptType(I_XPT_TYPE_ROSS);
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
