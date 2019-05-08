#ifndef OTHERS_H
#define OTHERS_H

#include <QWidget>
#include "errorwindow.h"
class Model;
class Controller;


namespace Ui {
class Others;
}

class Others : public QWidget
{
    Q_OBJECT

public:
    explicit Others(QWidget *parent = nullptr);
    void stackChanged();
    void setModelController(Model* model, Controller* controller);
    void updateUi();
    ~Others();

private slots:
    void on_btPanInvert_clicked(bool checked);

    void on_btTiltInvert_clicked(bool checked);

    void on_btZoomInvert_clicked(bool checked);

    void on_btFocusInvert_clicked(bool checked);

    void on_btMirror_clicked();

    void on_btUpperLimit_clicked();

    void on_btLowerLimit_clicked();

    void on_btCalibrate_clicked();

    void on_btHeadPower_clicked();

    void on_btRequest_clicked();

    void on_btSave_clicked();

    void on_btLoad_clicked();

    void on_btErrorWindow_clicked();

    void on_btClearLimit_clicked();

private:
    Ui::Others *ui;
    Model* model;
    Controller* controller;
    ErrorWindow* errorWindow;
};

#endif // OTHERS_H
