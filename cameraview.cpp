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

void CameraView::updateUi()
{
    setUpUi();
    for(int i =0;i<standardLength;i++){
        standard[i].button->setText(QString::number(model->getValue(DISP,standard[i].value)));

    }
    for(int i =0;i<textLength;i++){
        text[i].button->setText(model->getTextValue(text[i].value));
    }

    /*
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
 */
}

void CameraView::setUpUi()
{
    for (int i=0;i<16;i++) {
        text[i].button=ui->btGamma;
    }
    int value;
    value=model->getValue(DISP,V_B_RED);
    standardLength=0;
    textLength=0;
    switch (value) {
    default:standard[standardLength].button=ui->btbred;
            standard[standardLength].value=V_B_RED;
            standardLength++;
            ui->btbred->setEnabled(true);
        break;
    case -2048:ui->btbred->setDisabled(true);
               ui->btbred->setText("-");
        break;
    case -2049:text[textLength].button=ui->btbred;
               text[textLength].value=V_B_RED;
               textLength++;
               ui->btbred->setEnabled(true);
    }

    value=model->getValue(DISP,V_B_BLUE);
    switch (value) {
    default:
            standard[standardLength].button=ui->btbBlue;
            standard[standardLength].value=V_B_BLUE;
            standardLength++;
            ui->btbBlue->setEnabled(true);
        break;
    case -2048:ui->btbBlue->setDisabled(true);
               ui->btbBlue->setText("-");
        break;
    case -2049:
                text[textLength].button=ui->btbBlue;
                text[textLength].value=V_B_BLUE;
                textLength++;
                ui->btbBlue->setEnabled(true);
    }

    value=model->getValue(DISP,V_W_BLUE);
    switch (value) {
    default:standard[standardLength].button=ui->btwBlue;
            standard[standardLength].value=V_W_BLUE;
            standardLength++;
            ui->btwBlue->setEnabled(true);
        break;
    case -2048:ui->btwBlue->setDisabled(true);
               ui->btwBlue->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btwBlue;
               text[textLength].value=V_W_BLUE;
               textLength++;
               ui->btwBlue->setEnabled(true);
    }

    value=model->getValue(DISP,V_W_RED);
    switch (value) {
    default:standard[standardLength].button=ui->btwRed;
            standard[standardLength].value=V_W_RED;
            standardLength++;
            ui->btwRed->setEnabled(true);
        break;
    case -2048:ui->btwRed->setDisabled(true);
               ui->btwRed->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btwRed;
               text[textLength].value=V_W_RED;
               textLength++;
               ui->btwRed->setEnabled(true);
    }

    value=model->getValue(DISP,V_SHUTTER);
    switch (value) {

    default:standard[standardLength].button=ui->btShutter;
            standard[standardLength].value=V_SHUTTER;
            standardLength++;
            ui->btShutter->setEnabled(true);
        break;
    case -2048:ui->btShutter->setDisabled(true);
               ui->btShutter->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btShutter;
               text[textLength].value=V_SHUTTER;
               textLength++;
               ui->btShutter->setEnabled(true);
    }

    value=model->getValue(DISP,V_ND_FILTER);
    switch (value) {

    default:standard[standardLength].button=ui->btndFilter;
            standard[standardLength].value=V_ND_FILTER;
            standardLength++;
            ui->btndFilter->setEnabled(true);
        break;
    case -2048:ui->btndFilter->setDisabled(true);
               ui->btndFilter->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btndFilter;
               text[textLength].value=V_ND_FILTER;
               textLength++;
               ui->btndFilter->setEnabled(true);
    }

    value=model->getValue(DISP,V_GAIN);
    switch (value) {

    default:standard[standardLength].button=ui->btGain;
            standard[standardLength].value=V_GAIN;
            standardLength++;
            ui->btGain->setEnabled(true);
        break;
    case -2048:ui->btGain->setDisabled(true);
               ui->btGain->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btGain;
               text[textLength].value=V_GAIN;
               textLength++;
               ui->btGain->setEnabled(true);
    }

    value=model->getValue(DISP,V_DETAIL);
    switch (value) {

    default:standard[standardLength].button=ui->btDetail;
            standard[standardLength].value=V_DETAIL;
            standardLength++;
            ui->btDetail->setEnabled(true);
        break;
    case -2048:ui->btDetail->setDisabled(true);
               ui->btDetail->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btDetail;
               text[textLength].value=V_DETAIL;
               textLength++;
               ui->btDetail->setEnabled(true);
    }

    value=model->getValue(DISP,V_COLOR);
    switch (value) {

    default:standard[standardLength].button=ui->btSaturation;
            standard[standardLength].value=V_COLOR;
            standardLength++;
            ui->btSaturation->setEnabled(true);
        break;
    case -2048:ui->btSaturation->setDisabled(true);
               ui->btSaturation->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btSaturation;
               text[textLength].value=V_COLOR;
               textLength++;
               ui->btSaturation->setEnabled(true);
    }

    value=model->getValue(DISP,V_COL_TEMP);
    switch (value) {

    default:standard[standardLength].button=ui->btColorTemp;
            standard[standardLength].value=V_COL_TEMP;
            standardLength++;
            ui->btColorTemp->setEnabled(true);
        break;
    case -2048:ui->btColorTemp->setDisabled(true);
               ui->btColorTemp->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btColorTemp;
               text[textLength].value=V_COL_TEMP;
               textLength++;
               ui->btColorTemp->setEnabled(true);
    }
    value=model->getValue(DISP,V_KNEE);
    switch (value) {

    default:standard[standardLength].button=ui->btKnee;
            standard[standardLength].value=V_KNEE;
            standardLength++;
            ui->btKnee->setEnabled(true);
        break;
    case -2048:ui->btKnee->setDisabled(true);
               ui->btKnee->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btKnee;
               text[textLength].value=V_KNEE;
               textLength++;
               ui->btKnee->setEnabled(true);
    }

    value=model->getValue(DISP,V_KNEE_POINT);
    switch (value) {

    default:standard[standardLength].button=ui->btKneePoint;
            standard[standardLength].value=V_KNEE_POINT;
            standardLength++;
            ui->btKneePoint->setEnabled(true);
        break;
    case -2048:ui->btKneePoint->setDisabled(true);
               ui->btKneePoint->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btKneePoint;
               text[textLength].value=V_KNEE_POINT;
               textLength++;
               ui->btKneePoint->setEnabled(true);
    }


    value=model->getValue(DISP,V_GAMMA);
    switch (value) {

    default:standard[standardLength].button=ui->btGamma;
            standard[standardLength].value=V_GAMMA;
            standardLength++;
            ui->btGamma->setEnabled(true);
        break;
    case -2048:ui->btGamma->setDisabled(true);
               ui->btGamma->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btGamma;
               text[textLength].value=V_GAMMA;
               textLength++;
               ui->btGamma->setEnabled(true);
    }

    value=model->getValue(DISP,V_GAMMA_TAB);
    switch (value) {

    default:standard[standardLength].button=ui->btGammaTab;
            standard[standardLength].value=V_GAMMA_TAB;
            standardLength++;
            ui->btGammaTab->setEnabled(true);
        break;
    case -2048:ui->btGammaTab->setDisabled(true);
               ui->btGammaTab->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btGammaTab;
               text[textLength].value=V_GAMMA_TAB;
               textLength++;
               ui->btGammaTab->setEnabled(true);
    }

    value=model->getValue(DISP,V_PED);
    switch (value) {

    default:standard[standardLength].button=ui->btBlackLevel;
            standard[standardLength].value=V_PED;
            standardLength++;
            ui->btBlackLevel->setEnabled(true);
        break;
    case -2048:ui->btBlackLevel->setDisabled(true);
               ui->btBlackLevel->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btBlackLevel;
               text[textLength].value=V_PED;
               textLength++;
               ui->btBlackLevel->setEnabled(true);
    }

    value=model->getValue(DISP,V_IRIS);
    switch (value) {

    default:standard[standardLength].button=ui->btIris;
            standard[standardLength].value=V_IRIS;
            standardLength++;
            ui->btIris->setEnabled(true);
        break;
    case -2048:ui->btIris->setDisabled(true);
               ui->btIris->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btIris;
               text[textLength].value=V_IRIS;
               textLength++;
               ui->btIris->setEnabled(true);
    }

}

void CameraView::setModelController(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
}

void CameraView::on_btGain_clicked()
{
    model->setRotaryField(V_GAIN,SEND);
}

void CameraView::on_btDetail_clicked()
{
    model->setRotaryField(V_DETAIL,SEND);
}

void CameraView::on_btShutter_clicked()
{
    model->setRotaryField(V_SHUTTER,SEND);
}

void CameraView::on_btndFilter_clicked()
{
    model->setRotaryField(V_ND_FILTER,SEND);
}

void CameraView::on_btwRed_clicked()
{
    model->setRotaryField(V_W_RED,SEND);
}

void CameraView::on_btwBlue_clicked()
{
    model->setRotaryField(V_W_BLUE,SEND);
}

void CameraView::on_btbred_clicked()
{
    model->setRotaryField(V_B_RED,SEND);
}

void CameraView::on_btbBlue_clicked()
{
    model->setRotaryField(V_B_BLUE,SEND);
}

void CameraView::on_btSaturation_clicked()
{
    model->setRotaryField(V_COLOR,SEND);
}

void CameraView::on_btColorTemp_clicked()
{
    model->setRotaryField(V_COL_TEMP,SEND);
}

void CameraView::on_btKnee_clicked()
{
    model->setRotaryField(V_KNEE,SEND);
}

void CameraView::on_btKneePoint_clicked()
{
    model->setRotaryField(V_KNEE_POINT,SEND);
}

void CameraView::on_btGamma_clicked()
{
    model->setRotaryField(V_GAMMA,SEND);
}



void CameraView::on_btGammaTab_clicked()
{
    model->setRotaryField(V_GAMMA_TAB,SEND);
}




void CameraView::on_btBlackLevel_clicked()
{
    model->setRotaryField(V_PED,SEND);
}

void CameraView::on_btIris_clicked()
{
    model->setRotaryField(V_IRIS,SEND);
}



void CameraView::stackChanged()
{
    QPushButton *button=ui->btGain;
    QList<QPushButton*> allButtons=this->findChildren<QPushButton*>();
    for(int i=0;i<allButtons.size();i++){
        if(allButtons[i]->isChecked())
        {button=allButtons[i];}

    }
    button->click();
}
