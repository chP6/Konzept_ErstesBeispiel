#include "model.h"
#include "view.h"
#include "config.h"
#include "bbmcommandtypes.h"
#include <QString>

Model::Model()
{
    count = 0;
    x = 0;
    y = 0;

    memset(cameras,0,sizeof(cameras));
    //set all cameras to type 1 (CODGER)
    for (int i=1; i < NUMBER_OF_CAMERAS+1; i++) {
        setCamType(i,SONY_EVS_CODGER);
    }
    activeCamera=1;
}

void Model::setData(int data){
    count = data;
    emit updateView();
}

int Model::getData(void){
    return count;
}

void Model::setAxis(int x, int y){
    this->x = x;
    this->y = y;
    emit updateView();
}

void Model::getAxis(int &x, int &y){
    x = this->x;
    y = this->y;
}

void Model::addError(std::string str){
    errorList.prepend(QString::fromStdString(str));
    emit updateView();
}

void Model::clearErrors(){
    errorList.clear();
    emit updateView();
}

void Model::setUsedPreset(int presetNr)
{
    cameras[activeCamera].usedPresets |= 1 << presetNr;
     //cameras[activeCamera].usedPresets[presetNr]=1;
}

int Model::getUsedPreset()
{
    return cameras[activeCamera].usedPresets;
}

void Model::setActivePreset(unsigned char actPreset)
{
    cameras[activeCamera].activePreset=actPreset;
    emit updateView();
}

unsigned char Model::getActivePreset()
{
    return cameras[activeCamera].activePreset;
}



void Model::setActiveCamera(unsigned char camNr)
{
    activeCamera=camNr;
    emit updateView();
}

unsigned char Model::getActiveCamera()
{
    return activeCamera;
}

QStringList* Model::getErrorList(){
    return &errorList;
}

void Model::setCamType(int camNr, int type)
{
    switch (type) {
    case 1:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[camNr-1].values[i][j]=c1Values[i][j];}
        }
        cameras[camNr-1].camType=type;
        cameras[camNr-1].textTable=&c1TextTable[0][0];
        break;
    case 2:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[camNr-1].values[i][j]=c2Values[i][j];}
        }
        cameras[camNr-1].camType=type;
        break;
    case 5:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[camNr-1].values[i][j]=rValues[i][j];}
        }
        cameras[camNr-1].camType=type;
        break;
    case 6:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[camNr-1].values[i][j]=rValues[i][j];}
        }
        cameras[camNr-1].values[V_ND_FILTER][0]=0;
        cameras[camNr-1].values[V_ND_FILTER][1]=0;
        cameras[camNr-1].values[V_ND_FILTER][2]=3;
        cameras[camNr-1].camType=type;
        break;

    }
}


// for active camera
unsigned char Model::getCamtype()
{
    return cameras[activeCamera-1].camType;
}

// overload for specific camera
unsigned char Model::getCamtype(int camNr)
{
    return cameras[camNr-1].camType;
}

void Model::setValue(int type, int property, int value)
{
    switch (type) {
    case ABS:
        cameras[activeCamera-1].values[property][VAL]=value;
        emit updateView();
        break;
    case INC:
        cameras[activeCamera-1].values[property][VAL]+=value;
        emit updateView();
        if (cameras[activeCamera-1].values[property][VAL] > cameras[activeCamera-1].values[property][MAX]) {
            cameras[activeCamera-1].values[property][VAL] = cameras[activeCamera-1].values[property][MAX];
       emit updateView();
        }
        if (cameras[activeCamera-1].values[property][VAL] < cameras[activeCamera-1].values[property][MIN]) {
            cameras[activeCamera-1].values[property][VAL] = cameras[activeCamera-1].values[property][MIN];
        emit updateView();
        }
        break;
    default:
        break;
    }
}

int Model::getValue(int type, int property)
{
    switch (type) {
    case ABS:
        return cameras[activeCamera-1].values[property][VAL];
    case DISP:
        switch (cameras[activeCamera-1].values[property][TYP]) {
        case NORMAL:
            return cameras[activeCamera-1].values[property][VAL];
        case CENTER:
            return cameras[activeCamera-1].values[property][VAL]-(cameras[activeCamera-1].values[property][MAX]-cameras[activeCamera-1].values[property][MIN])/2;
        case NAN:
            return -2048;
        case TEXT:
            return -2049;
        default:
            return -1;
        }
    default:
        return -1;
    }
}

QString Model::getTextValue(int property)
{
    return *(cameras[activeCamera-1].textTable+(cameras[activeCamera-1].values[property][4]*15)+(cameras[activeCamera-1].values[property][0]-cameras[activeCamera-1].values[property][1]));
}


int Model::getMin(int property)
{
    return cameras[activeCamera-1].values[property][MIN];
}

int Model::getMax(int property)
{
    return cameras[activeCamera-1].values[property][MAX];
}

void Model::setCamFlag(int flag, bool value)
{
   cameras[activeCamera-1].flags[flag]=value;
}

bool Model::getCamFlag(int flag)
{
    return cameras[activeCamera-1].flags[flag];
}

int Model::setWatchdogWaitingflag(bool waiting){
    if(waiting){
        if(watchdogWaitingForAnswerFlag){
            watchdogWaitingForAnswerFlag = true;
            if(serverConnected){
                emit updateServerConnectionStatus(false);       //signal connection lost
            }
            serverConnected = false;
            return -1;      // error, already waiting for answer -> connection lost
        }
        else{
            watchdogWaitingForAnswerFlag = true;
            return 0;       // ok, set waiting
        }
    }
    else{
        watchdogWaitingForAnswerFlag = false;
        if(!serverConnected){
            emit updateServerConnectionStatus(true);        //signal connected
        }
        serverConnected = true;
        return 0;           // ok, clear flag
    }
}

int Model::getRotaryField()
{
    return rotaryField;
}

void Model::setRotaryField(int field)
{
    rotaryField=field;
}

int Model::getTxCommand(int value)
{
    return commandtype[value];
}
