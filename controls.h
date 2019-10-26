#ifndef CONTROLS_H
#define CONTROLS_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QWidget>
#include "errorwindow.h"
#include "config.h"

class Model;
class Controller;

namespace Ui {
class Controls;
}

class ControlsAction : public QAction {
    Q_OBJECT
public:
    ControlsAction(control_t control, QObject *parent = nullptr);
    virtual ~ControlsAction() {}
    void onTriggered(void);

protected:
    control_t m_control;

signals:
    void controlSelected(control_t control);
};

class ControlsMenu : public QMenu {
    Q_OBJECT
public:
    ControlsMenu(axis_t axis, QWidget *parent = nullptr);
    virtual ~ControlsMenu() {}

protected:
    axis_t m_axis;

private slots:
    void setControl(control_t control);

signals:
    void controlSelected(axis_t axis, control_t control);
};

class Controls : public QWidget
{
    Q_OBJECT

public:
    explicit Controls(QWidget *parent = nullptr);
    ~Controls();
    void stackChanged();
    void setModelController(Model* model, Controller* controller);
    void updateUi();

private slots:
    void setControl(axis_t axis, control_t control);
    void on_btPanInvert_clicked(bool checked);
    void on_btTiltInvert_clicked(bool checked);
    void on_btZoomInvert_clicked(bool checked);
    void on_btFocusInvert_clicked(bool checked);
    void on_btTravellingInvert_clicked(bool checked);

private:
    Ui::Controls *ui;
    Model* model;
    Controller* controller;
    ErrorWindow* errorWindow;

    ControlsMenu* controlMenus[kAxisMax];
};

#endif // CONTROLS_H
