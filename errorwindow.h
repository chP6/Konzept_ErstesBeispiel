#ifndef ERRORWINDOW_H
#define ERRORWINDOW_H

//#include <QWidget>
#include <QStringListModel>
#include <QObject>
#include "model.h"
#include "controller.h"

class ErrorWindow : QObject
{
    Q_OBJECT

public:
    explicit ErrorWindow();
    ~ErrorWindow();
    void setModelController(Model* model, Controller* controller);
    void updateUi();

private slots:
    void on_pbEwClose_clicked();
    void on_pbEwClear_clicked();

private:
    QStringListModel *listModel;
    Model* model;
    Controller* controller;
};

#endif // ERRORWINDOW_H
