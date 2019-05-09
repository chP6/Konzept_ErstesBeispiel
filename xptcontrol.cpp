#include "xptcontrol.h"
#include "ui_xptcontrol.h"

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
