#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
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

public:
    explicit View(QWidget *parent = nullptr);
    ~View();
    void setModelController(Model& model, Controller& controller);

private:
    Ui::View *ui;
    Controller* controller;
    Model* model;
};

#endif // VIEW_H
