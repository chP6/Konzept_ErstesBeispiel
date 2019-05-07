#ifndef VIE_H
#define VIE_H

#include <QWidget>
#include <QMainWindow>
#include "errorwindow.h"

namespace Ui {
class View;
}

class View : public QMainWindow
{
    Q_OBJECT

public slots:
    void on_modelUpdate();
    void on_serverConnectionStatusChanged(bool connected);

private slots:



    void on_btHome_clicked();

    void on_btCamCtrl_clicked();

    void on_btXptControl_clicked();

    void on_btOthers_clicked();

public:
    explicit View(QWidget *parent = nullptr);
    ~View();
    void setModelController(Model& model, Controller& controller);

private:
    Ui::View *ui;
    Controller* controller;
    Model* model;
};

#endif // VIE_H
