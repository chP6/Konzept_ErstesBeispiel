#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include "errorwindow.h"
class Controller;       //forward declaration
class Model;

namespace Ui {
class View;
}

class View : public QWidget
{
    Q_OBJECT

public slots:
    void on_modelUpdate();
private slots:
    void on_pbInc_clicked();
    void on_pbClear_clicked();
    void on_pbErrors_clicked();

    void on_storePreset_clicked();

public:
    explicit View(QWidget *parent = nullptr);
    ~View();
    void setModelController(Model& model, Controller& controller);

private:
    Ui::View *ui;
    Controller* controller;
    Model* model;
    ErrorWindow errorFenster;
};

#endif // VIEW_H
