#include "cameraview.h"
#include "ui_cameraview.h"
#include <config.h>
#include "model.h"
#include "controller.h"
#include "events.h"

CameraView::CameraView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraView)
{
    ui->setupUi(this);
}

CameraView::~CameraView()
{
    delete ui;
}

void CameraView::update()
{
 ui->btbred->setText(QString::number(model->getValue(DISP,V_B_RED)));
 ui->btbBlue->setText(QString::number(model->getValue(DISP,V_B_BLUE)));
 ui->btwRed->setText(QString::number(model->getValue(DISP,V_W_RED)));
 ui->btwBlue->setText(QString::number(model->getValue(DISP,V_W_BLUE)));
 ui->btShutter->setText(model->getTextValue(V_SHUTTER));
 ui->btndFilter->setText(QString::number(model->getValue(DISP,V_ND_FILTER)));
 ui->btGain->setText(model->getTextValue(V_GAIN));
 ui->btDetail->setText(QString::number(model->getValue(DISP,V_DETAIL)));
 ui->btSaturation->setText(QString::number(model->getValue(DISP,V_COLOR)));
 ui->btColorTemp->setText(model->getTextValue(V_COL_TEMP));
 ui->btKnee->setText(QString::number(model->getValue(DISP,V_KNEE)));
 ui->btKneePoint->setText(QString::number(model->getValue(DISP,V_KNEE_POINT)));
 ui->btGamma->setText(QString::number(model->getValue(DISP,V_GAMMA)));
 ui->btGammaTab->setText(model->getTextValue(V_GAMMA_TAB));
}

void CameraView::setUpUi()
{
    int value;
    value=model->getValue(DISP,V_B_RED);
    if (value==-2048) {
        ui->btbred->setDisabled(true);
        ui->btbred->setText("-");
    }

    value=model->getValue(DISP,V_B_BLUE);
    if (value==-2048) {
        ui->btbBlue->setDisabled(true);
        ui->btbBlue->setText("-");
    }

    value=model->getValue(DISP,V_W_RED);
    if (value==-2048) {
        ui->btwRed->setDisabled(true);
        ui->btwRed->setText("-");
    }

    value=model->getValue(DISP,V_W_BLUE);
    if (value==-2048) {
        ui->btwBlue->setDisabled(true);
        ui->btwBlue->setText("-");
    }

    value=model->getValue(DISP,V_SHUTTER);
    if (value==-2048) {
        ui->btShutter->setDisabled(true);
        ui->btShutter->setText("-");
    }

    value=model->getValue(DISP,V_ND_FILTER);
    if (value==-2048) {
        ui->btndFilter->setDisabled(true);
        ui->btndFilter->setText("-");
    }

    value=model->getValue(DISP,V_GAIN);
    if (value==-2048) {
        ui->btGain->setDisabled(true);
        ui->btGain->setText("-");
    }

    value=model->getValue(DISP,V_DETAIL);
    if (value==-2048) {
        ui->btDetail->setDisabled(true);
        ui->btDetail->setText("-");
    }

    value=model->getValue(DISP,V_COLOR);
    if (value==-2048) {
        ui->btSaturation->setDisabled(true);
        ui->btSaturation->setText("-");
    }

    value=model->getValue(DISP,V_COL_TEMP);
    if (value==-2048) {
        ui->btColorTemp->setDisabled(true);
        ui->btColorTemp->setText("-");
    }

    value=model->getValue(DISP,V_KNEE);
    if (value==-2048) {
        ui->btKnee->setDisabled(true);
        ui->btKnee->setText("-");
    }

    value=model->getValue(DISP,V_KNEE_POINT);
    if (value==-2048) {
        ui->btKneePoint->setDisabled(true);
        ui->btKneePoint->setText("-");
    }

    value=model->getValue(DISP,V_GAMMA);
    if (value==-2048) {
        ui->btGamma->setDisabled(true);
        ui->btGamma->setText("-");
    }

    value=model->getValue(DISP,V_GAMMA_TAB);
    if (value==-2048) {
        ui->btGammaTab->setDisabled(true);
        ui->btGammaTab->setText("-");
    }

}

void CameraView::setModelController(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
}

void CameraView::on_btGain_clicked()
{
    model->setRotaryField(V_GAIN);
}

void CameraView::on_btDetail_clicked()
{
    model->setRotaryField(V_DETAIL);
}

void CameraView::on_btShutter_clicked()
{
    model->setRotaryField(V_SHUTTER);
}

void CameraView::on_btndFilter_clicked()
{
    model->setRotaryField(V_ND_FILTER);
}

void CameraView::on_btwRed_clicked()
{
    model->setRotaryField(V_W_RED);
}

void CameraView::on_btwBlue_clicked()
{
    model->setRotaryField(V_W_BLUE);
}

void CameraView::on_btbred_clicked()
{
    model->setRotaryField(V_B_RED);
}

void CameraView::on_btbBlue_clicked()
{
    model->setRotaryField(V_B_BLUE);
}

void CameraView::on_btSaturation_clicked()
{
    model->setRotaryField(V_COLOR);
}

void CameraView::on_btColorTemp_clicked()
{
    model->setRotaryField(V_COL_TEMP);
}

void CameraView::on_btKnee_clicked()
{
    model->setRotaryField(V_KNEE);
}

void CameraView::on_btKneePoint_clicked()
{
    model->setRotaryField(V_KNEE_POINT);
}

void CameraView::on_btGamma_clicked()
{
    model->setRotaryField(V_GAMMA);
}



void CameraView::on_btGammaTab_clicked()
{
    model->setRotaryField(V_GAMMA_TAB);
}
