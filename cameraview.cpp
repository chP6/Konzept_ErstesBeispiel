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
    s_UiValue button;
    buttons.clear();
    button.button=ui->btbred;
    button.value=V_B_RED;
    buttons.append(button);

    button.button = ui->btbBlue;
    button.value = V_B_BLUE;
    buttons.append(button);

    button.button = ui->btwRed;
    button.value = V_W_RED;
    buttons.append(button);

    button.button = ui->btwBlue;
    button.value = V_W_BLUE;
    buttons.append(button);

    button.button = ui->btShutter;
    button.value = V_SHUTTER;
    buttons.append(button);

    button.button = ui->btGain;
    button.value = V_GAIN;
    buttons.append(button);

    button.button = ui->btndFilter;
    button.value = V_ND_FILTER;
    buttons.append(button);

    button.button = ui->btDetail;
    button.value = V_DETAIL;
    buttons.append(button);

    button.button = ui->btSaturation;
    button.value = V_COLOR;
    buttons.append(button);

    button.button = ui->btColorTemp;
    button.value = V_COL_TEMP;
    buttons.append(button);

    button.button = ui->btKnee;
    button.value = V_KNEE;
    buttons.append(button);

    button.button = ui->btKneePoint;
    button.value = V_KNEE_POINT;
    buttons.append(button);

    button.button = ui->btGamma;
    button.value = V_GAMMA;
    buttons.append(button);

    button.button = ui->btGammaTab;
    button.value = V_GAMMA_TAB;
    buttons.append(button);

    button.button = ui->btIris;
    button.value = V_IRIS;
    buttons.append(button);

    button.button = ui->btBlackLevel;
    button.value = V_PED;
    buttons.append(button);
/*
    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i].button->setStyleSheet("color: yellow");
    }
*/
}

CameraView::~CameraView()
{
    delete ui;
}

void CameraView::updateUi()
{
    //setUpUi();
    if(!(model->getCamFlag(F_RECEIVED_ALL))){
       // QList<int> remainingTelegram = model->getRemainingTelegrams();
    }

    for(int i =0;i<standardButtons.size();i++){
        standardButtons[i].button->setText(QString::number(model->getValue(DISP,standardButtons[i].value)));
    }
    for(int i =0;i<textButtons.size();i++){
        textButtons[i].button->setText(model->getTextValue(textButtons[i].value));
    }

    /*
    for(int i =0;i<standardButtons.size();i++){
        standardButtons[i].button->setText(QString::number(model->getValue(DISP,standardButtons[i].value)));

    }
    for(int i =0;i<textButtons.size();i++){
        textButtons[i].button->setText(model->getTextValue(textButtons[i].value));
    }*/

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
    int value;
    std::vector<int> requested;
    requested = model->getRemainingTelegrams();
    //textButtons.clear();
    //standardButtons.clear();
    if(!(standardButtons.empty())){
    for (int i = 0; i < buttons.size(); i++) {

        value = model->getValue(DISP,buttons[i].value);

        if(!requested.empty()){
            std::vector<int>::iterator iterator = std::find(requested.begin(),requested.end(),buttons[i].value);

           if(iterator != requested.end() )
            buttons[i].button->setStyleSheet("color: yellow");
            }
        else {
           buttons[i].button->setStyleSheet("color: white");
            }

        switch (value) {
            default:
            if(standardButtons.first().button != buttons[i].button){
                buttons[i].button->setDisabled(false);
                buttons[i].button->setEnabled(true);
                //standardButtons.append(buttons[i]);
                standardButtons.replace(0,buttons[i]);
            }
            standardButtons.move(0,standardButtons.size()-1);
            break;
        case -2048:
                buttons[i].button->setStyleSheet("color: white");
                buttons[i].button->setDisabled(true);
                buttons[i].button->setText("-");
            break;
        case -2049:
                if(textButtons.first().button != buttons[i].button){
                buttons[i].button->setDisabled(false);
                buttons[i].button->setEnabled(true);
                textButtons.replace(0,buttons[i]);
        }
                textButtons.move(0,textButtons.size()-1);
            break;



    }
}}else {
for (int i = 0; i < buttons.size(); i++) {

    value = model->getValue(DISP,buttons[i].value);



    switch (value) {
        default:
            buttons[i].button->setDisabled(false);
            buttons[i].button->setEnabled(true);
            standardButtons.append(buttons[i]);
        break;
    case -2048:
            buttons[i].button->setStyleSheet("color: white");
            buttons[i].button->setDisabled(true);
            buttons[i].button->setText("-");
        break;
    case -2049:
            buttons[i].button->setDisabled(false);
            buttons[i].button->setEnabled(true);
            textButtons.append(buttons[i]);
    break;
    }

}
}


    /*
    for (int i=0;i<16;i++) {
        text[i].button=ui->btGamma;
    }
    //int value;
    //int requested;
    value=model->getValue(DISP,V_B_RED);
    requested=model->getRequestReceived(V_B_RED);
    standardLength=0;
    textLength=0;
    switch (value) {
    default:standard[standardLength].button=ui->btbred;
            standard[standardLength].value=V_B_RED;
            ui->btbred->setDisabled(false);
            standardLength++;
            ui->btbred->setEnabled(true);
        break;
    case -2048:ui->btbred->setDisabled(true);
               ui->btbred->setText("-");
        break;
    case -2049:text[textLength].button=ui->btbred;
               text[textLength].value=V_B_RED;
               ui->btbred->setDisabled(false);
               textLength++;
               ui->btbred->setEnabled(true);
    }
    if(requested){
        ui->btbred->setText("HALLOO");
    }

    value=model->getValue(DISP,V_B_BLUE);
    switch (value) {
    default:
            standard[standardLength].button=ui->btbBlue;
            standard[standardLength].value=V_B_BLUE;
            ui->btbBlue->setDisabled(false);
            standardLength++;
            ui->btbBlue->setEnabled(true);
        break;
    case -2048:ui->btbBlue->setDisabled(true);
               ui->btbBlue->setText("-");
        break;
    case -2049:
                text[textLength].button=ui->btbBlue;
                text[textLength].value=V_B_BLUE;
                ui->btbBlue->setDisabled(false);
                textLength++;
                ui->btbBlue->setEnabled(true);
    }

    value=model->getValue(DISP,V_W_BLUE);

    switch (value) {
    default:standard[standardLength].button=ui->btwBlue;
            standard[standardLength].value=V_W_BLUE;
            ui->btwBlue->setDisabled(false);
            standardLength++;
            ui->btwBlue->setEnabled(true);
        break;
    case -2048:ui->btwBlue->setDisabled(true);
               ui->btwBlue->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btwBlue;
               text[textLength].value=V_W_BLUE;
               ui->btwBlue->setDisabled(false);
               textLength++;
               ui->btwBlue->setEnabled(true);
    }

    value=model->getValue(DISP,V_W_RED);
    requested=model->getRequestReceived(V_W_RED);
    switch (value) {
    default:standard[standardLength].button=ui->btwRed;
            standard[standardLength].value=V_W_RED;
            ui->btbred->setDisabled(false);
            standardLength++;
            ui->btwRed->setEnabled(true);
        break;
    case -2048:ui->btwRed->setDisabled(true);
               ui->btwRed->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btwRed;
               text[textLength].value=V_W_RED;
               ui->btwRed->setDisabled(false);
               textLength++;
               ui->btwRed->setEnabled(true);
    }
    if(!requested){
        ui->btwRed->setText("HALLOO");
    }


    value=model->getValue(DISP,V_SHUTTER);
    switch (value) {

    default:standard[standardLength].button=ui->btShutter;
            standard[standardLength].value=V_SHUTTER;
            ui->btShutter->setDisabled(false);
            standardLength++;
            ui->btShutter->setEnabled(true);
        break;
    case -2048:ui->btShutter->setDisabled(true);
               ui->btShutter->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btShutter;
               text[textLength].value=V_SHUTTER;
               ui->btShutter->setDisabled(false);
               textLength++;
               ui->btShutter->setEnabled(true);
    }

    value=model->getValue(DISP,V_ND_FILTER);
    switch (value) {

    default:standard[standardLength].button=ui->btndFilter;
            standard[standardLength].value=V_ND_FILTER;
            ui->btndFilter->setDisabled(false);
            standardLength++;
            ui->btndFilter->setEnabled(true);
        break;
    case -2048:ui->btndFilter->setDisabled(true);
               ui->btndFilter->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btndFilter;
               text[textLength].value=V_ND_FILTER;
               ui->btndFilter->setDisabled(false);
               textLength++;
               ui->btndFilter->setEnabled(true);
    }

    value=model->getValue(DISP,V_GAIN);
    switch (value) {

    default:standard[standardLength].button=ui->btGain;
            standard[standardLength].value=V_GAIN;
            ui->btGain->setDisabled(false);
            standardLength++;
            ui->btGain->setEnabled(true);
        break;
    case -2048:ui->btGain->setDisabled(true);
               ui->btGain->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btGain;
               text[textLength].value=V_GAIN;
               ui->btGain->setDisabled(false);
               textLength++;
               ui->btGain->setEnabled(true);
    }

    value=model->getValue(DISP,V_DETAIL);
    switch (value) {

    default:standard[standardLength].button=ui->btDetail;
            standard[standardLength].value=V_DETAIL;
            ui->btDetail->setDisabled(false);
            standardLength++;
            ui->btDetail->setEnabled(true);
        break;
    case -2048:ui->btDetail->setDisabled(true);
               ui->btDetail->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btDetail;
               text[textLength].value=V_DETAIL;
               ui->btDetail->setDisabled(false);
               textLength++;
               ui->btDetail->setEnabled(true);
    }

    value=model->getValue(DISP,V_COLOR);
    switch (value) {

    default:standard[standardLength].button=ui->btSaturation;
            standard[standardLength].value=V_COLOR;
            ui->btSaturation->setDisabled(false);
            standardLength++;
            ui->btSaturation->setEnabled(true);
        break;
    case -2048:ui->btSaturation->setDisabled(true);
               ui->btSaturation->setText("-");

        break;
    case -2049:
               text[textLength].button=ui->btSaturation;
               text[textLength].value=V_COLOR;
               ui->btSaturation->setDisabled(false);
               textLength++;
               ui->btSaturation->setEnabled(true);
    }

    value=model->getValue(DISP,V_COL_TEMP);
    switch (value) {

    default:standard[standardLength].button=ui->btColorTemp;
            standard[standardLength].value=V_COL_TEMP;
            ui->btColorTemp->setDisabled(false);
            standardLength++;
            ui->btColorTemp->setEnabled(true);
        break;
    case -2048:ui->btColorTemp->setDisabled(true);
               ui->btColorTemp->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btColorTemp;
               text[textLength].value=V_COL_TEMP;
               ui->btColorTemp->setDisabled(false);
               textLength++;
               ui->btColorTemp->setEnabled(true);
    }
    value=model->getValue(DISP,V_KNEE);
    switch (value) {

    default:standard[standardLength].button=ui->btKnee;
            standard[standardLength].value=V_KNEE;
            ui->btKnee->setDisabled(false);
            standardLength++;
            ui->btKnee->setEnabled(true);
        break;
    case -2048:ui->btKnee->setDisabled(true);
               ui->btKnee->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btKnee;
               text[textLength].value=V_KNEE;
               ui->btKnee->setDisabled(false);
               textLength++;
               ui->btKnee->setEnabled(true);
    }

    value=model->getValue(DISP,V_KNEE_POINT);
    switch (value) {

    default:standard[standardLength].button=ui->btKneePoint;
            standard[standardLength].value=V_KNEE_POINT;
            ui->btKneePoint->setDisabled(false);
            standardLength++;
            ui->btKneePoint->setEnabled(true);
        break;
    case -2048:ui->btKneePoint->setDisabled(true);
               ui->btKneePoint->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btKneePoint;
               text[textLength].value=V_KNEE_POINT;
               ui->btKneePoint->setDisabled(false);
               textLength++;
               ui->btKneePoint->setEnabled(true);
    }


    value=model->getValue(DISP,V_GAMMA);
    switch (value) {

    default:standard[standardLength].button=ui->btGamma;
            standard[standardLength].value=V_GAMMA;
            ui->btGamma->setDisabled(false);
            standardLength++;
            ui->btGamma->setEnabled(true);
        break;
    case -2048:ui->btGamma->setDisabled(true);
               ui->btGamma->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btGamma;
               text[textLength].value=V_GAMMA;
               ui->btGamma->setDisabled(false);
               textLength++;
               ui->btGamma->setEnabled(true);
    }

    value=model->getValue(DISP,V_GAMMA_TAB);
    switch (value) {

    default:standard[standardLength].button=ui->btGammaTab;
            standard[standardLength].value=V_GAMMA_TAB;
            ui->btGammaTab->setDisabled(false);
            standardLength++;
            ui->btGammaTab->setEnabled(true);
        break;
    case -2048:ui->btGammaTab->setDisabled(true);
               ui->btGammaTab->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btGammaTab;
               text[textLength].value=V_GAMMA_TAB;
               ui->btGammaTab->setDisabled(false);
               textLength++;
               ui->btGammaTab->setEnabled(true);
    }

    value=model->getValue(DISP,V_PED);
    switch (value) {

    default:standard[standardLength].button=ui->btBlackLevel;
            standard[standardLength].value=V_PED;
            ui->btBlackLevel->setDisabled(false);
            standardLength++;
            ui->btBlackLevel->setEnabled(true);
        break;
    case -2048:ui->btBlackLevel->setDisabled(true);
               ui->btBlackLevel->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btBlackLevel;
               text[textLength].value=V_PED;
               ui->btBlackLevel->setDisabled(false);
               textLength++;
               ui->btBlackLevel->setEnabled(true);
    }

    value=model->getValue(DISP,V_IRIS);
    switch (value) {

    default:standard[standardLength].button=ui->btIris;
            standard[standardLength].value=V_IRIS;
            ui->btIris->setDisabled(false);
            standardLength++;
            ui->btIris->setEnabled(true);
        break;
    case -2048:ui->btIris->setDisabled(true);
               ui->btIris->setText("-");
        break;
    case -2049:
               text[textLength].button=ui->btIris;
               text[textLength].value=V_IRIS;
               ui->btIris->setDisabled(false);
               textLength++;
               ui->btIris->setEnabled(true);
    }*/
    /*
    standardLength=0;
    textLength=0;
    for (int i=0;i<buttons.size();i++) {
        value=model->getValue(DISP,buttons[i].value);
        requested = model->getRequestReceived(buttons[i].value);

        if(requested){
            //buttons[i].button->setFocus();
            }
        else {
            //buttons[i].button->setStyleSheet("border-color: yellow");
            }
        switch (value) {

        default:standard[standardLength].button=buttons[i].button;
                standard[standardLength].value=buttons[i].value;
                buttons[i].button->setDisabled(false);
                standardLength++;
                buttons[i].button->setEnabled(true);
            break;
        case -2048:buttons[i].button->setDisabled(true);
                    //buttons[i].button->setStyleSheet("color: white");
                   buttons[i].button->setText("-");
            break;
        case -2049:
                   text[textLength].button=buttons[i].button;
                   text[textLength].value=buttons[i].value;
                   buttons[i].button->setDisabled(false);
                   textLength++;
                   buttons[i].button->setEnabled(true);
        }
    }*/

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

void CameraView::signalRequest(int property)
{
    for (int i = 0;i < textButtons.size(); i++) {
        if(textButtons[i].value == property){
            textButtons[i].button->setStyleSheet("color: white");
        }
    }

    for (int i = 0;i < standardButtons.size(); i++) {
        if(standardButtons[i].value == property){
            standardButtons[i].button->setStyleSheet("color: white");
        }
    }
}

void CameraView::newRequest()
{
    for (int i = 0;i < textButtons.size(); i++) {

            textButtons[i].button->setStyleSheet("color: yellow");

    }

    for (int i = 0;i < standardButtons.size(); i++) {

            standardButtons[i].button->setStyleSheet("color: yellow");

    }
}
