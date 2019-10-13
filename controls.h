#ifndef CONTROLS_H
#define CONTROLS_H

#include "model.h"
#include "controller.h"
#include "events.h"
#include <QObject>
#include <QColor>
#include <QDebug>
#include <QMetaMethod>

class Controls : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool invertPan READ invertPan WRITE setInvertPan NOTIFY invertPanChanged)
    Q_PROPERTY(bool invertTilt READ invertTilt WRITE setInvertTilt NOTIFY invertTiltChanged)
    Q_PROPERTY(bool invertZoom READ invertZoom WRITE setInvertZoom NOTIFY invertZoomChanged)
    Q_PROPERTY(bool invertFocus READ invertFocus WRITE setInvertFocus NOTIFY invertFocusChanged)
    Q_PROPERTY(bool invertTravelling READ invertTravelling WRITE setInvertTravelling NOTIFY invertTravellingChanged)
    Q_PROPERTY(QList<int> control READ control NOTIFY controlChanged)
public:
    Controls();
    Controls(Model* model, Controller* controller);
    ~Controls();

    void updateAll();
    bool invertPan();
    bool invertTilt();
    bool invertZoom();
    bool invertFocus();
    bool invertTravelling();
    Q_INVOKABLE void setControl(int axis, int control);
    Q_INVOKABLE int getControl(int axis);

    QList<int> control();

public slots:

    void setInvertPan(bool invertPan);
    void setInvertTilt(bool invertTilt);
    void setInvertZoom(bool invertZoom);
    void setInvertFocus(bool invertFocus);
    void setInvertTravelling(bool invertTravelling);


signals:

    void invertPanChanged();
    void invertTiltChanged();
    void invertZoomChanged();
    void invertFocusChanged();
    void invertTravellingChanged();
    void controlChanged();

private:
    Model* model;
    Controller* controller;

};
////#include <QAction>
////#include <QMenu>
//#include <QObject>
////#include <QWidget>
//#include "errorwindow.h"
//#include "config.h"

//class Model;
//class Controller;

//namespace Ui {
//class Controls;
//}

//class ControlsAction : public QAction {
//    Q_OBJECT
//public:
//    ControlsAction(control_t control, QObject *parent = nullptr);
//    virtual ~ControlsAction() {}
//    void onTriggered(void);

//protected:
//    control_t m_control;

//signals:
//    void controlSelected(control_t control);
//};

//class ControlsMenu : public QMenu {
//    Q_OBJECT
//public:
//    ControlsMenu(axis_t axis, QWidget *parent = nullptr);
//    virtual ~ControlsMenu() {}

//protected:
//    axis_t m_axis;

//private slots:
//    void setControl(control_t control);

//signals:
//    void controlSelected(axis_t axis, control_t control);
//};

//class Controls : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit Controls(QWidget *parent = nullptr);
//    ~Controls();
//    void stackChanged();
//    void setModelController(Model* model, Controller* controller);
//    void updateUi();

//private slots:
//    void setControl(axis_t axis, control_t control);
//    void on_btPanInvert_clicked(bool checked);
//    void on_btTiltInvert_clicked(bool checked);
//    void on_btZoomInvert_clicked(bool checked);
//    void on_btFocusInvert_clicked(bool checked);
//    void on_btTravellingInvert_clicked(bool checked);

//private:
//    Ui::Controls *ui;
//    Model* model;
//    Controller* controller;
//    ErrorWindow* errorWindow;

//    ControlsMenu* controlMenus[kAxisMax];
//};

#endif // CONTROLS_H
