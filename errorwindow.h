#ifndef ERRORWINDOW_H
#define ERRORWINDOW_H

#include <QWidget>
#include <QStringListModel>

class Model;
class Controller;

namespace Ui {
class ErrorWindow;
}

class ErrorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorWindow(QWidget *parent = nullptr);
    ~ErrorWindow();
    void setModelController(Model* model, Controller* controller);
    void update();

private slots:
    void on_pbEwClose_clicked();
    void on_pbEwClear_clicked();

private:
    Ui::ErrorWindow *ui;
    QStringListModel *listModel;
    Model* model;
    Controller* controller;
};

#endif // ERRORWINDOW_H
