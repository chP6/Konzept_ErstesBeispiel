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
    void on_modelSetUp();
    void on_sppUpdate(bool active);
    void on_serverConnectionStatusChanged(bool connected);
    void on_cameraConnectionStatusChanged(bool connected);
    void on_xptConnectionStatusChanged(bool connected);
    void on_xptEnableStatusChanged(bool connected);
    void on_loadButtonCleared();
    void on_newReceive(int property);
    void on_newRequest();

private slots:
    void on_btHome_clicked();
    void on_btCamCtrl_clicked();
    void on_btXptControl_clicked();
    void on_btOthers_clicked();
    void on_btControls_clicked();

signals:
    void stackChanged();

public:
    explicit View(QWidget *parent = nullptr);
    ~View();
    void setModelController(Model& model, Controller& controller);

private:
    Ui::View *ui;
    Controller* controller;
    Model* model;
    static void signalHandler(int signum);
};

#endif // VIE_H
