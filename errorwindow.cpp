#include "errorwindow.h"
#include "ui_errorwindow.h"
#include <QStringListModel>
#include "model.h"
#include "controller.h"
#include "config.h"

ErrorWindow::ErrorWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ErrorWindow)
{
    ui->setupUi(this);
    listModel = new QStringListModel(this);
}

ErrorWindow::~ErrorWindow()
{
    delete ui;
}

void ErrorWindow::on_pbEwClose_clicked()
{
    this->close();
}

void ErrorWindow::setModelController(Model* model, Controller* controller){
    this->model = model;
    this->controller = controller;
}

void ErrorWindow::update(){
    listModel->setStringList(*model->getErrorList());
    ui->listView->setModel(listModel);
}

void ErrorWindow::on_pbEwClear_clicked()
{
    controller->clearErrors();
}
