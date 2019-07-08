#include "controller.h"
#include "model.h"
#include "poller.h"
#include <unistd.h>
#include <thread>
#include "events.h"
#include <errno.h>
#include "config.h"
#include <QDebug>
#include <QSettings>

#include <chrono>
#include <ctime>
#include <sys/time.h>



void Controller::onXptLableChanged()
{
    model->setXptNumberOfInputs(xptSocket.getNumberOfInputs());
    model->setXptNumberOfOutputs(xptSocket.getNumberOfOutputs());
    model->setXptInputLables(xptSocket.getInputLabels());
    model->setXptOutputLables(xptSocket.getOutputLabels());
    model->updateView();
}

Controller::Controller(Model& model)// : poller(*this)    //poller Konstruktor aufrufen -> erwartet Objekt (as reference) darum this dereferenzieren
{
    setModel(model);
    contr_err = txSocket.init(9000);
    if(contr_err<0){
        contr_err = errno;  //zwischenspeichern (muss)
        logSystemError(contr_err, "Could not initialize udp-interface");
    }


    presetbus.setRow(UPPER_ROW);
    contr_err=presetbus.initSpi();
    if(contr_err<0){
        contr_err = errno;
        logSystemError(contr_err, "Could not open SPI-BUS 1");
    }

    camerabus.setRow(LOWER_ROW);
    camerabus.initSpi();
    if(contr_err<0){
        contr_err = errno;
        logSystemError(contr_err, "Could not open SPI-BUS 2");
    }

    camerabus.setLed(CAMERA_COLOR,model.getActiveCameraSlot());
    presetbus.setLed(PRESET_COLOR,model.getActivePreset());

    blinkTimer.init(E_BOUNCE_BLINK,*this);
    blinkTimer.setInterval(500*1000);

    xptWatchdog.init(E_XPT_WATCHDOG,*this);
    xptWatchdog.setInterval(2000*1000);


    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        reqSettingsTimer[i].init(E_REQ_SETTINGS_TIMER, i,*this);
        reqSettingsTimer[i].setInterval(1000*1000);
    }


    //init sppTimers
    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        sppTimer[i].init(E_SPP_WAIT_DONE,i,*this);
    }
    queueEvent(E_SETUP_HEAD);
   QObject::connect(&xptSocket, SIGNAL(inputLabelsChanged()),            // model signal mit view slot verbinden
                     this, SLOT(onXptLableChanged()));


}

void Controller::setModel(Model &model){
    this->model = &model;
}

void Controller::setPoller(Poller &poller){
    this->poller = &poller;
}

void Controller::increment(int inc){
    int value = model->getData();
    value+=inc;
    model->setData(value);
    //logError("Test Error");
}

void Controller::clear(){
    model->setData(0);
    usleep(2000*1000);
}

void Controller::setAxis(int x, int y){
    model->setAxis(x,y);
}

void Controller::logSystemError(int err_no, std::string msg){
    model->addError("(S) " + msg + " ERRNO: " + strerror(err_no));        //msg + string aus errno nummer
}

void Controller::logError(std::string msg){
    model->addError(msg);
}

void Controller::clearErrors(){
    model->clearErrors();
}

void Controller::settingsWrite(QSettings &savefile){
    savefile.clear();
    savefile.beginGroup("xpt");
    savefile.setValue("xptType",model->getXptType());
    savefile.setValue("xptIpField1",model->getXptIpField(0));
    savefile.setValue("xptIpField2",model->getXptIpField(1));
    savefile.setValue("xptIpField3",model->getXptIpField(2));
    savefile.setValue("xptIpField4",model->getXptIpField(3));
    savefile.setValue("xptNumberOfInputs",model->getXptNumberOfInputs());
    savefile.setValue("xptNumberOfOutputs",model->getXptNumberOfOutputs());
    savefile.setValue("xptDestination",model->getXptDestination());
    savefile.endGroup();
    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        savefile.beginGroup("slot_"+QString::number(i));

        savefile.setValue("camType",model->getCamtype(i));
        savefile.setValue("activePreset",model->getActivePreset(i));
        //savefile.setValue("usedPresets",model->getUsedPreset(i));
        savefile.setValue("xptSource",model->getXptSlotSource(i));
        for (int k=0;k<NUMBER_OF_FLAGS;k++) {
            savefile.setValue("flag_"+QString::number(k),model->getCamFlag(i,k));
        }
        for (int j=0;j<ROW_ENTRIES;j++) {
            savefile.setValue("value_"+QString::number(j),model->getValue(i,ABS,j));
        }
        savefile.endGroup();
    }
    savefile.sync();
}

void Controller::settingsLoad(QSettings &savefile, bool send){
    savefile.beginGroup("xpt");
        model->setXptType(savefile.value("xptType").toInt());
        model->setXptIpField(ABS,0,savefile.value("xptIpField1").toInt());
        model->setXptIpField(ABS,1,savefile.value("xptIpField2").toInt());
        model->setXptIpField(ABS,2,savefile.value("xptIpField3").toInt());
        model->setXptIpField(ABS,3,savefile.value("xptIpField4").toInt());
        model->setXptNumberOfInputs(savefile.value("xptNumberOfInputs").toInt());
        model->setXptNumberOfOutputs(savefile.value("xptNumberOfOutputs").toInt());
        savefile.endGroup();

    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        savefile.beginGroup("slot_"+QString::number(i));

        model->setCamType(i,savefile.value("camType").toInt());             //get min max etc...
        model->setActivePreset(i,savefile.value("activePreset").toInt());
        //model->setUsedPreset(i,savefile.value("usedPresets").toInt());
        model->setXptSlot(i);
        model->setXptSlotSource(savefile.value("xptSource").toInt());

        for (int k=0;k<NUMBER_OF_FLAGS;k++) {
            model->setCamFlag(i,k,savefile.value("flag_"+QString::number(k)).toBool());
        }

        //Set texttable pointer according to type
        model->setTextTable(i,model->getCamtype(i));

        for (int j=0;j<ROW_ENTRIES;j++) {
            model->setValue(i, ABS, j, savefile.value("value_"+QString::number(j)).toInt());

            if(send){
                //send value to cameras
                if(model->getTxCommand(j) > 0){  //there could be values without commandtypes

                    //Head Arduino seems to need pause to process certain commands
                    if(j==V_RAMP || j==V_PT_SPEED || j==V_SHUTTER || j==V_GAIN){
                        usleep(TX_T);
                    }
                    txSocket.send(model->getValue(i,ABS,V_HEADNR), model->getTxCommand(j),model->getValue(ABS,j));
                }
            }
        }
        savefile.endGroup();
    }
}

int Controller::writeSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    settingsWrite(savefile);
    return savefile.status();
}


int Controller::loadSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    settingsLoad(savefile, true);
    presetbus.setLed(PRESET_COLOR, model->getActivePreset());
    emit clearLoadButon();
    return savefile.status();
}

int Controller::writeAutosave(){
    QSettings savefile(AUTOSAVE_PATH, QSettings::NativeFormat);
    settingsWrite(savefile);
    return savefile.status();
}

int Controller::loadAutosave(){
    QSettings savefile(AUTOSAVE_PATH, QSettings::NativeFormat);
    settingsLoad(savefile,false);
    presetbus.setLed(PRESET_COLOR, model->getActivePreset());
    return savefile.status();
}



void Controller::checkSettingsRequest(int slotNr){
    std::vector<int> remainingRequests;
    remainingRequests = model->getRemainingTelegrams(slotNr);

    if(remainingRequests.empty()){
        reqSettingsTimer[slotNr].stop();
        model->setCamFlag(F_RECEIVED_ALL, true);
    }
    // if some requests still open and KNOWN flag of according slot set, Re Request
    else if(model->getCamFlag(slotNr, F_CONNECTED)){
        for (size_t i=0;i<remainingRequests.size();i++){
            int headNr = model->getValue(slotNr,ABS,V_HEADNR);
            int command = model->getTxCommand(remainingRequests[i]);
            txSocket.request(headNr, command);
            qDebug("ReRequest| Command: %d", command);
        }
    }

    //model->setRequestReceived(slotNr,)


    /*
    if(model->getRequestSettingsFlag()){

        bool anyActive = false;
        int headNr = model->getCurrReqHeadNr();
        if(cmd != TIMER_OUT){
            model->setReqPendArr(cmd,false);
            //check if all done
            for (int i=0;i<MAX_NUMBER_OF_CMDS;i++) {
                if(model->getReqPendArr(i)){
                    anyActive = true;
                }
            }
            if(!anyActive){
                model->setRequestSettingsFlag(false);
                reqSettingsTimer.stop();
                qDebug("All Requests recieved!");
            }
        }
        else{       // timer run out
            //resend still pending requests
            reqSettingsTimer.stop();
            for (int i=0;i<MAX_NUMBER_OF_CMDS;i++) {
                if(model->getReqPendArr(i)){
                    txSocket.request(headNr, i);
                    qDebug("Resend request command: %d", i);
                }
            }
            reqSettingsTimer.start();
        }
    }*/
}

void Controller::requestCameraSettings(int slot){
    int headNr = model->getValue(slot,ABS,V_HEADNR);
    int command;
    model->clearRemainingTelegrams(slot);
    if(slot == model->getActiveCameraSlot()){model->receiveAllNew();}
    for (int i=0;i<ROW_ENTRIES;i++) {
        command = model->getRequestCommand(slot,i);     //check if requestable & push, else -1
        if(command>0){
          txSocket.request(headNr, command);
        }
    }
    reqSettingsTimer[slot].start();
}


// change value on other camera slots too, if they have same headNr as current
void Controller::alignSlots(int value){
    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        if(i != model->getActiveCameraSlot()){
            if(model->getValue(ABS,V_HEADNR) == model->getValue(i, ABS,V_HEADNR)){
                model->setValue(i,ABS,value,model->getValue(ABS,value));
            }
        }
    }
}



void Controller::queueEvent(int evt){
    eventQueue.qeueEvent(evt);
}

void Controller::queueEvent(int evt, int dataA){
    std::vector<int> data;
    data.push_back(dataA);
    eventQueue.qeueEvent(evt, data);
}

void Controller::queueEvent(int evt, int dataA, int dataB){
    std::vector<int> data;
    data.push_back(dataA);
    data.push_back(dataB);
    eventQueue.qeueEvent(evt, data);
}

void Controller::queueEvent(int evt, int dataA, int dataB, int dataC){
    std::vector<int> data;
    data.push_back(dataA);
    data.push_back(dataB);
    data.push_back(dataC);
    eventQueue.qeueEvent(evt, data);
}


void Controller::queueEvent(int evt, std::vector<int> data){
    eventQueue.qeueEvent(evt, data);
}

void Controller::queueEvent(int evt, bool sta){
    eventQueue.qeueEvent(evt, sta);
}



void Controller::startQueueProcessThread(){
    usleep(800000);
    eventQueue.initCleanup();
    //poller->joystick.initRead();
    std::thread t1(&Controller::processQeue, this);       //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t1.detach();
}



void Controller::processQeue(){
    event_s loadedEvent;

    int from, type, command, data, headNr, sppPreset=0, time, slotNr;

    //struct timeval  tv1, tv2;
    model->setUpView();

    while(1){
        eventQueue.pullEvent(loadedEvent);      //blockiert, falls queue leer
//        gettimeofday(&tv1, NULL);

        switch (loadedEvent.evt) {
        case E_CLEAR:
            clear();
            logError("Counter cleared!");
            break;
        case E_INCREASE:
            int field;
            int destination;
            field=model->getRotaryField();
            destination=model->getRotaryDestination();

            if (destination==SEND) {
                if(model->getFastIris() && model->getCamtype()==6 && model->getRotaryField() == V_IRIS){
                    model->setValue(INC,field,loadedEvent.data[0]<<5);
                }else {
                    model->setValue(INC,field,loadedEvent.data[0]);     //Last Element
            }


                alignSlots(field);

                if(model->getTxCommand(field) > 0){  //there could be values without commandtypes
                   txSocket.send(model->getValue(ABS,V_HEADNR),model->getTxCommand(field),model->getValue(ABS,field));
                }
                if(field==V_HEADNR){
                    model->setCamFlag(F_KNOWN, false);
                    qDebug("KNOWN Flag cleared!");
                    model->clearRemainingTelegrams(model->getActiveCameraSlot());
                    reqSettingsTimer[model->getActiveCameraSlot()].stop();
                    model->setUpView();
                    model->updateView();
                }
            }
           else{
                switch (field) {
                case I_XPT_SOURCE:
                         model->setXptSlotSource(loadedEvent.data[0]);
                    break;
                case I_XPT_DESTINATION:
                    model->setXptDestination(loadedEvent.data[0]);
                    break;
                case I_XPT_IP_FIELD_1:
                    model->setXptIpField(INC,0,loadedEvent.data[0]);

                    break;
                case I_XPT_IP_FIELD_2:
                    model->setXptIpField(INC,1,loadedEvent.data[0]);

                    break;
                case I_XPT_IP_FIELD_3:
                    model->setXptIpField(INC,2,loadedEvent.data[0]);

                    break;
                case I_XPT_IP_FIELD_4:
                    model->setXptIpField(INC,3,loadedEvent.data[0]);

                    break;
                default:
                    break;

                }


            }

            break;
        case E_SET_TILT:
            int x,y;
            x = loadedEvent.data[0];
            if(model->getCamFlag(F_X_INVERT)){x=10000-x;}
            y = loadedEvent.data[1];

            if(model->getCamFlag(F_Y_INVERT)){y=10000-y;}

            setAxis(x,y);

            //creep prevention
            if(x==5000 && y==5000){
                usleep(CREEP_T);
            }

            txSocket.send(model->getValue(ABS,V_HEADNR), TILT_PAN, x, y);

            if(model->getCamFlag(F_SPP_ON)){
                queueEvent(E_SPP_ABORT);
            }

            //debug
            qDebug("Axis: %d/%d",x,y);
            break;

         case E_SET_ZOOM:
            int z;
            z=loadedEvent.data[0];
            if(!model->getCamFlag(F_Z_INVERT)){z = 254 - z;}


            //creep prevention
            if(z==127){
                usleep(CREEP_T);
            }
            txSocket.send(model->getValue(ABS,V_HEADNR),ZOOM_FOCUS_SET, z);

            if(model->getCamFlag(F_BOUNCING) ){
                model->setCamFlag(F_BOUNCING,false);
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Stop It
            }
            if (model->getCamFlag(F_BOUNCE_ENABLE) && !model->getCamFlag(F_BOUNCE_ENABLE)) {
               model->setCamFlag(F_BOUNCE_ABORTED,true);
            }
            if(model->getCamFlag(F_SPP_ON)){
                queueEvent(E_SPP_ABORT);
            }

            //debug
//            int z;
//            z=loadedEvent.data[0];
//            qDebug("Zoom: %d",z);

            break;
        case E_FOCUS_CHANGE:
            int focus;
            focus=loadedEvent.data[0];
            if(model->getCamFlag(F_FOCUSINVERT)){focus=(-focus);}
            model->setValue(INC, V_FOCUS, focus);
            txSocket.send(model->getValue(ABS,V_HEADNR), FOCUS_SET_ABSOLUTE, model->getValue(ABS, V_FOCUS));
            qDebug("FOCUS: %d", model->getValue(ABS,V_FOCUS));
            break;
        case E_AUTOFOCUS:
            txSocket.send(model->getValue(ABS,V_HEADNR), SET_FOCUS_PUSH);
            //debug
            //txSocket.request(1,ZOOM_FOCUS_SET);
            qDebug("AUTOFOCUS!");
            break;
        case E_AUTOFOCUS_ANSWER:
            model->setValue(ABS,V_FOCUS,loadedEvent.data[0]);
            break;
        case E_TX_SERV_WATCHDOG:
            txSocket.send(SERVER, WATCHDOG);
            model->setWatchdogWaitingflag(true);

            //set all cameraWaitingFlags
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                model->setCameraWaitingflag(i,true);
            }
            //qDebug("TX WATCHDOG");

            break;
        case E_RX_SERV_WATCHDOG:
            model->setWatchdogWaitingflag(false);   //clear flag
            break;
        case E_REQ_TEST:
            txSocket.request(1, FOCUS_SET_ABSOLUTE);
            logError("Sending Request!");
            break;
        case E_STORE_PRESET_DEBUG:
            txSocket.send(1, STORE_PRESET, loadedEvent.data[0]);
            logError("Store Preset!");
            break;
        case E_GOTO_PRESET:
            txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1,model->getValue(ABS,V_TRANS_SPEED));
            break;
        case E_STORE_PRESET:
            presetbus.setLed(ACT_PRESET_COLOR,model->getActivePreset());
            model->setCamFlag(F_PRST_IN_STORE,TRUE);
            presetbus.showStored(model->getUsedPreset(),model->getActivePreset());
            // todo: presetbus.blink(1);
            break;
        case E_PRESET_CHANGE:
        {
            int previousPreset;
            previousPreset=model->getActivePreset();
            model->setActivePreset(loadedEvent.data[0]);

            if(model->getCamFlag(F_SPP_ON)){
                queueEvent(E_SPP_ABORT);
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1,model->getValue(ABS,V_TRANS_SPEED));
                break;
            }

            if(model->getCamFlag(F_PRST_IN_STORE)){
                model->setUsedPreset(model->getActivePreset());
                model->setCamFlag(F_PRST_IN_STORE,FALSE);
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                txSocket.send(model->getValue(ABS,V_HEADNR), STORE_PRESET, model->getActivePreset()+1);
                break;
            }

            if(model->getCamFlag(F_BOUNCE_ENABLE) && !model->getCamFlag(F_BOUNCING)){
                if(previousPreset==model->getActivePreset()){
                    presetbus.setLed(CAMERA_COLOR,model->getActivePreset());
                    if (model->getCamFlag(F_BOUNCE_ABORTED)) {
                        txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Restart it
                        model->setCamFlag(F_BOUNCE_ABORTED,false);
                    }
                    else {
                        txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_TELE_SET);    //TeleSet
                        txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Start it
                    }

                    model->setCamFlag(F_BOUNCING,true);
                    blinkTimer.start();
                    break;
                }

            }

            if((model->getCamFlag(F_BOUNCING))){
                model->setCamFlag(F_BOUNCING,false);
                bool anyBouncingActive = false;
                    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                        if (model->getCamFlag(i,F_BOUNCING)) {
                            anyBouncingActive = true;
                        }
                       }
                    if(!anyBouncingActive){
                        blinkTimer.stop();
                    }
              if(previousPreset==model->getActivePreset()){
                    presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                    txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED));  //Stop It
                    model->setCamFlag(F_BOUNCE_ABORTED,false);
              }

            }

            if(model->getCamFlag(F_FASTTRANS)){
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1,1);
                break;
            }
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1,model->getValue(ABS,V_TRANS_SPEED));
            break;


        }

        /*
            if(model->getCamFlag(F_SPP_ON) == false){                   // preset change during spp -> do nothing, but abort
                presetbus.setLed(PRESET_COLOR,loadedEvent.data[0]);
                previousPreset=model->getActivePreset();
                model->setActivePreset(loadedEvent.data[0]);

                if(model->getCamFlag(F_PRST_IN_STORE)){
                    model->setUsedPreset(loadedEvent.data[0]);
                    if (model->getCamFlag(F_BOUNCE_ENABLE)) {
                        txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_TELE_SET);
                    }
                    model->setCamFlag(F_PRST_IN_STORE,FALSE);
                    txSocket.send(model->getValue(ABS,V_HEADNR), STORE_PRESET, loadedEvent.data[0]+1);
                }
                else{
                    if(!(model->getCamFlag(F_FASTTRANS))){
                        txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1,model->getValue(ABS,V_TRANS_SPEED));

                    }
                    else {
                        txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1,1); // fastets possible transission

                    }

                }

                if(model->getCamFlag(F_BOUNCE_ENABLE) && !(model->getCamFlag(F_BOUNCING))){
                    if(previousPreset==model->getActivePreset()){
                        presetbus.setLed(CAMERA_COLOR,loadedEvent.data[0]);
                        txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED));
                        model->setCamFlag(F_BOUNCING,true);
                        blinkTimer.start();
                    }
                }
                else{
                    model->setCamFlag(F_BOUNCING,false);

                    //only stop when all bouncing flags cleared
                    bool anyBouncingActive = false;
                    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                        if (model->getCamFlag(i,F_BOUNCING)) {
                            anyBouncingActive = true;
                        }
                    }
                    if(!anyBouncingActive){
                        blinkTimer.stop();
                    }
                }
            }
            else{
                queueEvent(E_SPP_ABORT);
            }
            break;
        */
        case E_EXT_PRESET_CHANGE:
        {
            from = loadedEvent.data[0];
            data = loadedEvent.data[1];
            qDebug()<<"HEAD Nr:"<<from<<"Preset:"<<data;
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(model->getValue(i,ABS,V_HEADNR) == from ){
                    if(!(i == model->getActiveCameraSlot())){
                        model->setActivePreset(i,data-1);
                        if(model->getCamFlag(i,F_BOUNCING)){
                            model->setCamFlag(i,F_BOUNCING,false);
                        }
                        if (model->getCamFlag(i,F_SPP_ON)) {
                            model->setCamFlag(i,F_SPP_ON,false);
                        }

                    }
                    else {
                        queueEvent(E_PRESET_CHANGE,data-1);
                    }
                }

            }
            break;

        }
        case E_CAMERA_CHANGE:

            camerabus.setLed(CAMERA_COLOR,loadedEvent.data[0]);
            model->setActiveCameraSlot(loadedEvent.data[0]);
            model->setUpView();
            model->updateView();

            if (model->getCamFlag(F_SPP_ON)){
                presetbus.setLed(SPP_COLOR, model->getActivePreset());
            }
            else if(model->getCamFlag(F_BOUNCING)){
                presetbus.setLed(CAMERA_COLOR,model->getActivePreset());
            }
            else{
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            }

            if(model->getXptEnabled() && model->getXptConnected()){
                contr_err = xptSocket.sendChange(model->getXptSlotSource(model->getActiveCameraSlot()),model->getXptDestination());
                if (contr_err < 0) {
                    contr_err = errno;  //zwischenspeichern (muss)
                    logSystemError(contr_err, "Could not send to Xpt");
                    model->setXptConnected(false);
                }
            }
            break;

        case E_RX_CAMERA_WATCHDOG:
            from = loadedEvent.data[0];
            type = loadedEvent.data[1];
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                // camera exists -> clear cameraWaitingFlags
                if (model->getValue(i,ABS,V_HEADNR) == from) {
                    model->setCameraWaitingflag(i,false);
                }

                // respective camera exists in a slot & known flag is not set -> request camera settings
                if (model->getValue(i,ABS,V_HEADNR) == from && !model->getCamFlag(i,F_KNOWN)) {
                    //load all non requestable default settings for this cam type
                    model->setCamType(i, type);
                    //get actual settings from camera
                    requestCameraSettings(i);

                    model->setCamFlag(i,F_KNOWN,true);
                    model->setUpView();     //grays out ui buttons for not supported camera settings, set yellow for requests
                    model->updateView();
                    //qDebug("KNOWN Flag Set, requested camera settings");
                }
            }
            //qDebug("RX WATCHDOG");
            break;
        case E_IRIS_CHANGE:

            alignSlots(V_IRIS);
            if(model->getFastIris() && model->getCamtype()==6){
                 model->setValue(INC,V_IRIS,loadedEvent.data[0]<<3);
            }
            else {
               model->setValue(INC, V_IRIS, loadedEvent.data[0]);
            }
            txSocket.send(model->getValue(ABS,V_HEADNR), IRIS_OPEN, model->getValue(ABS,V_IRIS));
            qDebug("IRIS: %d", model->getValue(ABS,V_IRIS));
            break;
        case E_PED_CHANGE:
            model->setValue(INC, V_PED, loadedEvent.data[0]);
            alignSlots(V_PED);
            txSocket.send(model->getValue(ABS,V_HEADNR), MASTER_PED_UP, model->getValue(ABS,V_PED));
            qDebug("PED: %d", model->getValue(ABS,V_PED));
            break;
        case E_WBLUE_CHANGE:
            model->setValue(INC, V_W_BLUE, loadedEvent.data[0]);
            alignSlots(V_W_BLUE);
            txSocket.send(model->getValue(ABS,V_HEADNR), BLUE_GAIN_ADJ_UP, model->getValue(ABS,V_W_BLUE));
            qDebug("WBLUE: %d", model->getValue(ABS,V_W_BLUE));
            break;
        case E_WRED_CHANGE:
            model->setValue(INC, V_W_RED, loadedEvent.data[0]);
            alignSlots(V_W_RED);
            txSocket.send(model->getValue(ABS,V_HEADNR), RED_GAIN_ADJ_UP, model->getValue(ABS,V_W_RED));
            qDebug("WRED: %d", model->getValue(ABS,V_W_RED));
            break;
        case E_BBLUE_CHANGE:
            model->setValue(INC, V_B_BLUE, loadedEvent.data[0]);
            alignSlots(V_B_BLUE);
            txSocket.send(model->getValue(ABS,V_HEADNR), BLUE_PED_UP, model->getValue(ABS,V_B_BLUE));
            qDebug("BBLUE: %d", model->getValue(ABS,V_B_BLUE));
            break;
        case E_BRED_CHANGE:
            model->setValue(INC, V_B_RED, loadedEvent.data[0]);
            alignSlots(V_B_RED);
            txSocket.send(model->getValue(ABS,V_HEADNR), RED_PED_UP, model->getValue(ABS,V_B_RED));
            qDebug("BRED: %d", model->getValue(ABS,V_B_RED));
            break;
        case E_GAIN_CHANGE:
            model->setValue(INC, V_GAIN, loadedEvent.data[0]);
            alignSlots(V_GAIN);
            txSocket.send(model->getValue(ABS,V_HEADNR), CAMERA_GAIN_UP, model->getValue(ABS,V_GAIN));
            qDebug("GAIN: %d", model->getValue(ABS,V_GAIN));
            break;
        case E_SHUTTER_CHANGE:
            model->setValue(INC, V_SHUTTER, loadedEvent.data[0]);
            alignSlots(V_SHUTTER);
            txSocket.send(model->getValue(ABS,V_HEADNR), SHUTTER_UP, model->getValue(ABS,V_SHUTTER));
            qDebug("SHUTTER: %d, MIN: %d, MAX: %d", model->getValue(ABS,V_SHUTTER), model->getMin(V_SHUTTER),model->getMax(V_SHUTTER));
            break;
        case E_WRITE_SAVEFILE:
            writeSavefile();
            //txSocket.request(3,MASTER_PED_UP); //debug misuse of button
            qDebug("wrote savefile");
            break;
        case E_LOAD_SAVEFILE:
            loadSavefile();
            qDebug("loaded savefile");
            break;
        case E_WRITE_AUTOSAVE:
            writeAutosave();
            qDebug("wrote autosave");
            break;
        case E_LOAD_AUTOSAVE:
            loadAutosave();
            qDebug("loaded autosave");
            break;
        case E_WIDESET:
            txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_WIDE_SET);
            break;
        case E_CAMERA_ANSWER:
            from = loadedEvent.data[0];
            command = loadedEvent.data[1];
            data = loadedEvent.data[2];

            //writing answer values from camera to respective properties in model, if headnumber exists
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(model->getValue(i,ABS,V_HEADNR) == from){

                    if(model->getValueFromBBMCommand(command) > 0){  //there could be commandtypes without values
                       model->setValue(i, ABS, model->getValueFromBBMCommand(command), data);
                       model->setRequestReceived(i,model->getValueFromBBMCommand(command));
//                       if(!model->getCamFlag(i,F_RECEIVED_ALL)){
//                         checkSettingsRequest(i);
//                       }
                    }
                    qDebug("ANSWER | command: %d, value: %d", command, data);
                }
            }
            break;
        case E_RX_ADJ_RCP_CMD:
            from = loadedEvent.data[0];         //to which camera head the cmd was sent
            command = loadedEvent.data[1];
            data = loadedEvent.data[2];

            //update model, if headnumber exists in slots
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(model->getValue(i,ABS,V_HEADNR) == from){

                    if(model->getValueFromBBMCommand(command) > 0){  //there could be commandtypes without values
                       model->setValue(i, ABS, model->getValueFromBBMCommand(command), data);
                    }
                    qDebug("ADJ RCP | command: %d, value: %d", command, data);
                }
            }
            break;
        case E_SETUP_HEAD:
            loadAutosave();       //todo: case if empty file or not exist

            //send PT SPEED , RAMP values to Head, request all camera settings
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                txSocket.send(model->getValue(i,ABS,V_HEADNR), RAMP, model->getValue(i,ABS,V_RAMP));
                usleep(TX_T);
                txSocket.send(model->getValue(i,ABS,V_HEADNR), PAN_TILT_SPEED, model->getValue(i,ABS,V_PT_SPEED));
                usleep(TX_T);
                requestCameraSettings(i);
            }
            model->updateView();
            qDebug("HEAD init done");
            break;
        case E_CALIB_HEAD:
            txSocket.send(model->getValue(ABS,V_HEADNR),CALIBRATE_HEAD);
            break;
        case E_SET_LOWER_LIMIT:
            txSocket.send(model->getValue(ABS,V_HEADNR),TILT_LOWER_LIMIT);
            break;
        case E_SET_UPPER_LIMIT:
            txSocket.send(model->getValue(ABS,V_HEADNR),TILT_UPPER_LIMIT);
            break;
        case E_CLEAR_LIMIT:
            txSocket.send(model->getValue(ABS,V_HEADNR),TILT_CLEAR_LIMIT);
            break;
        case E_BOUNCE_BLINK:
            if(model->getCamFlag(F_BOUNCING)){
                if(model->toggleBlink()){
                    presetbus.setLed(PRESET_COLOR, model->getActivePreset());
                }
                else{
                    presetbus.setLed(0,0,0,model->getActivePreset());
                }
            }
            break;

        case E_XPT_CONNECT:
            if(model->getXptEnabled()){
                contr_err = xptSocket.init(XptInterface::XptType(model->getXptType()),model->getXptIpAdress());
                if(contr_err<0){
                    contr_err = errno;  //zwischenspeichern (muss)
                    logSystemError(contr_err, "Could not initialize xpt-interface");
                }
                contr_err = xptSocket.connectToXpt();
                if(contr_err < 0){
                    contr_err = errno;
                    logSystemError(contr_err, "Could not connect to Xpt");
                    model->updateXptEnableStatus(false); //initial connection failed
                    model->setXptConnected(false);
                }else {
                    model->setXptConnected(true);
                    xptWatchdog.start();
                    }
               /* if(contr_err){
                    model->setXptNumberOfInputs(xptSocket.getNumberOfInputs());
                    model->setXptNumberOfOutputs(xptSocket.getNumberOfOutputs());
                    model->setXptInputLables(xptSocket.getInputLabels());
                    model->setXptOutputLables(xptSocket.getOutputLabels());
                    model->updateView();
                }*/

            }
           else{
            contr_err = xptSocket.disconnect();
            if(contr_err < 0){
                contr_err = errno;  //zwischenspeichern (muss)
                logSystemError(contr_err, "Could not disconnect xpt");
            }
                model->setXptConnected(false);
                xptWatchdog.stop();
            }
            break;

        case E_XPT_WATCHDOG:
            if(model->getXptEnabled())
            {
                int connection;
                connection = xptSocket.checkConnection();
                if (connection <= 0 ) {
                    xptConnectionAttempts++;
                    xptSocket.disconnect();
                    if (xptSocket.connectToXpt() < 0) {
                        model->setXptConnected(false);
                    }
                    if(xptConnectionAttempts > 20) {
                        xptConnectionAttempts=0;
                        model->setXptEnabled(false);
                        model->updateXptEnableStatus(false);
                        xptWatchdog.stop();
                        break;
                    }
                     //model->setXptConnected(false);
                   qDebug()<<"no ACK received";

                }
                else if(connection == 1 && model->getXptConnected()==false) {
                    model->setXptConnected(true);
                    break;
                }


            }
            break;


        case E_SPP_START:
            // only from active slot
            from = model->getActiveCameraSlot();        //slotNr
            headNr = model->getValue(ABS,V_HEADNR);
            sppPreset = model->getValue(ABS,V_SPP1);
            time = model->getValue(ABS,V_SPP_WAIT_TIME);

            // set states
            model->setCamFlag(from, F_SPP_ON, true);
            model->setSppState(from, S_SPP_GOTO1);

            // actions
            if(time==0){
                sppTimer[model->getActiveCameraSlot()].setInterval(1*10000);
            }
            else{
                sppTimer[model->getActiveCameraSlot()].setInterval(time*1000*1000);
            }
            txSocket.send(headNr, GOTO_PRESET, sppPreset+1,model->getValue(ABS,V_TRANS_SPEED));
            model->setActivePreset(sppPreset);
            presetbus.setLed(SPP_COLOR, sppPreset);
            // wait for response
            //qDebug("SPP START");
            break;

        case E_PRESET_REACHED:
        {
            // from anywhere
            headNr = loadedEvent.data[0];
            int presetNr = loadedEvent.data[1];
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(headNr == model->getValue(i,ABS,V_HEADNR) ){      //if sender head matches any current slot head & spp flag is set
                   model->setCamFlag(i,F_PRESET_MOVE,false);
                    qDebug()<<"reached";
                   if(model->getCamFlag(i,F_SPP_ON)){
                    //qDebug("SPP POS REACHED");
                    //set state
                    switch (model->getSppState(i)) {
                    case S_SPP_GOTO1:
                        if(presetNr == model->getValue(OFFSET,V_SPP1)){
                            model->setSppState(i, S_SPP_WAIT1);
                        }
                        break;
                    case S_SPP_GOTO2:
                        if(presetNr == model->getValue(OFFSET,V_SPP2)){
                            model->setSppState(i, S_SPP_WAIT2);
                        }
                        break;
                    default:
                        break;
                    }

                    //actions
                    sppTimer[i].start();
                    //qDebug("STARTED SPP TIMER %d",i);
                }}
            }

            break;
        }
        case E_SPP_WAIT_DONE:
            //qDebug("SPP TIMER ELAPSED");
            // from anywhere
            from = loadedEvent.data[0];                         // slotNr
            headNr = model->getValue(from,ABS,V_HEADNR);

            //set state
            switch (model->getSppState(from)) {
            case S_SPP_WAIT1:
                model->setSppState(from, S_SPP_GOTO2);
                sppPreset = model->getValue(from,ABS,V_SPP2);
                break;
            case S_SPP_WAIT2:
                model->setSppState(from, S_SPP_GOTO1);
                sppPreset = model->getValue(from,ABS,V_SPP1);
                break;
            default:
                break;
            }

            //action
            sppTimer[from].stop();
            txSocket.send(headNr, GOTO_PRESET, sppPreset+1, model->getValue(ABS,V_TRANS_SPEED) );
            model->setActivePreset(from, sppPreset);
            if(from == model->getActiveCameraSlot()){       // only set button led if according slot is active
                presetbus.setLed(SPP_COLOR, sppPreset);
            }
            // wait for response
            //qDebug("SPP GOTO NEXT PRESET");
            break;

        case E_SPP_ABORT:
            // only from current slot
            from = model->getActiveCameraSlot();        //slotNr

            //actions
            sppTimer[from].stop();
            presetbus.setLed(PRESET_COLOR, model->getActivePreset());

            //set states
            model->setCamFlag(from, F_SPP_ON, false);
            model->setSppState(from, S_SPP_IDLE);
            //qDebug("SPP ABORT");
            break;
        case E_REQUEST_SETTINGS:
            requestCameraSettings(model->getActiveCameraSlot());
            break;
        case E_REQ_SETTINGS_TIMER:
            slotNr = loadedEvent.data[0];
            checkSettingsRequest(slotNr);
            //qDebug("Req Timer Out");
            break;

        case E_FAST_IRIS:
            if(model->getCamtype()==6 && model->getRotaryField()==V_IRIS){
            if (model->getFastIris() == true) {
                model->setFastIris(false);
            }
            else {
                model->setFastIris(true);
            }
            }
            else {
                if (model->getCamFlag(F_PRST_IN_STORE)) {
                    model->clearUsedPresets();
                    presetbus.showStored(0,model->getActivePreset());
                }
            }
            break;

        default:
            break;
        }
//        gettimeofday(&tv2, NULL);
//        qDebug("Work: %f seconds",
//             (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//             (double) (tv2.tv_sec - tv1.tv_sec));
    }
}
