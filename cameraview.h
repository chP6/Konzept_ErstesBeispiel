#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QWidget>

class Model;
class Controller;
namespace Ui {
class CameraView;
}

class CameraView : public QWidget
{
    Q_OBJECT

public:
    void update();
    void setUpUi();
    explicit CameraView(QWidget *parent = nullptr);
    void setModelController(Model* model, Controller* controller);
    ~CameraView();

private slots:
    void on_btGain_clicked();

    void on_btDetail_clicked();

    void on_btShutter_clicked();

    void on_btndFilter_clicked();

    void on_btwRed_clicked();

    void on_btwBlue_clicked();

    void on_btbred_clicked();

    void on_btbBlue_clicked();

    void on_btSaturation_clicked();

    void on_btColorTemp_clicked();

    void on_btKnee_clicked();

    void on_btKneePoint_clicked();

    void on_btGamma_clicked();



    void on_btGammaTab_clicked();

private:
    Ui::CameraView *ui;
    Model* model;
    Controller* controller;
};

#endif // CAMERAVIEW_H
