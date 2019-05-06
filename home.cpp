#include "home.h"
#include "ui_home.h"
#include <QDebug>

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

void Home::on_btBounce_clicked(bool checked)
{
    qDebug()<<"halloos";
}

void Home::on_btWideSet_clicked()
{

}

void Home::on_btAutoZoomSpeed_clicked()
{

}

void Home::on_btTransSpeed_clicked()
{

}

void Home::on_btPanTiltSpeed_clicked()
{

}

void Home::on_btRamp_clicked()
{

}

void Home::on_btFasttrans_clicked(bool checked)
{

}

void Home::on_btSpp1_clicked()
{

}

void Home::on_btSpp2_clicked()
{

}

void Home::on_btSppWait_clicked()
{

}

void Home::on_btSppStart_clicked(bool checked)
{

}

void Home::on_btHeadNr_clicked()
{

}

void Home::on_btStorePreset_clicked()
{

}
