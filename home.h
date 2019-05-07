#ifndef HOME_H
#define HOME_H

#include <QWidget>
class Model;
class Controller;


namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();
    void updateUi();
     void setModelController(Model* model, Controller* controller);
     void serverConnectionChanged(bool connection);
     void stackChanged();
private:
    Ui::Home *ui;
    Model* model;
    Controller* controller;

private slots:


    void on_btBounce_clicked(bool checked);

    void on_btWideSet_clicked();

    void on_btAutoZoomSpeed_clicked();

    void on_btTransSpeed_clicked();

    void on_btPanTiltSpeed_clicked();

    void on_btRamp_clicked();

    void on_btFasttrans_clicked(bool checked);

    void on_btSpp1_clicked();

    void on_btSpp2_clicked();

    void on_btSppWait_clicked();

    void on_btSppStart_clicked(bool checked);

    void on_btHeadNr_clicked();

    void on_btStorePreset_clicked();


};

#endif // HOME_H
