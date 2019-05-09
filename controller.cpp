#include "controller.h"
#include "model.h"
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



Controller::Controller(Model& model)// : poller(*this)    //poller Konstruktor aufrufen -> erwartet Objekt (as reference) darum this dereferenzieren
{
    setModel(model);
    contr_err = txSocket.init(9000);
    if(contr_err<0){
        contr_err = errno;  //zwischenspeichern (muss)
        logSystemError(contr_err, "Could not initialize udp-interface");
    }

    contr_err = xptSocket.init(9990,"192.168.0.30");
    if(contr_err<0){
        contr_err = errno;  //zwischenspeichern (muss)
        logSystemError(contr_err, "Could not initialize xpt-interface");
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
    blinkTimer.init(500*1000,E_BOUNCE_BLINK,*this);
    queueEvent(E_SETUP_HEAD);
}

void Controller::setModel(Model &model){
    this->model = &model;
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


int Controller::writeSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    savefile.clear();
    int currSlot = model->getActiveCameraSlot();

    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        savefile.beginGroup("slot_"+QString::number(i));

        savefile.setValue("camType",model->getCamtype(i));
        savefile.setValue("activePreset",model->getActivePreset(i));
        savefile.setValue("usedPresets",model->getUsedPreset(i));
        for (int k=0;k<NUMBER_OF_FLAGS;k++) {
            savefile.setValue("flag_"+QString::number(k),model->getCamFlag(i,k));
        }
        for (int j=0;j<ROW_ENTRIES;j++) {
            savefile.setValue("value_"+QString::number(j),model->getValue(i,ABS,j));
        }
        savefile.endGroup();
    }
    savefile.sync();

    model->setActiveCameraSlot(currSlot);
    return savefile.status();
}


int Controller::loadSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    int currSlot = model->getActiveCameraSlot();

    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        savefile.beginGroup("slot_"+QString::number(i));

        model->setCamType(i,savefile.value("camType").toInt());
        model->setActivePreset(i,savefile.value("activePreset").toInt());
        model->setUsedPreset(i,savefile.value("usedPresets").toInt());

        for (int k=0;k<NUMBER_OF_FLAGS;k++) {
            model->setCamFlag(i,k,savefile.value("flag_"+QString::number(k)).toBool());
        }

        //Set texttable pointer according to type
        model->setTextTable(i,model->getCamtype(i));

        for (int j=0;j<ROW_ENTRIES;j++) {
            model->setValue(i, ABS, j, savefile.value("value_"+QString::number(j)).toInt());

            //send value to camera
            if(model->getTxCommand(j) > 0){  //there could be values without commandtypes
               txSocket.send(model->getValue(i,ABS,V_HEADNR), model->getTxCommand(j),model->getValue(ABS,j));
            }
        }
        savefile.endGroup();
    }

    model->setActiveCameraSlot(currSlot);
    presetbus.setLed(PRESET_COLOR, model->getActivePreset());
    return savefile.status();
}


void Controller::requestCameraSettings(){
    txSocket.request(model->getValue(ABS,V_HEADNR), CAMERA_GAIN_UP);
    txSocket.request(model->getValue(ABS,V_HEADNR), SHUTTER_UP);
    //ND_FILTER
    //DETAIL
    txSocket.request(model->getValue(ABS,V_HEADNR), RED_GAIN_ADJ_UP);
    txSocket.request(model->getValue(ABS,V_HEADNR), BLUE_GAIN_ADJ_UP);
    //B_RED
    //B_BLUE
    txSocket.request(model->getValue(ABS,V_HEADNR), MASTER_PED_UP);
    txSocket.request(model->getValue(ABS,V_HEADNR), IRIS_OPEN);
    txSocket.request(model->getValue(ABS,V_HEADNR), COLOR_UP);
    txSocket.request(model->getValue(ABS,V_HEADNR), WHITE_BALANCE_PRST);
    //KNEE
    //KNEE_POINT
    //GAMMA
    txSocket.request(model->getValue(ABS,V_HEADNR), GAMMA_TABLE);
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
    std::thread t1(&Controller::processQeue, this);       //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    t1.detach();
}


////debug:
//loadSavefile();
//qDebug("load savefile");

void Controller::processQeue(){
    event_s loadedEvent;
    int from, type, command, data;
    //struct timeval  tv1, tv2;


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
                model->setValue(INC,field,loadedEvent.data[0]);     //Last Element

                if(model->getTxCommand(field) > 0){  //there could be values without commandtypes
                   txSocket.send(model->getValue(ABS,V_HEADNR),model->getTxCommand(field),model->getValue(ABS,field));
                }
                if(field==V_HEADNR){
                    model->setCamFlag(F_KNOWN, false);
                    qDebug("KNOWN Flag cleared!");
                }
            }
           else{
                switch (field) {
                case V_XPT_SOURCE:
                         model->setValue(model->getXptSlot(),INC,field,loadedEvent.data[0]);
                    break;
                case XPT_DESTINATION:
                    model->setXptDestination(loadedEvent.data[0]);
                    break;
                case XPT_IP_FIELD_1:
                    model->setXptIpField(0,loadedEvent.data[0]);
                    xptSocket.changeIP(model->getXptIpAdress());
                    break;
                case XPT_IP_FIELD_2:
                    model->setXptIpField(1,loadedEvent.data[0]);
                    xptSocket.changeIP(model->getXptIpAdress());
                    break;
                case XPT_IP_FIELD_3:
                    model->setXptIpField(2,loadedEvent.data[0]);
                    xptSocket.changeIP(model->getXptIpAdress());
                    break;
                case XPT_IP_FIELD_4:
                    model->setXptIpField(3,loadedEvent.data[0]);
                    xptSocket.changeIP(model->getXptIpAdress());
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

            txSocket.send(model->getValue(ABS,V_HEADNR), TILT_PAN, x, y);
            if(model->getCamFlag(F_BOUNCING)){
                model->setCamFlag(F_BOUNCING,false);
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            }
            //debug
            //char str[100];
            //sprintf(str,"%d/%d",x,y);
            //qDebug() << "Axis: " << str;

            break;
         case E_SET_ZOOM:
            int z;
            z=loadedEvent.data[0];
            if(model->getCamFlag(F_Z_INVERT)){z=254-z;}
            txSocket.send(model->getValue(ABS,V_HEADNR),ZOOM_FOCUS_SET, z);
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
            txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]);
            break;
        case E_STORE_PRESET:
            presetbus.setLed(ACT_PRESET_COLOR,model->getActivePreset());
            model->setCamFlag(F_PRST_IN_STORE,TRUE);
            presetbus.showStored(model->getUsedPreset(),model->getActivePreset());
            presetbus.blink(1);
            break;
        case E_PRESET_CHANGE:
            int previousPreset;
            presetbus.setLed(PRESET_COLOR,loadedEvent.data[0]);
            previousPreset=model->getActivePreset();
            model->setActivePreset(loadedEvent.data[0]);

            if(model->getCamFlag(F_PRST_IN_STORE)){
                model->setUsedPreset(loadedEvent.data[0]);
                model->setCamFlag(F_PRST_IN_STORE,FALSE);
                txSocket.send(model->getValue(ABS,V_HEADNR), STORE_PRESET, loadedEvent.data[0]+1);
            }
            else{
                txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1);
            }

            if(model->getCamFlag(F_BOUNCE_ENABLE) && !(model->getCamFlag(F_BOUNCING))){
                if(previousPreset==model->getActivePreset()){
                    presetbus.setLed(CAMERA_COLOR,loadedEvent.data[0]);
                    txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_TELE_SET);
                    txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_START);
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
            break;
        case E_CAMERA_CHANGE:
            camerabus.setLed(CAMERA_COLOR,loadedEvent.data[0]);
            model->setActiveCameraSlot(loadedEvent.data[0]);
            if(!(model->getCamFlag(F_BOUNCING))){
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            }
            else{
                presetbus.setLed(CAMERA_COLOR,model->getActivePreset());
            }
            if(model->getXptConnected()){
                contr_err = xptSocket.sendChange(model->getValue(ABS,V_XPT_SOURCE),model->getXptDestination());
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
                if (model->getValue(i,ABS,V_HEADNR) == from && !model->getCamFlag(i,F_KNOWN)) {    //if headnumber == from
                    model->setCamType(i, type);
                    requestCameraSettings();
                    model->setCamFlag(i,F_KNOWN,true);
                    model->setUpView();     //grays out ui buttons for not supported camera settings
                    qDebug("KNOWN Flag Set, requested camera settings");
                    model->setUpView();
                }
            }
            //qDebug("RX WATCHDOG");
            break;
        case E_IRIS_CHANGE:
            model->setValue(INC, V_IRIS, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), IRIS_OPEN, model->getValue(ABS,V_IRIS));
            qDebug("IRIS: %d", model->getValue(ABS,V_IRIS));
            break;
        case E_PED_CHANGE:
            model->setValue(INC, V_PED, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), MASTER_PED_UP, model->getValue(ABS,V_PED));
            qDebug("PED: %d", model->getValue(ABS,V_PED));
            break;
        case E_WBLUE_CHANGE:
            model->setValue(INC, V_W_BLUE, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), BLUE_GAIN_ADJ_UP, model->getValue(ABS,V_W_BLUE));
            qDebug("WBLUE: %d", model->getValue(ABS,V_W_BLUE));
            break;
        case E_WRED_CHANGE:
            model->setValue(INC, V_W_RED, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), RED_GAIN_ADJ_UP, model->getValue(ABS,V_W_RED));
            qDebug("WRED: %d", model->getValue(ABS,V_W_RED));
            break;
        case E_BBLUE_CHANGE:
            model->setValue(INC, V_B_BLUE, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), BLUE_PED_UP, model->getValue(ABS,V_B_BLUE));
            qDebug("BBLUE: %d", model->getValue(ABS,V_B_BLUE));
            break;
        case E_BRED_CHANGE:
            model->setValue(INC, V_B_RED, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), RED_PED_UP, model->getValue(ABS,V_B_RED));
            qDebug("BRED: %d", model->getValue(ABS,V_B_RED));
            break;
        case E_GAIN_CHANGE:
            model->setValue(INC, V_GAIN, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), CAMERA_GAIN_UP, model->getValue(ABS,V_GAIN));
            qDebug("GAIN: %d", model->getValue(ABS,V_GAIN));
            break;
        case E_SHUTTER_CHANGE:
            model->setValue(INC, V_SHUTTER, loadedEvent.data[0]);
            txSocket.send(model->getValue(ABS,V_HEADNR), SHUTTER_UP, model->getValue(ABS,V_SHUTTER));
            qDebug("SHUTTER: %d, MIN: %d, MAX: %d", model->getValue(ABS,V_SHUTTER), model->getMin(V_SHUTTER),model->getMax(V_SHUTTER));
            break;
        case E_WRITE_SAVEFILE:
            writeSavefile();
            qDebug("wrote savefile");
            break;
        case E_LOAD_SAVEFILE:
            loadSavefile();
            qDebug("loaded savefile");
            break;
        case E_FAST_TRANS:
            if(model->getCamFlag(F_FASTTRANS)){
                //send... nicht sicher ob es ein befehl geben sollte
            }
            else {
                //same
            }
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
                    model->setValue(i, ABS, model->getValueFromBBMCommand(command), data);
                    qDebug("ANSWER | command: %d, value: %d", command, data);
                }
            }
            break;
        case E_SETUP_HEAD:
            //send init values to head
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                txSocket.send(model->getValue(i,ABS,V_HEADNR), RAMP, model->getValue(i,ABS,V_RAMP));
                txSocket.send(model->getValue(i,ABS,V_HEADNR), PAN_TILT_SPEED, model->getValue(i,ABS,V_PT_SPEED));
            }
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
        case E_BOUNCE_BLINK:
            if(model->getCamFlag(F_BOUNCING)){
                if(model->toggleBlink()){
                    presetbus.setLed(PRESET_COLOR, model->getActiveCameraSlot());
                }
                else{
                    presetbus.setLed(0,0,0,model->getActiveCameraSlot());
                }
            }
            break;
        case E_XPT_CONNECT:
            if(model->getXptConnected()){
                contr_err = xptSocket.connectToXpt();
                if(contr_err < 0){
                    contr_err = errno;  //zwischenspeichern (muss)
                    logSystemError(contr_err, "Could not connect to Xpt");
                    model->setXptConnected(false);
                }
                model->updateXptConnectionStatus(model->getXptConnected());
            }
           else{
            contr_err = xptSocket.disconnect();
            if(contr_err < 0){
                contr_err = errno;  //zwischenspeichern (muss)
                logSystemError(contr_err, "Could not disconnect xpt");
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
