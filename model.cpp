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
    xptNumberOfInputs = 8;
    xptNumberOfOutputs = 8;

    memset(cameras,0,sizeof(cameras));
    /*set all cameras to type 1 (CODGER)*/
    for (int i=0; i<NUMBER_OF_SLOTS; i++) {
        setCamTypeWithDefValues(i,SONY_EVS_CODGER);

        setValue(i, ABS,V_HEADNR,i+1);   //set headNr.
        /*set all flags to flase*/
        for (int j=0;j<NUMBER_OF_FLAGS;j++) {
            cameras[i].flags[j] = false;
        }
    }
/*
   for (int i=0;i<NUMBER_OF_SLOTS;i++) { // set all to not pending
        for (int j=0;j<ROW_ENTRIES;j++) {
            if(cameras[i].values[j][5]==REQUESTABLE){
                cameras[i].remainingTelegrams.push_back(j);
            }
        }
    }*/

    //cameras[0].remainingTelegrams.push_back(10);


    // emit updateView(); -> not possible yet, signals not routed at this point -> do in controller
}

/*Add error to error list*/
void Model::addError(std::string str){
    errorList.prepend(QString::fromStdString(str));
    emit updateView();
}

/*Clear error list*/
void Model::clearErrors(){
    errorList.clear();
    emit updateView();
}

/*Set used presets in current slot*/
void Model::setUsedPreset(int presetNr)
{
    cameras[activeCameraSlot].usedPresets |= 1 << presetNr;
     //cameras[activeCameraSlot].usedPresets[presetNr]=1;
}

/*Overload function, with slot number*/
void Model::setUsedPreset(int slotNr, int presetNr)
{
    cameras[slotNr].usedPresets = presetNr;
}

/**/
void Model::clearUsedPresets()
{
    cameras[activeCameraSlot].usedPresets = 0;
}

/**/
int Model::getUsedPreset()
{
    return cameras[activeCameraSlot].usedPresets;
}

//overload
int Model::getUsedPreset(int slotNr)
{
    return cameras[slotNr].usedPresets;
}

/*Set currently active preset in current slot*/
void Model::setActivePreset(int actPreset)
{
    cameras[activeCameraSlot].activePreset=actPreset;
    emit updateView();
}

/*Overload function with slot number*/
void Model::setActivePreset(int slotNr, int actPreset)
{
    cameras[slotNr].activePreset=actPreset;
    emit updateView();
}

/*Get active preset from current slot*/
int Model::getActivePreset()
{
    return cameras[activeCameraSlot].activePreset;
}

/*Overload function, with slot number*/
int Model::getActivePreset(int slotNr)
{
    return cameras[slotNr].activePreset;
}


/*Change active camera slot*/
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

/*Get active camera slot*/
int Model::getActiveCameraSlot()
{
    return activeCameraSlot;
}

/*Get error list*/
QStringList* Model::getErrorList(){
    return &errorList;
}

/*Change camera type*/
void Model::setCamTypeOnly(int slotNr, int type){
    cameras[slotNr].camType=type;
}

/*Change camera type and according min,max values*/
void Model::setCamType(int slotNr, int type){
    if(cameras[slotNr].camType != type){
        cameras[slotNr].camType=type;
        setCamTypeWithDefBorders(slotNr,type);
    }
}

/*Change camera type and load default values*/
void Model::setCamTypeWithDefValues(int slotNr, int type)
{
    int start = 0;       //with head no
    switch (type) {
    case 1:
        /*CODGER*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=c1Values[i][j];}
        }
        for(int k=1;k<COLUM_ENTRIES;k++){cameras[slotNr].values[0][k]=c1Values[0][k];}  //headNr without actual value
        cameras[slotNr].textTable=&c1TextTable[0][0];
        break;
        
    case 2:
        /*MH322*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=c2Values[i][j];}
        }
        for(int k=1;k<COLUM_ENTRIES;k++){cameras[slotNr].values[0][k]=c2Values[0][k];}  //headNr without actual value
        cameras[slotNr].textTable=&c2TextTable[0][0];
        break;
        
    case 4:
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=ursaValues[i][j];}
        }
        for(int k=1;k<COLUM_ENTRIES;k++){cameras[slotNr].values[0][k]=ursaValues[0][k];}  //headNr without actual value
        cameras[slotNr].textTable=&ursaTextTable[0][0];
        break;

    case 5:
        /*HPX600, URSA, HITACHI*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=rValues[i][j];}
        }
        for(int k=1;k<COLUM_ENTRIES;k++){cameras[slotNr].values[0][k]=rValues[0][k];}  //headNr without actual value
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    case 6:
        /*RAVEN*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=rValues[i][j];}
        }
        for(int k=1;k<COLUM_ENTRIES;k++){cameras[slotNr].values[0][k]=rValues[0][k];}  //headNr without actual value
        cameras[slotNr].values[V_ND_FILTER][0]=0;
        cameras[slotNr].values[V_ND_FILTER][1]=0;
        cameras[slotNr].values[V_ND_FILTER][2]=3;
        cameras[slotNr].values[V_ND_FILTER][3]=TEXT;
        cameras[slotNr].values[V_ND_FILTER][4]=4;
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    }
    cameras[slotNr].camType=type;
}

/*Load default min,max values. Requestable properties will be requested from camera, nonrequestable set default values*/
void Model::setCamTypeWithDefBorders(int slotNr, int type)
{
    int start = 1;       //no change on head number in slot (omit first entry)
    switch (type) {
    case 1:
        /*Fill in default values (min, max etc..), but not actual value*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=1;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=c1Values[i][j];}
        }

        /*All nonrequestable values: write standard values and prevent out of bounds ??*/
        for (int i=start;i<ROW_ENTRIES;i++) {
            if(cameras[slotNr].values[i][5] != REQUESTABLE){
                if (cameras[slotNr].values[i][VAL] > cameras[slotNr].values[i][MAX] ) {
                    cameras[slotNr].values[i][VAL]=c1Values[i][MAX];
                }
                if (cameras[slotNr].values[i][VAL] < cameras[slotNr].values[i][MIN] ) {
                    cameras[slotNr].values[i][VAL]=c1Values[i][MIN];
                }

                //cameras[slotNr].values[i][VAL]=c1Values[i][VAL];
            }
        }
        cameras[slotNr].textTable=&c1TextTable[0][0];
        break;
    case 2:
        /*Fill in default values (min, max etc..), but not actual value*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=1;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=c2Values[i][j];}
        }
        /*Load default values for nonrequestable values*/
        for (int i=start;i<ROW_ENTRIES;i++) {
            if(cameras[slotNr].values[i][5] != REQUESTABLE){
                cameras[slotNr].values[i][VAL]=c2Values[i][VAL];
            }
        }
        cameras[slotNr].textTable=&c2TextTable[0][0];
        break;

    case 4:
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=1;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=ursaValues[i][j];}
        }
        for (int i=start;i<ROW_ENTRIES;i++) {
            //all nonrequestable values: write standard value
            if(cameras[slotNr].values[i][5] != REQUESTABLE){
                cameras[slotNr].values[i][VAL]=ursaValues[i][VAL];
            }
        }
        cameras[slotNr].textTable=&ursaTextTable[0][0];
        break;

    case 5:
        /*Fill in default values (min, max etc..), but not actual value*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=1;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=rValues[i][j];}
        }
        for (int i=start;i<ROW_ENTRIES;i++) {
            /*Load default values for nonrequestable values*/
            if(cameras[slotNr].values[i][5] != REQUESTABLE){
                cameras[slotNr].values[i][VAL]=rValues[i][VAL];
            }
        }
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    case 6:
        /*Fill in default values (min, max etc..), but not actual value*/
        for(int i= start;i<ROW_ENTRIES;i++){
            for(int j=1;j<COLUM_ENTRIES;j++){cameras[slotNr].values[i][j]=rValues[i][j];}
        }
        /*Load default values for nonrequestable values*/
        for (int i=start;i<ROW_ENTRIES;i++) {
            if(cameras[slotNr].values[i][5] != REQUESTABLE){
                cameras[slotNr].values[i][VAL]=rValues[i][VAL];
            }
        }
        /*Only RAVEN has ND Filters*/
        cameras[slotNr].values[V_ND_FILTER][1]=0;
        cameras[slotNr].values[V_ND_FILTER][2]=3;
        cameras[slotNr].values[V_ND_FILTER][3]=TEXT;
        cameras[slotNr].values[V_ND_FILTER][4]=4;
        cameras[slotNr].textTable=&rTextTable[0][0];
        break;
    }
    cameras[slotNr].camType=type;
}


/*Get camera type of current slot*/
int Model::getCamtype()
{
    return cameras[activeCameraSlot].camType;
}

/*Overload function with slot number*/
int Model::getCamtype(int slotNr)
{
    return cameras[slotNr].camType;
}

/*Setter in current slot*/
void Model::setValue(int type, int property, int value)
{
    switch (type) {
    case ABS:
        /*Absolute change*/
        cameras[activeCameraSlot].values[property][VAL]=value;
        emit updateView();
        break;
    case INC:
        /*Incremental change, prevent out of bound*/
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

/*Overload with slot number*/
void Model::setValue(int slotNr, int type, int property, int value)
{
    switch (type) {
    case ABS:
        /*Absolute change*/
        if (value > cameras[slotNr].values[property][MAX]) { //prevent buffer overflow, with garbage values
            cameras[slotNr].values[property][VAL] = cameras[slotNr].values[property][MAX];
        }
         else if (value < cameras[slotNr].values[property][MIN]) {
            cameras[slotNr].values[property][VAL] = cameras[slotNr].values[property][MIN];
        }
        else{
        cameras[slotNr].values[property][VAL]=value;
        }
        emit updateView();
        break;
    case INC:
        /*Incremental change, prevent out of bound*/
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

/*Getter in current slot*/
int Model::getValue(int type, int property)
{
    switch (type) {
    case ABS:
        /*Return absolute value*/
        return cameras[activeCameraSlot].values[property][VAL];
    case DISP:
        /*Return value to show in GUI*/
        switch (cameras[activeCameraSlot].values[property][TYP]) {
        case NORMAL:
            /*Absolute value*/
            return cameras[activeCameraSlot].values[property][VAL];
        case CENTER:
            /*Value stored 0...255, displayed as -128...+128 (as example)*/
            return cameras[activeCameraSlot].values[property][VAL]-(cameras[activeCameraSlot].values[property][MAX] \
                                                                    -cameras[activeCameraSlot].values[property][MIN])/2;
        case NAN:
            /*GUI does not need to display anything*/
            return -2048;
        case TEXT:
            /*GUI should display text*/
            return -2049;
        case OFFSET:
            /*Special case for spp, because position is stored 0...5*/
            return cameras[activeCameraSlot].values[property][VAL]+1;
        default:
            return -1;
        }
    default:
        return -1;
    }
}

/*Overload with slot number*/
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
        case OFFSET:
            return cameras[activeCameraSlot].values[property][VAL]+1;
        default:
            return -1;
        }
    default:
        return -1;
    }
}

/*Text getter*/
QString Model::getTextValue(int property)
{
    return *(cameras[activeCameraSlot].textTable+(cameras[activeCameraSlot].values[property][4]*15)+ \
            (cameras[activeCameraSlot].values[property][0]-cameras[activeCameraSlot].values[property][1]));
}

/*Minimum getter*/
int Model::getMin(int property)
{
    return cameras[activeCameraSlot].values[property][MIN];
}

/*Maximum getter*/
int Model::getMax(int property)
{
    return cameras[activeCameraSlot].values[property][MAX];
}

/*Camera flag setter*/
void Model::setCamFlag(int flag, bool value)
{
   cameras[activeCameraSlot].flags[flag]=value;
}

/*Overload function with slot number*/
void Model::setCamFlag(int slotNr, int flag, bool value)
{
   cameras[slotNr].flags[flag]=value;
}

/*Camera flag getter*/
bool Model::getCamFlag(int flag)
{
    return cameras[activeCameraSlot].flags[flag];
}

/*Overload function with slot number*/
bool Model::getCamFlag(int slotNr, int flag)
{
    return cameras[slotNr].flags[flag];
}

/*Determine server connection status*/
int Model::setWatchdogWaitingflag(bool waiting){
    if(waiting){
        if(watchdogWaitingForAnswerFlag){
            /*Already waiting since last Watchdog ping sent*/
            if(serverConnected){
                emit updateServerConnectionStatus(false);       //signal: connection lost
            }
            serverConnected = false;
            return -1;      // error, already waiting for answer -> connection lost
        }
        else{
            /*Answer from last Watchdog ping was received, set flag to waiting*/
            watchdogWaitingForAnswerFlag = true;
            return 0;       // ok, set waiting
        }
    }
    else{
        /*Server answer received, clear waiting flag*/
        watchdogWaitingForAnswerFlag = false;
        if(!serverConnected){
            emit updateServerConnectionStatus(true);        //signal: connected
        }
        serverConnected = true;
        return 0;           // ok, clear flag
    }
}

/*Determine camera connection status*/
int Model::setCameraWaitingflag(int slotNr, bool waiting){
    if(waiting){
        /*Watchdog ping sent, decrement stack*/
        if (cameraWaitingForAnswerStack[slotNr]<=0) {
            /*Stack empty, signal disconnect if connection status not already disconnected*/
            if(cameras[slotNr].flags[F_CONNECTED]){
                if(slotNr == getActiveCameraSlot()){
                    emit updateCameraConnectionStatus(false);
                }
                cameras[slotNr].flags[F_CONNECTED] = false;
                //cameras[slotNr].flags[F_CAMERA_KNOWN] = false;
            }
        }
        else {
            cameraWaitingForAnswerStack[slotNr]--;
        }
    }
    else{
        /*Answer received. Fill stack, send connected signal if connection status not already ok*/
        cameraWaitingForAnswerStack[slotNr] = 2;
        if(!cameras[slotNr].flags[F_CONNECTED]){
            if(slotNr == getActiveCameraSlot()){
                emit updateCameraConnectionStatus(true);        //signal: connected
            }
        }
        cameras[slotNr].flags[F_CONNECTED] = true;
        return 0;           // ok, clear flag
    }
    return 0;
    //qDebug("stack: %d",cameraWaitingForAnswerStack[slotNr]);
}

/**/
int Model::getRotaryField()
{
    return rotaryField;
}

/**/
int Model::getRotaryDestination()
{
    return rotaryDestination;
}

/**/
void Model::setRotaryField(int field, int destination)
{
    rotaryDestination=destination;
    rotaryField=field;
}

/*Get associated commandtype from property*/
int Model::getTxCommand(int value)
{
    return commandtype[value];
}

/*Get associated property from commandtype*/
int Model::getValueFromBBMCommand(int bbm_command){
    for (int i=0;i<ROW_ENTRIES;i++) {
        if (bbm_command == commandtype[i]) {
            return i;
        }
    }
    /*Not all properties have a command type*/
    return -1;
}

/**/
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

/*LED blinking for bounce function*/
int Model::toggleBlink(){
    blinkToggle=!blinkToggle;
    return blinkToggle;
}

/**/
void Model::setXptConnected(bool flag)
{
  if(xptConnect != flag){
      xptConnect=flag;
      emit updateXptConnectionStatus(xptConnect);
  }

}

/**/
bool Model::getXptConnected()
{
    return xptConnect;
}

/**/
void Model::setXptSlotSource(int source)
{
   cameras[xptSlot].xptSource+=source;


   if(cameras[xptSlot].xptSource >= xptNumberOfInputs){
       cameras[xptSlot].xptSource = xptNumberOfInputs-1;
   }
   if(cameras[xptSlot].xptSource < 1){
       cameras[xptSlot].xptSource = 0;
   }
       emit updateView();

}

/**/
int Model::getXptSlotSource(int slotNr)
{
    return  cameras[slotNr].xptSource;
}

/**/
void Model::setXptDestination(int destination)
{
    xptDestination+=destination;

    if(xptDestination >= xptNumberOfOutputs){
        xptDestination = xptNumberOfOutputs-1;
    }
    if(xptDestination < 1){
        xptDestination = 0;
    }
     emit updateView();

}

/**/
int Model::getXptDestination()
{
    return xptDestination;
}

/**/
void Model::setXptSlot(int slot)
{
    xptSlot=slot;
}

/**/
int Model::getXptSlot()
{
    return xptSlot;
}

/**/
void Model::setXptIpField(int type,int field, int value)
{
    switch (type) {
    case INC:
        xptFields[field]+=value;


        if(xptFields[3] < 2){
            xptFields[3] = 2;

        }
        if(xptFields[field] > 254){
            xptFields[field] = 254;

        }
        if(xptFields[field] < 0){
            xptFields[field] = 0;

        }
        emit updateView();
    break;

    case ABS:
        xptFields[field]=value;


        if(xptFields[3] < 2){
            xptFields[3] = 2;

        }
        if(xptFields[field] > 254){
            xptFields[field] = 254;

        }
        if(xptFields[field] < 0){
            xptFields[field] = 0;

        }
        emit updateView();
    break;
    default:
        break;

    }


}

/**/
int Model::getXptIpField(int field)
{
    return xptFields[field];
}

/**/
char *Model::getXptIpAdress()
{
    sprintf(xptIpAddress,"%d.%d.%d.%d",xptFields[0],xptFields[1],xptFields[2],xptFields[3]);
    return &xptIpAddress[0];
}

/**/
void Model::setXptNumberOfInputs(int inputs)
{
    if(inputs != xptNumberOfInputs){
        xptNumberOfInputs = inputs;
    }
}

/**/
int Model::getXptNumberOfInputs()
{
    return xptNumberOfInputs;
}

/**/
void Model::setXptNumberOfOutputs(int outputs)
{
    if(outputs != xptNumberOfOutputs){
        xptNumberOfOutputs = outputs;
    }
}

/**/
int Model::getXptNumberOfOutputs()
{
    return xptNumberOfOutputs;
}

/**/
bool Model::getXptEnabled()
{
    return xptEnabled;
}

/**/
void Model::setXptEnabled(bool flag)
{
    if(xptEnabled != flag){
        xptEnabled = flag;
    }
}

/**/
void Model::setXptInputLables(QList<QString> inputLables)
{
    if(!inputLables.empty()){
        xptInputLabels=inputLables;
    }
}

/**/
void Model::setXptOutputLables(QList<QString> outputLables)
{
    if(!outputLables.empty()){
        xptOutputLabels=outputLables;
    }
}

/**/
QList<QString> Model::getXptInputLables()
{
    return xptInputLabels;
}

/**/
QList<QString> Model::getXptOutputLables()
{
    return xptOutputLabels;
}

/**/
void Model::setXptType(int type)
{
    if (xptType != type) {
        xptType = type;
    }

}

/**/
int Model::getXptType()
{
    return xptType;
}

/**/
bool Model::getFastIris()
{
    return fastIris;
}

/**/
void Model::setFastIris(bool flag)
{
    if (fastIris != flag)
    {
        fastIris = flag;
    }
}

/*Spp state setter*/
void Model::setSppState(int slotNr, int state){
    sppState[slotNr] = state;
}

/*Spp state getter*/
int Model::getSppState(int slotNr){
    return sppState[slotNr];

}

/*?? unused*/
bool Model::getRequestSettingsFlag(){
    return requestSettingsFlag;
}

/*?? unused*/
void Model::setRequestSettingsFlag(bool value){
    requestSettingsFlag = value;
}

/*?? unused*/
void Model::setReqPendArr(int pos, bool value){
    if(pos< MAX_NUMBER_OF_CMDS){
        reqPendingArr[pos] = value;
    }
}

/*?? unused*/
bool Model::getReqPendArr(int pos){
    if(pos >= MAX_NUMBER_OF_CMDS){
        return false;
    }
    else{
        return reqPendingArr[pos];
    }
}

/*?? unused*/
void Model::setCurrReqHeadNr(int headNr){
    currReqHeadNr = headNr;
}

/*?? unused*/
int Model::getCurrReqHeadNr(){
    return currReqHeadNr;
}

/*Pushes requested property into remainingTelegrams. Used to check camera settings requests*/
int Model::getRequestCommand(int slotNr, int property)
{
    //cameras[slotNr].remainingTelegrams.clear();
    if(cameras[slotNr].values[property][5]==REQUESTABLE){
        std::unique_lock<std::mutex> lock(cameras[slotNr].mtx);     //mutex
        cameras[slotNr].remainingTelegrams.push_back(property);
        return commandtype[property];
    }
    else {
        return -1;
    }
}

/*Remove entry from remainingTelegrams*/
void Model::setRequestReceived(int slotNr, int property)
{
    std::unique_lock<std::mutex> lock(cameras[slotNr].mtx);     //mutex
    if (!cameras[slotNr].remainingTelegrams.empty()) {
        std::vector<int>::iterator iterator = std::find(cameras[slotNr].remainingTelegrams.begin(),cameras[slotNr].remainingTelegrams.end(),property);
        if(iterator != cameras[slotNr].remainingTelegrams.end()){
            cameras[slotNr].remainingTelegrams.erase(iterator);
            if(activeCameraSlot == slotNr){
                emit newSignalReceived(property);
                emit setUpView();
                emit updateView();
            }
        }
    }
    else {
        setCamFlag(slotNr,F_RECEIVED_ALL,true);
    }
}

/*Returns remaining telegrams from camera request*/
std::vector<int> Model::getRemainingTelegrams()
{
    std::unique_lock<std::mutex> lock(cameras[activeCameraSlot].mtx);       //mutex because of gui thread
    return cameras[activeCameraSlot].remainingTelegrams;
}

/*Overload function with slot number*/
std::vector<int> Model::getRemainingTelegrams(int slotNr)
{
    std::unique_lock<std::mutex> lock(cameras[slotNr].mtx);     // mutex
    return cameras[slotNr].remainingTelegrams;
}

/*Clears all remaining telegrams from camera request*/
void Model::clearRemainingTelegrams(int slotNr)
{
    std::unique_lock<std::mutex> lock(cameras[slotNr].mtx);     // mutex
    cameras[slotNr].remainingTelegrams.clear();
}


