
#include "controls.h"
//#include "ui_controls.h"
//#include <QDebug>
//#include <QString>
//#include <list>
//#include "config.h"
//#include "model.h"
//#include "controller.h"
//#include "events.h"

//ControlsAction::ControlsAction(control_t control, QObject *parent)
//        : QAction(controlStr(control), parent), m_control(control) {
//    QObject::connect(this, &ControlsAction::triggered, this, &ControlsAction::onTriggered);
//}

//void ControlsAction::onTriggered(void) {
//    emit controlSelected(m_control);
//}

//ControlsMenu::ControlsMenu(axis_t axis, QWidget *parent)
//    : QMenu(parent), m_axis(axis) {

//    std::list<QAction*> actions;
//    actions.push_back(new ControlsAction(kControlJoystickX, this));
//    actions.push_back(new ControlsAction(kControlJoystickY, this));
//    actions.push_back(new ControlsAction(kControlJoystickZ, this));
//    actions.push_back(new ControlsAction(kControlZoomRocker, this));
//    actions.push_back(new ControlsAction(kControlFocusWheel, this));
//    actions.push_back(new ControlsAction(kControlNone, this));

//    for (QAction *action : actions) {
//        addAction(action);
//        connect(action, SIGNAL(controlSelected(control_t)), this, SLOT(setControl(control_t)));
//    }
//}

//void ControlsMenu::setControl(control_t control) {
//    emit controlSelected(m_axis, control);
//}

//Controls::Controls(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::Controls)
//{
//    ui->setupUi(this);

//    controlMenus[kAxisPan] = new ControlsMenu(kAxisPan, this);
//    controlMenus[kAxisTilt] = new ControlsMenu(kAxisTilt, this);
//    controlMenus[kAxisZoom] = new ControlsMenu(kAxisZoom, this);
//    controlMenus[kAxisFocus] = new ControlsMenu(kAxisFocus, this);
//    controlMenus[kAxisTravelling] = new ControlsMenu(kAxisTravelling, this);

//    ui->tbPanAxis->setMenu(controlMenus[kAxisPan]);
//    ui->tbTiltAxis->setMenu(controlMenus[kAxisTilt]);
//    ui->tbZoomAxis->setMenu(controlMenus[kAxisZoom]);
//    ui->tbFocusAxis->setMenu(controlMenus[kAxisFocus]);
//    ui->tbTravellingAxis->setMenu(controlMenus[kAxisTravelling]);

//    QList<QPushButton*> allButtons = this->findChildren<QPushButton*>();

//    for (axis_t axis : { kAxisPan, kAxisTilt, kAxisZoom, kAxisFocus, kAxisTravelling })
//        connect(controlMenus[axis], SIGNAL(controlSelected(axis_t, control_t)), this, SLOT(setControl(axis_t, control_t)));
//}

//Controls::~Controls()
//{
//    for (axis_t axis : { kAxisPan, kAxisTilt, kAxisZoom, kAxisFocus, kAxisTravelling })
//        delete controlMenus[axis];
//    delete ui;
//}

//void Controls::setModelController(Model *model, Controller *controller)
//{
//    this->model = model;
//    this->controller = controller;
//}

//void Controls::stackChanged()
//{
//    QList<QPushButton*> allButtons = this->findChildren<QPushButton*>();
//    for (int i = 0; i < allButtons.size(); i++) {
//        if (allButtons[i]->isChecked() && allButtons[i]->autoExclusive())
//            allButtons[i]->click();
//    }
//    updateUi();
//}

//void Controls::updateUi()
//{
//    ui->btPanInvert->setChecked(model->getCamFlag(F_PAN_INVERT));
//    ui->btTiltInvert->setChecked(model->getCamFlag(F_TILT_INVERT));
//    ui->btZoomInvert->setChecked(model->getCamFlag(F_ZOOM_INVERT));
//    ui->btFocusInvert->setChecked(model->getCamFlag(F_FOCUS_INVERT));
//    ui->btTravellingInvert->setChecked(model->getCamFlag(F_TRAVELLING_INVERT));

//    ui->tbPanAxis->setText(controlStr(model->getControl(kAxisPan)));
//    ui->tbTiltAxis->setText(controlStr(model->getControl(kAxisTilt)));
//    ui->tbZoomAxis->setText(controlStr(model->getControl(kAxisZoom)));
//    ui->tbFocusAxis->setText(controlStr(model->getControl(kAxisFocus)));
//    ui->tbTravellingAxis->setText(controlStr(model->getControl(kAxisTravelling)));
//}

//void Controls::setControl(axis_t axis, control_t control)
//{
//    model->setControl(axis, control);
//    updateUi();
//}

//void Controls::on_btPanInvert_clicked(bool checked)
//{
//    model->setCamFlag(F_PAN_INVERT, checked);
//}

//void Controls::on_btTiltInvert_clicked(bool checked)
//{
//    model->setCamFlag(F_TILT_INVERT, checked);
//}

//void Controls::on_btZoomInvert_clicked(bool checked)
//{
//    model->setCamFlag(F_ZOOM_INVERT, checked);
//}

//void Controls::on_btFocusInvert_clicked(bool checked)
//{
//    model->setCamFlag(F_FOCUS_INVERT, checked);
//}

//void Controls::on_btTravellingInvert_clicked(bool checked)
//{
//    model->setCamFlag(F_TRAVELLING_INVERT, checked);
//}

Controls::Controls()
{

}

Controls::Controls(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
}

Controls::~Controls()
{

}

void Controls::updateAll()
{
    QMetaMethod method;
    QMetaObject meta = *this->metaObject();
    for(int i = meta.methodOffset(); i< meta.methodCount(); i++){
      method = meta.method(i);
     QMetaMethod::MethodType type =  method.methodType();
     if(type == QMetaMethod::Signal){
          method.invoke(this);
     }
    }
}

bool Controls::invertPan()
{
   return model->getCamFlag(PanInverted);
}

bool Controls::invertTilt()
{
    return model->getCamFlag(TiltInverted);
}

bool Controls::invertZoom()
{
    return model->getCamFlag(ZoomInverted);
}

bool Controls::invertFocus()
{
    return model->getCamFlag(FocusInverted);
}

bool Controls::invertTravelling()
{
    return  model->getCamFlag(TravellingInverted);
}

void Controls::setControl(int axis, int control)
{
    model->setControl(axis_t(axis),control_t(control));
}

int Controls::getControl(int axis)
{
  int control = model->getControl(axis_t(axis));
  return  control;
}

QList<int> Controls::control()
{
    QList<int> list;
    for (axis_t axes : {KAxisPan, KAxisTilt, KAxisZoom, KAxisFocus, KAxisTravelling}) {
        list.append(model->getControl(axes));
    }
    return list;
}


void Controls::setInvertPan(bool invertPan)
{
    model->setCamFlag(PanInverted,invertPan);
}

void Controls::setInvertTilt(bool invertTilt)
{
    model->setCamFlag(TiltInverted, invertTilt);
}

void Controls::setInvertZoom(bool invertZoom)
{
    model->setCamFlag(ZoomInverted, invertZoom);
}

void Controls::setInvertFocus(bool invertFocus)
{
    model->setCamFlag(FocusInverted, invertFocus);
}

void Controls::setInvertTravelling(bool invertTravelling)
{
    model->setCamFlag(TravellingInverted, invertTravelling);
}


