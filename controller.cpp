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
    int currCamera = model->getActiveCamera();

    for (int i=1;i<=NUMBER_OF_CAMERAS;i++) {
        model->setActiveCamera(i);
        savefile.beginGroup("camera_"+QString::number(i));

        for (int j=0;j<ROW_ENTRIES;j++) {
            savefile.setValue("value_"+QString::number(j),model->getValue(ABS,j));
        }
        savefile.endGroup();
    }
    savefile.sync();

    model->setActiveCamera(currCamera);
    return savefile.status();
}


int Controller::loadSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    int currCamera = model->getActiveCamera();

    for (int i=1;i<=NUMBER_OF_CAMERAS;i++) {
        model->setActiveCamera(i);
        savefile.beginGroup("camera_"+QString::number(i));

        for (int j=0;j<ROW_ENTRIES;j++) {
            model->setValue(ABS, j, savefile.value("value_"+QString::number(j)).toInt());

            //send value to camera
            if(model->getTxCommand(j) > 0){  //there could be values without commandtypes
               txSocket.send(i,model->getTxCommand(j),model->getValue(ABS,j));
            }
        }
        savefile.endGroup();
    }

    model->setActiveCamera(currCamera);
    return savefile.status();
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
            field=model->getRotaryField();
            model->setValue(INC,field,loadedEvent.data[0]);     //Last Element

            if(model->getTxCommand(field) > 0){  //there could be values without commandtypes
               txSocket.send(model->getActiveCamera(),model->getTxCommand(field),model->getValue(ABS,field));
            }
            if(field==V_HEADNR){
                model->setUpView();
            }

            break;
        case E_SET_TILT:
            int x,y;
            x = loadedEvent.data[0];
            y = loadedEvent.data[1];
            setAxis(x,10000-y);
            txSocket.send(model->getActiveCamera(), TILT_PAN, x, y);

            //debug
            //char str[100];
            //sprintf(str,"%d/%d",x,y);
            //qDebug() << "Axis: " << str;

            break;
         case E_SET_ZOOM:
            txSocket.send(model->getActiveCamera(),ZOOM_FOCUS_SET, loadedEvent.data[0]);
            break;
        case E_FOCUS_CHANGE:
            model->setValue(INC, V_FOCUS, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), FOCUS_SET_ABSOLUTE, model->getValue(ABS, V_FOCUS));
            qDebug("FOCUS: %d", model->getValue(ABS,V_FOCUS));
            break;
        case E_AUTOFOCUS:
            txSocket.send(model->getActiveCamera(), SET_FOCUS_PUSH);
            qDebug("AUTOFOCUS!");
            break;
        case E_AUTOFOCUS_ANSWER:
            model->setValue(ABS,V_FOCUS,loadedEvent.data[0]);
            break;
        case E_TX_WATCHDOG:
            txSocket.send(SERVER, WATCHDOG);
            model->setWatchdogWaitingflag(true);
            break;
        case E_RX_WATCHDOG:
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
            txSocket.send(model->getActiveCamera(), GOTO_PRESET, loadedEvent.data[0]);
            break;
        case E_STORE_PRESET:
            presetbus.setLed(ACT_PRESET_COLOR,model->getActivePreset());
            model->setCamFlag(PRST_IN_STORE,TRUE);
            presetbus.showStored(model->getUsedPreset(),model->getActivePreset());
            break;
        case E_PRESET_CHANGE:
            presetbus.setLed(PRESET_COLOR,loadedEvent.data[0]);
            model->setActivePreset(loadedEvent.data[0]);
            if(model->getCamFlag(PRST_IN_STORE)){
                model->setUsedPreset(loadedEvent.data[0]);
                model->setCamFlag(PRST_IN_STORE,FALSE);
                txSocket.send(model->getActiveCamera(), STORE_PRESET, loadedEvent.data[0]+1);
            }
            else{
                txSocket.send(model->getActiveCamera(), GOTO_PRESET, loadedEvent.data[0]+1);
            }
            break;
        case E_CAMERA_CHANGE:
            camerabus.setLed(CAMERA_COLOR,loadedEvent.data[0]);
            model->setActiveCamera(loadedEvent.data[0]);
            presetbus.setLed(PRESET_COLOR,model->getActivePreset());

            break;
        case E_CHECK_CAMERA_TYPE:
            int from, type;
            from = loadedEvent.data[0];
            type = loadedEvent.data[1];
            if(model->getCamtype(from) != type){
                model->setCamType(from, type);
                logError("Camera Head Type changed");
            }
            break;
        case E_IRIS_CHANGE:
            model->setValue(INC, V_IRIS, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), IRIS_OPEN, model->getValue(ABS,V_IRIS));
            qDebug("IRIS: %d", model->getValue(ABS,V_IRIS));
            break;
        case E_PED_CHANGE:
            model->setValue(INC, V_PED, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), MASTER_PED_UP, model->getValue(ABS,V_PED));
            qDebug("PED: %d", model->getValue(ABS,V_PED));
            break;
        case E_WBLUE_CHANGE:
            model->setValue(INC, V_W_BLUE, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), BLUE_GAIN_ADJ_UP, model->getValue(ABS,V_W_BLUE));
            qDebug("WBLUE: %d", model->getValue(ABS,V_W_BLUE));
            break;
        case E_WRED_CHANGE:
            model->setValue(INC, V_W_RED, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), RED_GAIN_ADJ_UP, model->getValue(ABS,V_W_RED));
            qDebug("WRED: %d", model->getValue(ABS,V_W_RED));
            break;
        case E_BBLUE_CHANGE:
            model->setValue(INC, V_B_BLUE, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), BLUE_PED_UP, model->getValue(ABS,V_B_BLUE));
            qDebug("BBLUE: %d", model->getValue(ABS,V_B_BLUE));
            break;
        case E_BRED_CHANGE:
            model->setValue(INC, V_B_RED, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), RED_PED_UP, model->getValue(ABS,V_B_RED));
            qDebug("BRED: %d", model->getValue(ABS,V_B_RED));
            break;
        case E_GAIN_CHANGE:
            model->setValue(INC, V_GAIN, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), CAMERA_GAIN_UP, model->getValue(ABS,V_GAIN));
            qDebug("GAIN: %d", model->getValue(ABS,V_GAIN));
            break;
        case E_SHUTTER_CHANGE:
            model->setValue(INC, V_SHUTTER, loadedEvent.data[0]);
            txSocket.send(model->getActiveCamera(), SHUTTER_UP, model->getValue(ABS,V_SHUTTER));
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
        default:
            break;
        }
//        gettimeofday(&tv2, NULL);
//        qDebug("Work: %f seconds",
//             (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//             (double) (tv2.tv_sec - tv1.tv_sec));
    }
}
