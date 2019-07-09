#ifndef XPTCONTROL_H
#define XPTCONTROL_H

#include <QWidget>
#include <QMenu>

class Model;
class Controller;

namespace Ui {
class XptControl;
}

class XptControl : public QWidget
{
    Q_OBJECT

public:
    explicit XptControl(QWidget *parent = nullptr);
    void setModelController(Model* model, Controller* controller);
     void stackChanged();
     void updateUi();
     void xptStatusChanged(bool connected);
     void xptEnableChanged(bool connected);
    ~XptControl();

private slots:
    void on_btSource1_clicked();

    void on_btSource2_clicked();

    void on_btSource3_clicked();

    void on_btSource4_clicked();

    void on_btSource5_clicked();

    void on_btSource6_clicked();

    void on_btXPTIp_1_clicked();

    void on_btXPTIp_2_clicked();

    void on_btXPTIp_3_clicked();

    void on_btXPTIp_4_clicked();

    void on_btDestination_clicked();

    void on_btConnect_clicked(bool checked);

    void on_blackMagic_triggered();
    void on_ross_triggered();

private:
    Ui::XptControl *ui;
    Model* model;
    Controller* controller;
    QMenu *menu;
    QAction *blackMagic;
    QAction *ross;
};

#endif // XPTCONTROL_H
