#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QWidget>
#include <QPushButton>
struct s_UiValue{QPushButton *button; int value;};
class Model;
class Controller;
namespace Ui {
class CameraView;
}

class CameraView : public QWidget
{
    Q_OBJECT

public:
    void updateUi();
    void setUpUi();
    explicit CameraView(QWidget *parent = nullptr);
    void setModelController(Model* model, Controller* controller);
    void stackChanged();
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

    void on_btBlackLevel_clicked();

    void on_btIris_clicked();

private:
    Ui::CameraView *ui;
    Model* model;
    Controller* controller;
    struct s_UiValue standard[16];
    struct s_UiValue text[16];
    int standardLength=0;
    int textLength=0;

};

#endif // CAMERAVIEW_H
