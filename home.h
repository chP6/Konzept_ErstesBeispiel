#ifndef HOME_H
#define HOME_H

#include <QWidget>

namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();

private:
    Ui::Home *ui;

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
