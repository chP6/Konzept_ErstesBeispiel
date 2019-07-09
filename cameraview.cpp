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
    s_UiValue button;   //struct s_UiValue{QPushButton *button; int value;};

    /*Assign each button to is corresponding value and store it in a List -> QList<s_UiValue> */
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
}

CameraView::~CameraView()
{
    delete ui;
}

void CameraView::updateUi()
{
   /*Update every button with its respective value from the model*/
    for(int i =0;i<standardButtons.size();i++){
        standardButtons[i].button->setText(QString::number(model->getValue(DISP,standardButtons[i].value)));
    }
    for(int i =0;i<textButtons.size();i++){
        textButtons[i].button->setText(model->getTextValue(textButtons[i].value));
    }
}

void CameraView::setUpUi()
{
    /*Get information if the button should show text or Numbers or if it isnt available*/
    int value;
    std::vector<int> requested;
    requested = model->getRemainingTelegrams();
    textButtons.clear();
    standardButtons.clear();
        /*for every button get its property and store it in the corresponding arrays or disable it*/
        for (int i = 0; i < buttons.size(); i++) {

            value = model->getValue(DISP,buttons[i].value);

            switch (value) {
            /*Button shows number so it is a standard button*/
                default:
                    buttons[i].button->setDisabled(false);
                    buttons[i].button->setEnabled(true);
                    standardButtons.append(buttons[i]);
                break;
            case -2048:
            /*Button has no value so it needs to be disabled*/
                    buttons[i].button->setStyleSheet("color: white");
                    buttons[i].button->setDisabled(true);
                    buttons[i].button->setText("-");
            break;
            case -2049:
            /*Button shows text so it is a textButton*/
                    buttons[i].button->setDisabled(false);
                    buttons[i].button->setEnabled(true);
                    textButtons.append(buttons[i]);
        break;
            }
        /*Show values that couldn't be received from the camera in yellow*/
            if(!requested.empty()){
                std::vector<int>::iterator iterator = std::find(requested.begin(),requested.end(),buttons[i].value);

                if(iterator != requested.end() )
                   buttons[i].button->setStyleSheet("color: rgb(247,231,116)");
            }
       /*Show the values that could be received in white*/
            else {
                buttons[i].button->setStyleSheet("color: white");
            }
        }
}

void CameraView::setModelController(Model *model, Controller *controller)
{
    this->model = model;
    this->controller = controller;
}

/*Qt Slots for button clicks **************/
/*point to the respective field, the rotary encoder will queue the event
  value can be sent ***********/
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
/**************************************************/


void CameraView::stackChanged()
{
    /*Press the last pressed button in this window again*/
    QPushButton *button=ui->btGain;                                     //dummy
    QList<QPushButton*> allButtons=this->findChildren<QPushButton*>();  //find all buttons and put them in a list
    for(int i=0;i<allButtons.size();i++){
        if(allButtons[i]->isChecked())
        {button=allButtons[i];}     //find the one which was checked

    }
    button->click();                //click it
}

void CameraView::signalRequest(int property)
{
    /*Signal has been received after a second more  attemt*/
    /*find the button to the corresponding value and make it white*/
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
    /*Everything will be requestet new*/
    /*Make every button yellow*/
    for (int i = 0;i < textButtons.size(); i++) {

            textButtons[i].button->setStyleSheet("color: rgb(247,231,116)");

    }

    for (int i = 0;i < standardButtons.size(); i++) {

            standardButtons[i].button->setStyleSheet("color: rgb(247,231,116)");

    }
}
