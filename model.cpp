#include "model.h"
#include "view.h"
#include "config.h"
#include "bbmcommandtypes.h"
#include <QString>
#include <stdio.h>

Model::Model()
{
    count = 0;
    x = 0;
    y = 0;

    memset(cameras,0,sizeof(cameras));
    //set all cameras to type 1 (CODGER)
    for (int i=0; i < NUMBER_OF_SLOTS; i++) {
        setCamTypeWithDefValues(i,SONY_EVS_CODGER);
        //set all flags to flase
        for (int j=0;j<NUMBER_OF_FLAGS;j++) {
            cameras[i].flags[j] = false;
        }
    }
    for (int i=0;i<NUMBER_OF_SLOTS;i++) { //every camera a different HeadNr
        cameras[i].values[V_HEADNR][0]=i+1;
        cameras[i].xptSource=i+1;
    }

    activeCameraSlot=0;
    xptFields[0]=192;
    xptFields[1]=168;
    xptFields[2]=1;
    xptFields[3]=2;
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
    cameras[activeCameraSlot].usedPresets |= 1 << presetNr;
     //cameras[activeCameraSlot].usedPresets[presetNr]=1;
}

//overload, absolut value from savefile
void Model::setUsedPreset(int slotNr, int presetNr)
{
    cameras[slotNr].usedPresets = presetNr;
}

int Model::getUsedPreset()
{
    return cameras[activeCameraSlot].usedPresets;
}

//overload
int Model::getUsedPreset(int slotNr)
{
    return cameras[slotNr].usedPresets;
}

void Model::setActivePreset(int actPreset)
{
    cameras[activeCameraSlot].activePreset=actPreset;
    emit updateView();
}

//overload
void Model::setActivePreset(int slotNr, int actPreset)
{
    cameras[slotNr].activePreset=actPreset;
    emit updateView();
}

int Model::getActivePreset()
{
    return cameras[activeCameraSlot].activePreset;
}

//overload
int Model::getActivePreset(int slotNr)
{
    return cameras[slotNr].activePreset;
}


// returns headnumber
int Model::setActiveCameraSlot(int slotNr)
{
    activeCameraSlot=slotNr;
    emit updateView();
    // update connection status of camera
    if(cameras[slotNr].flags[F_CONNECTED]){
        emit updateCameraConnectionStatus(true);
    }
    else{
        emit updateCameraConnectionStatus(false);
    }

    return cameras[activeCameraSlot].values[V_HEADNR][VAL];
}

int Model::getActiveCameraSlot()
{
    return activeCameraSlot;
}

QStringList* Model::getErrorList(){
    return &errorList;
}

void Model::setCamType(int slotNr, int type){
    cameras[slotNr].camType=type;
}

void Model::setCamTypeWithDefValues(int slotNr, int type)
{
    switch (type) {
    case 1:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=c1Values[i][j];}
        }
        cameras[slotNr].camType=type;
        cameras[slotNr].textTable=&c1TextTable[0][0];
        break;
    case 2:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=c2Values[i][j];}
        }
        cameras[slotNr].camType=type;
        cameras[slotNr].textTable=&c2TextTable[0][0];
        break;
    case 5:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=rValues[i][j];}
        }
        cameras[slotNr].camType=type;
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    case 6:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=rValues[i][j];}
        }
        cameras[slotNr].values[V_ND_FILTER][0]=0;
        cameras[slotNr].values[V_ND_FILTER][1]=0;
        cameras[slotNr].values[V_ND_FILTER][2]=3;
        cameras[slotNr].values[V_ND_FILTER][3]=4;
        cameras[slotNr].camType=type;
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    }
}


// for active camera
unsigned char Model::getCamtype()
{
    return cameras[activeCameraSlot].camType;
}

// overload for specific camera
unsigned char Model::getCamtype(int slotNr)
{
    return cameras[slotNr].camType;
}

void Model::setValue(int type, int property, int value)
{
    switch (type) {
    case ABS:
        cameras[activeCameraSlot].values[property][VAL]=value;
        emit updateView();
        break;
    case INC:
        cameras[activeCameraSlot].values[property][VAL]+=value;
        qDebug("Slot: %d, Value: %d", activeCameraSlot, cameras[activeCameraSlot].values[property][VAL]);
        emit updateView();
        if (cameras[activeCameraSlot].values[property][VAL] > cameras[activeCameraSlot].values[property][MAX]) {
            cameras[activeCameraSlot].values[property][VAL] = cameras[activeCameraSlot].values[property][MAX];
       emit updateView();
        }
        if (cameras[activeCameraSlot].values[property][VAL] < cameras[activeCameraSlot].values[property][MIN]) {
            cameras[activeCameraSlot].values[property][VAL] = cameras[activeCameraSlot].values[property][MIN];
        emit updateView();
        }
        break;
    default:
        break;
    }
}

//overload with slotNr
void Model::setValue(int slotNr, int type, int property, int value)
{
    switch (type) {
    case ABS:
        cameras[slotNr].values[property][VAL]=value;
        emit updateView();
        break;
    case INC:
        cameras[slotNr].values[property][VAL]+=value;
        qDebug("Slot: %d, Value: %d", slotNr, cameras[slotNr].values[property][VAL]);
        emit updateView();
        if (cameras[slotNr].values[property][VAL] > cameras[slotNr].values[property][MAX]) {
            cameras[slotNr].values[property][VAL] = cameras[slotNr].values[property][MAX];
       emit updateView();
        }
        if (cameras[slotNr].values[property][VAL] < cameras[slotNr].values[property][MIN]) {
            cameras[slotNr].values[property][VAL] = cameras[slotNr].values[property][MIN];
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
        return cameras[activeCameraSlot].values[property][VAL];
    case DISP:
        switch (cameras[activeCameraSlot].values[property][TYP]) {
        case NORMAL:
            return cameras[activeCameraSlot].values[property][VAL];
        case CENTER:
            return cameras[activeCameraSlot].values[property][VAL]-(cameras[activeCameraSlot].values[property][MAX]-cameras[activeCameraSlot].values[property][MIN])/2;
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

//overload with slot nr
int Model::getValue(int slotNr, int type, int property)
{
    switch (type) {
    case ABS:
        return cameras[slotNr].values[property][VAL];
    case DISP:
        switch (cameras[slotNr].values[property][TYP]) {
        case NORMAL:
            return cameras[slotNr].values[property][VAL];
        case CENTER:
            return cameras[slotNr].values[property][VAL]-(cameras[slotNr].values[property][MAX]-cameras[slotNr].values[property][MIN])/2;
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
    return *(cameras[activeCameraSlot].textTable+(cameras[activeCameraSlot].values[property][4]*15)+(cameras[activeCameraSlot].values[property][0]-cameras[activeCameraSlot].values[property][1]));
}


int Model::getMin(int property)
{
    return cameras[activeCameraSlot].values[property][MIN];
}

int Model::getMax(int property)
{
    return cameras[activeCameraSlot].values[property][MAX];
}

void Model::setCamFlag(int flag, bool value)
{
   cameras[activeCameraSlot].flags[flag]=value;
}

//overload with slotNr
void Model::setCamFlag(int slotNr, int flag, bool value)
{
   cameras[slotNr].flags[flag]=value;
}

bool Model::getCamFlag(int flag)
{
    return cameras[activeCameraSlot].flags[flag];
}

//overload with slotNr
bool Model::getCamFlag(int slotNr, int flag)
{
    return cameras[slotNr].flags[flag];
}

int Model::setWatchdogWaitingflag(bool waiting){
    if(waiting){
        if(watchdogWaitingForAnswerFlag){
            if(serverConnected){
                emit updateServerConnectionStatus(false);       //signal: connection lost
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
            emit updateServerConnectionStatus(true);        //signal: connected
        }
        serverConnected = true;
        return 0;           // ok, clear flag
    }
}

int Model::setCameraWaitingflag(int slotNr, bool waiting){
    if(waiting){        // out ping sent -> decrement stack 1
        if (cameraWaitingForAnswerStack[slotNr]<=0) {   // empty -> signal disconnect if connection status not already disconnected
            if(cameras[slotNr].flags[F_CONNECTED]){
                emit updateCameraConnectionStatus(false);        //signal: disconnected
                cameras[slotNr].flags[F_CONNECTED] = false;
            }
        }
        else {
            cameraWaitingForAnswerStack[slotNr]--;
        }
    }
    else{   // Answer received -> fill stack, send signal if connection status was not already ok
        cameraWaitingForAnswerStack[slotNr] = 2;
        if(!cameras[slotNr].flags[F_CONNECTED]){
            emit updateCameraConnectionStatus(true);        //signal: connected
        }
        cameras[slotNr].flags[F_CONNECTED] = true;
        return 0;           // ok, clear flag
    }
    //qDebug("stack: %d",cameraWaitingForAnswerStack[slotNr]);
}

int Model::getRotaryField()
{
    return rotaryField;
}

int Model::getRotaryDestination()
{
    return rotaryDestination;
}

void Model::setRotaryField(int field, int destination)
{
    rotaryDestination=destination;
    rotaryField=field;
}

int Model::getTxCommand(int value)
{
    return commandtype[value];
}

int Model::getValueFromBBMCommand(int bbm_command){
    for (int i=0;i<ROW_ENTRIES;i++) {
        if (bbm_command == commandtype[i]) {
            return i;
        }
    }
    return -1;
}

void Model::setTextTable(int slotNr, int type){
    switch (type) {
    case 1:
        cameras[slotNr].textTable=&c1TextTable[0][0];
        break;
    case 2:
        cameras[slotNr].textTable=&c2TextTable[0][0];
        break;
    case 5:
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    case 6:
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    }
}

int Model::toggleBlink(){
    blinkToggle=!blinkToggle;
    return blinkToggle;
}

void Model::setXptConnected(bool flag)
{
   xptConnect=flag;
   emit updateXptConnectionStatus(xptConnect);
}

bool Model::getXptConnected()
{
    return xptConnect;
}

void Model::setSlotSource(int source)
{
   cameras[xptSlot].xptSource+=source;
    emit updateView();

   if(cameras[xptSlot].xptSource > 40){
       cameras[xptSlot].xptSource = 40;
        emit updateView();
   }
   if(cameras[xptSlot].xptSource < 1){
       cameras[xptSlot].xptSource = 1;
        emit updateView();
   }

}

int Model::getSlotSource(int slotNr)
{
    return  cameras[slotNr].xptSource;
}

void Model::setXptDestination(int destination)
{
    xptDestination+=destination;
    emit updateView();
    if(xptDestination > 40){
        xptDestination = 40;
         emit updateView();
    }
    if(xptDestination < 1){
        xptDestination = 1;
         emit updateView();
    }

}

int Model::getXptDestination()
{
    return xptDestination;
}

void Model::setXptSlot(int slot)
{
    xptSlot=slot;
}

int Model::getXptSlot()
{
    return xptSlot;
}

void Model::setXptIpField(int field, int value)
{
    xptFields[field]+=value;
    emit updateView();
    if(xptFields[3] < 2){
        xptFields[3] = 2;
         emit updateView();
    }
    if(xptFields[field] > 254){
        xptFields[field] = 254;
         emit updateView();
    }
    if(xptFields[field] < 0){
        xptFields[field] = 0;
         emit updateView();
    }


}

int Model::getXptIpField(int field)
{
    return xptFields[field];
}


char *Model::getXptIpAdress()
{
    sprintf(xptIpAddress,"%d.%d.%d.%d",xptFields[0],xptFields[1],xptFields[2],xptFields[3]);
    return &xptIpAddress[0];
}
