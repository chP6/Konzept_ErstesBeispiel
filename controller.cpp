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
#include "logging.h"
#include <QTime>

#include <chrono>
#include <ctime>
#include <sys/time.h>


/*Signal from xptSocktet 'inputLabelsChanged' to controller 'onXptLabelChanged'*/
void Controller::onXptLableChanged()
{
    /*update Model and View*/
    model->setXptNumberOfInputs(xptSocket.getNumberOfInputs());
    model->setXptNumberOfOutputs(xptSocket.getNumberOfOutputs());
    model->setXptInputLables(xptSocket.getInputLabels());
    model->setXptOutputLables(xptSocket.getOutputLabels());
    model->updateView();
}

Controller::Controller(Model& model)// : poller(*this)    //poller Konstruktor aufrufen -> erwartet Objekt (as reference) darum this dereferenzieren
{
    /*set the model for the controller*/
    setModel(model);

    /*UDP Socket initialize port 9000*/
    contr_err = txSocket.init(9000);
    if(contr_err<0){
        contr_err = errno;          //zwischenspeichern (muss)
        logSystemError(contr_err, "Could not initialize udp-interface");
    }

    /*Initialize the six buttons for the presets*/
    presetbus.setRow(UPPER_ROW);
    contr_err=presetbus.initSpi();  //SPI for the leds
    if(contr_err<0){
        contr_err = errno;
        logSystemError(contr_err, "Could not open SPI-BUS 1");
    }

    /*Initialize the six buttons for the cameras*/
    camerabus.setRow(LOWER_ROW);
    camerabus.initSpi();    //SPI for the leds
    if(contr_err<0){
        contr_err = errno;
        logSystemError(contr_err, "Could not open SPI-BUS 2");
    }

    /*Set the Leds after initialization*/
    camerabus.setLed(CAMERA_COLOR,model.getActiveCameraSlot());
    presetbus.setLed(PRESET_COLOR,model.getActivePreset());

    /*Set up a timer for the leds in the buttons to blink when necessary*/
    blinkTimer.init(E_BOUNCE_BLINK,*this);
    blinkTimer.setInterval(500*1000);

    /*Set up a watchdog timer for the xpt connection. Sends a ping to the XPT-router once it's connected*/
    xptWatchdog.init(E_XPT_WATCHDOG,*this);
    xptWatchdog.setInterval(2000*1000);

    /*Set up six timers for preiodical requests. Once a camera connects to the system it should send all of the requested values.
    The values which don't arrive, will be requested periodically, until they are here.*/
    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        reqSettingsTimer[i].init(E_REQ_SETTINGS_TIMER, i,*this);
        reqSettingsTimer[i].setInterval(1000*1000);
    }


    /*Set up six timers for the scheduled preset positioning. These timers set the wait time between the presets in seconds.*/
    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        sppTimer[i].init(E_SPP_WAIT_DONE,i,*this);
    }

    /*fire the first event in the queue*/
    queueEvent(E_SETUP_HEAD);

    /*Signal connection from xptSocktet 'inputLabelsChanged' to controller 'onXptLabelChanged'*/
    QObject::connect(&xptSocket, SIGNAL(inputLabelsChanged()),
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
/*Error information*/
void Controller::logSystemError(int err_no, std::string msg){
    model->addError("(S) " + msg + " ERRNO: " + strerror(err_no));        //msg + string aus errno nummer
}
/*Log information*/
void Controller::logError(std::string msg){
    model->addError(msg);
}
/*Clear the errors*/
void Controller::clearErrors(){
    model->clearErrors();
}
/*prepare a savefile with all necessary userdata from the model*/
void Controller::settingsWrite(QSettings &savefile){
    savefile.clear();
    /*xpt Settings*/
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
        /*all necessary data from every slot into a group called slot_'numberOftheSlot'*/
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

/*write the loaded settings to the model*/
void Controller::settingsLoad(QSettings &savefile, bool send){
    /*write according to group*/
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
        //model->setUsedPreset(i,savefile.value("usedPresets").toInt());    //better start a new session after reboot
        model->setXptSlot(i);
        model->setXptSlotSource(savefile.value("xptSource").toInt());

        for (int k=0;k<NUMBER_OF_FLAGS;k++) {
            model->setCamFlag(i,k,savefile.value("flag_"+QString::number(k)).toBool());
        }

        /*Set texttable pointer according to type*/
        model->setTextTable(i,model->getCamtype(i));

        for (int j=0;j<ROW_ENTRIES;j++) {
            model->setValue(i, ABS, j, savefile.value("value_"+QString::number(j)).toInt());

        /*Somethimes it is necessary to send certain values to the camera after reboot*/
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

/*write the file to /opt/savefile*/
int Controller::writeSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    settingsWrite(savefile);
    return savefile.status();
}

/*load the file from /opt/savefile*/
int Controller::loadSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    settingsLoad(savefile, true);
    presetbus.setLed(PRESET_COLOR, model->getActivePreset());
    emit clearLoadButon();
    return savefile.status();
}

/*write the autosavefile to /opt/autosave*/
int Controller::writeAutosave(){
    QSettings savefile(AUTOSAVE_PATH, QSettings::NativeFormat);
    settingsWrite(savefile);
    return savefile.status();
}

/*load the file from /opt/autosave*/
int Controller::loadAutosave(){
    QSettings savefile(AUTOSAVE_PATH, QSettings::NativeFormat);
    settingsLoad(savefile,false);
    presetbus.setLed(PRESET_COLOR, model->getActivePreset());
    return savefile.status();
}


/*routine to see if there are requested values that couldn't be recieved*/
void Controller::checkSettingsRequest(int slotNr){
    std::vector<int> remainingRequests;
    remainingRequests = model->getRemainingTelegrams(slotNr); //get the remaining telegrams into a vector

    /*if it's all empty, everything has been received and all good. Stop the timer for periodic requests*/
    if(remainingRequests.empty()){
        reqSettingsTimer[slotNr].stop();
        model->setCamFlag(F_RECEIVED_ALL, true);
    }
    /*if some requests still open and KNOWN flag of according slot set, Re Request*/
    else if(model->getCamFlag(slotNr, F_CONNECTED)){
        for (size_t i=0;i<remainingRequests.size();i++){
            int headNr = model->getValue(slotNr,ABS,V_HEADNR);          // get the headnumber according to the slot
            int command = model->getTxCommand(remainingRequests[i]);    //get the command to request again
            txSocket.request(headNr, command);                          //request it
            qCDebug(requestIo)<<"Rerequest| SlotNr:"<< slotNr << ", Command:" << command;
        }
    }
}

/*request all settings e.g. after startup or new connectiom or user input*/
void Controller::requestCameraSettings(int slot){
    int headNr = model->getValue(slot,ABS,V_HEADNR);
    int command;
    model->clearRemainingTelegrams(slot);
    if(slot == model->getActiveCameraSlot()){model->receiveAllNew();}
    for (int i=0;i<ROW_ENTRIES;i++) {
        command = model->getRequestCommand(slot,i);     //check if requestable & push, else -1
        if(command>0){
          txSocket.request(headNr, command);
          qCDebug(requestIo)<<"Initial request| HeadNr:"<< headNr << ", Command:" << command;
        }
    }
    reqSettingsTimer[slot].start();             //start the timer for requests that didn't arrive
    qCDebug(requestIo)<<"Request timer started| SlotNr:"<< slot;
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


/*overload functions for the queue*/
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


/*start the queue*/
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
        eventQueue.pullEvent(loadedEvent);      //blocks if queue is empty
//        gettimeofday(&tv1, NULL);

        switch (loadedEvent.evt) {
        case E_CLEAR:
            /*debugging no function*/
            clear();
            logError("Counter cleared!");
            break;
        case E_INCREASE:
            /*Signal from the Rotary Encoder for the touch and turn*/
            int field;
            int destination;
            field=model->getRotaryField();              //get the field that was pressed by the user on the touch
            destination=model->getRotaryDestination();  //get the destination for the value, SEND: can be sent right away, INTERNAL: nothing to send something else

            /*Cam Type 6 has a huge range of iris steps (0 - 4000), This requires a lot of turns with the rotary.
            Therefore there is a fast iris function*/
            if (destination==SEND) {
                if(model->getFastIris() && model->getCamtype()==6 && model->getRotaryField() == V_IRIS){
                    model->setValue(INC,field,loadedEvent.data[0]<<5);
                }else {
                    model->setValue(INC,field,loadedEvent.data[0]);     //Last Element
                       }

                alignSlots(field);

                /*send what could be sent*/
                if(model->getTxCommand(field) > 0){  //there could be values without commandtypes
                   txSocket.send(model->getValue(ABS,V_HEADNR),model->getTxCommand(field),model->getValue(ABS,field));
                }

                /*the headnumer has been changed*/
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
                /*nothing to send xpt settings */
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
            qCDebug(logicIo)<<"Touch and Turn| delta:"<< loadedEvent.data[0] << ", field:" << field;
            break;
        case E_SET_TILT:
            /*Pan and Tilt commands from the Joystick*/
            int x,y;
            x = loadedEvent.data[0];
            if(model->getCamFlag(F_X_INVERT)){x=10000-x;} //dirextion of the x axis inverted

            y = loadedEvent.data[1];
            if(model->getCamFlag(F_Y_INVERT)){y=10000-y;}  //direction of the y axis inverted

            setAxis(x,y); //debug

            /*Joystick creeps around the zero point*/
            if(x==5000 && y==5000){
                usleep(CREEP_T);
            }

            txSocket.send(model->getValue(ABS,V_HEADNR), TILT_PAN, x, y); //send pan tilt information to the camera

            /*Sceduled Preset Positioning can be aborted with the move of the joystick*/
            if(model->getCamFlag(F_SPP_ON)){
                queueEvent(E_SPP_ABORT);
                qCDebug(logicIo)<<"SPP stopped by Joystick";
            }

            //debug
            qCDebug(logicIo)<<"Joystick Pan/Tilt| x-Axis:"<< x << ", y-Axis:" << y;
            break;

         case E_SET_ZOOM:
            /*Zoom Commands from the z axis of the joystick*/
            int z;
            z=loadedEvent.data[0];
            if(!model->getCamFlag(F_Z_INVERT)){z = 254 - z;} //direction of the zoom inverted


            /*Joystick creeps around the zero point*/
            if(z==127){
                usleep(CREEP_T);
            }

            txSocket.send(model->getValue(ABS,V_HEADNR),ZOOM_FOCUS_SET, z); //send zoom information to the camera

            /*Bounce mode can be aborted with the use of the zoom*/
            if(model->getCamFlag(F_BOUNCING) ){
                model->setCamFlag(F_BOUNCING,false);
                model->setCamFlag(F_BOUNCE_ABORTED,true);
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Stop It
                qCDebug(logicIo)<<"Bounce stopped by Joystick";
            }

            /*Sceduled Preset Positioning can be aborted with the move of the joystick*/
            if(model->getCamFlag(F_SPP_ON)){
                queueEvent(E_SPP_ABORT);
                qCDebug(logicIo)<<"SPP stopped by Joystick";
            }

            //debug
            qCDebug(logicIo)<<"Joystick Zoom| z-Axis:"<< z;

            break;
        case E_FOCUS_CHANGE:
            /*Focus Command from the Rotary Encoder*/
            int focus;
            focus=loadedEvent.data[0];
            if(model->getCamFlag(F_FOCUSINVERT)){focus=(-focus);} //The direction of the focus can be inverted
            model->setValue(INC, V_FOCUS, focus);
            txSocket.send(model->getValue(ABS,V_HEADNR), FOCUS_SET_ABSOLUTE, model->getValue(ABS, V_FOCUS));
            qCDebug(logicIo)<<"Focus| delta:"<< focus << ", value:"<<model->getValue(ABS,V_FOCUS);
            break;
        case E_AUTOFOCUS:
            /*Trigger of the autofocus, send it right away*/
            txSocket.send(model->getValue(ABS,V_HEADNR), SET_FOCUS_PUSH);
            qCDebug(logicIo)<< "Autofocus pressed |";
            break;
        case E_AUTOFOCUS_ANSWER:
            /*Answer from the camera after autofocus to get the new value into the model*/
            model->setValue(ABS,V_FOCUS,loadedEvent.data[0]);
            qCDebug(rxHeadIo) << "Focus value received | value:" << loadedEvent.data[0];
            break;
        case E_TX_SERV_WATCHDOG:
            /*Watchdog to the server, periodically every second*/
            txSocket.send(SERVER, WATCHDOG);
            model->setWatchdogWaitingflag(true);    //waiting for answer

            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                model->setCameraWaitingflag(i,true);    //set all cameraWaitingFlags
            }
            qCDebug(txWatchdogIo) << "Watchdog sent to Server | Time:"<< QTime::currentTime().toString();;
            break;
        case E_RX_SERV_WATCHDOG:
            /*Watchdog answer from server*/
            model->setWatchdogWaitingflag(false);   //clear flag all good
            qCDebug(rxWatchdogIo) << "Watchdog received from Server | Time:"<< QTime::currentTime().toString();;
            break;
        case E_REQ_TEST:
            /*debug*/
            txSocket.request(1, FOCUS_SET_ABSOLUTE);
            logError("Sending Request!");
            break;
        case E_STORE_PRESET_DEBUG:
            /*debug*/
            txSocket.send(1, STORE_PRESET, loadedEvent.data[0]);
            logError("Store Preset!");
            break;
        case E_GOTO_PRESET:
            /*debug*/
            txSocket.send(model->getValue(ABS,V_HEADNR), GOTO_PRESET, loadedEvent.data[0]+1,model->getValue(ABS,V_TRANS_SPEED));
            break;
        case E_STORE_PRESET:
            /*Store preset from User Input Touchscreen*/
            presetbus.setLed(ACT_PRESET_COLOR,model->getActivePreset());
            model->setCamFlag(F_PRST_IN_STORE,TRUE);                //set flag for button push
            presetbus.showStored(model->getUsedPreset(),model->getActivePreset()); //show used presets red and unused green, active preset int PresetColor
            qCDebug(logicIo)<< "User Preset store |";
            break;
        case E_PRESET_CHANGE:
        {
            /*Push of a button on the presetbus*/
            int previousPreset;
            int headNr = model->getValue(ABS,V_HEADNR);
            int transitionSpeed = model->getValue(ABS,V_TRANS_SPEED);
            previousPreset=model->getActivePreset();        //get previous preset to detect if the same button was pushed a second time
            int activePreset = loadedEvent.data[0];
            model->setActivePreset(activePreset);    //get the new one

            /*If Scheduled preset positionin is on, abort it and go to the slected preset, then break*/
            if(model->getCamFlag(F_SPP_ON)){
                queueEvent(E_SPP_ABORT);
                presetbus.setLed(PRESET_COLOR,activePreset);
                txSocket.send(headNr, GOTO_PRESET, activePreset+1,transitionSpeed);
                qCDebug(logicIo) << "SPP aborted by key press|";
                qCDebug(presetIo) << "Goto Preset | HeadNr:" << headNr<< " ,PresetNr:" << activePreset+1 << ", Transition speed:" << transitionSpeed ;
                break;
            }

            /*User wants to store a preset, store it and break*/
            if(model->getCamFlag(F_PRST_IN_STORE)){
                model->setUsedPreset(activePreset);
                model->setCamFlag(F_PRST_IN_STORE,FALSE);
                presetbus.setLed(PRESET_COLOR,activePreset);
                txSocket.send(headNr, STORE_PRESET, activePreset+1);
                qCDebug(presetIo) << "Store Preset | HeadNr:" << headNr<< " ,PresetNr:" << activePreset+1 ;
                break;
            }

            /*Bounce mode is enabled but not started. If same button has been pressed two consecutive times, start it and break*/
            if(model->getCamFlag(F_BOUNCE_ENABLE) && !model->getCamFlag(F_BOUNCING)){
                if(previousPreset==activePreset){
                    presetbus.setLed(CAMERA_COLOR,activePreset);
                    /*TeleSet seems to be obsolete, to do: ask, reimplement, test*/
                    if (model->getCamFlag(F_BOUNCE_ABORTED)) {
                        /*If bounce was aborted, don't do a teleset, just restart it*/
                        txSocket.send(headNr,BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Restart it
                        model->setCamFlag(F_BOUNCE_ABORTED,false);
                        qCDebug(logicIo) << "Bounce restarted by key press (no TeleSet) |" << "HeadNr: " << headNr << "Zoom Speed:" << model->getValue(ABS, V_BOUNCE_ZOOM_SPEED);
                    }
                    else {
                        /*teleset and restart*/
                        txSocket.send(headNr,BNCE_ZOOM_TELE_SET);    //TeleSet
                        txSocket.send(headNr,BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Start it
                        qCDebug(logicIo) << "Bounce started by key press (TeleSet fired) |" << "HeadNr: " << headNr << "Zoom Speed:" << model->getValue(ABS, V_BOUNCE_ZOOM_SPEED);
                    }

                    model->setCamFlag(F_BOUNCING,true);
                    blinkTimer.start(); //blink the led
                    break;
                }

            }

            /*Bounce mode enabled and started*/
            if((model->getCamFlag(F_BOUNCING))){
                model->setCamFlag(F_BOUNCING,false);
                bool anyBouncingActive = false;
                /*Stop shared blink timer when nobody needs it*/
                    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                        if (model->getCamFlag(i,F_BOUNCING)) {
                            anyBouncingActive = true;
                        }
                       }
                    if(!anyBouncingActive){
                        blinkTimer.stop();
                    }
              /*same button again stop it and recall preset, therefore no break*/
              if(previousPreset==activePreset){
                    presetbus.setLed(PRESET_COLOR,activePreset);
                    txSocket.send(headNr,BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED));  //Stop It
                    model->setCamFlag(F_BOUNCE_ABORTED,false);
                    qCDebug(logicIo) << "Bounce stopped by key press|" << "HeadNr: " << headNr;

              }

            }

            /*Fast transition flag is set. Recall preset with fastest transition rate and break*/
            if(model->getCamFlag(F_FASTTRANS)){
                presetbus.setLed(PRESET_COLOR,activePreset);
                txSocket.send(headNr, GOTO_PRESET, activePreset+1,1);
                qCDebug(presetIo) << "Goto Preset | HeadNr:" << headNr<< " ,PresetNr:" << activePreset+1 << ", Transition speed: (FPM)";
                break;
            }

            /*Normal recall of a Preset*/
            presetbus.setLed(PRESET_COLOR,activePreset);
            txSocket.send(headNr, GOTO_PRESET, activePreset+1,transitionSpeed);
            qCDebug(presetIo) << "Goto Preset | HeadNr:" << headNr<< " ,PresetNr:" << activePreset+1 << ", Transition speed:" << transitionSpeed;
            break;


        }

        case E_EXT_PRESET_CHANGE:
        {
            /*Preset change from another OCP in the net*/
            from = loadedEvent.data[0];
            data = loadedEvent.data[1];
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(model->getCamFlag(i,F_BOUNCING)){

                    model->setCamFlag(i,F_BOUNCING,false);
                    qCDebug(logicIo) << "Bounce stopped by external Preset change|" << "HeadNr: " << from;
                }
                if (model->getCamFlag(i,F_SPP_ON)) {
                    model->setCamFlag(i,F_SPP_ON,false);
                    qCDebug(logicIo) << "SPP stopped by external Preset change|" << "HeadNr: " << from;
                }
                if(model->getValue(i,ABS,V_HEADNR) == from ){
                    if(!(i == model->getActiveCameraSlot())){
                        model->setActivePreset(i,data-1);
                        qCDebug(presetIo) << "External Preset Change on inactive Slot | HeadNr:" << from<< " ,PresetNr:" << data;
                    }
                    else {
                        model->setActivePreset(i,data-1);
                        presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                        qCDebug(presetIo) << "External Preset Change on active Slot | HeadNr:" << from<< " ,PresetNr:" << data;
                    }
                }

            }
            break;

        }
        case E_CAMERA_CHANGE:
        {
             /*Push of a button on the camerabus*/
            camerabus.setLed(CAMERA_COLOR,loadedEvent.data[0]);
            model->setActiveCameraSlot(loadedEvent.data[0]);
            /*Update the view*/
            model->setUpView();
            model->updateView();

            /*Update the presetbus leds*/
            if (model->getCamFlag(F_SPP_ON)){
                presetbus.setLed(SPP_COLOR, model->getActivePreset());
            }
            else if(model->getCamFlag(F_BOUNCING)){
                presetbus.setLed(CAMERA_COLOR,model->getActivePreset());
            }
            else{
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            }
            /*Send command to the xpt router, if enabled and connected*/
            if(model->getXptEnabled() && model->getXptConnected()){
                int source = model->getXptSlotSource(model->getActiveCameraSlot());
                int destination = model->getXptDestination();
                contr_err = xptSocket.sendChange(source, destination);
                qCDebug(xptIo)<<"Send routing change | Source:" << source << ", Destination:" << destination;
                if (contr_err < 0) {
                    contr_err = errno;
                    logSystemError(contr_err, "Could not send to Xpt");
                    model->setXptConnected(false);
                }
            }
            qCDebug(logicIo)<<"Camera slot change | SlotNr:" << loadedEvent.data[0];
            break;
        }
        case E_RX_CAMERA_WATCHDOG:
            /*Watchdog from Camera*/
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
                    qCDebug(logicIo) << "New camera registered | " << "HeadNr:" << from << "Cam Type:" << type ;
                }
            }
            qCDebug(rxWatchdogIo) << "Watchdog received from Camera | "<< "HeadNr:" << from << "Cam Type:" << type << "Time:"<< QTime::currentTime().toString();;
            break;
        case E_IRIS_CHANGE:
            /*Iris Change from RCP*/
            alignSlots(V_IRIS);
            /*Fast Iris mode for cam type 6*/
            if(model->getFastIris() && model->getCamtype()==6){
                 model->setValue(INC,V_IRIS,loadedEvent.data[0]<<3);
            }
            else {
               model->setValue(INC, V_IRIS, loadedEvent.data[0]);
            }
            txSocket.send(model->getValue(ABS,V_HEADNR), IRIS_OPEN, model->getValue(ABS,V_IRIS));
            qCDebug(logicIo) << "Iris Change from RCP | value:" << model->getValue(ABS,V_IRIS);
            break;
        case E_PED_CHANGE:
            /*Black Level change from RCP*/
            model->setValue(INC, V_PED, loadedEvent.data[0]);
            alignSlots(V_PED);
            txSocket.send(model->getValue(ABS,V_HEADNR), MASTER_PED_UP, model->getValue(ABS,V_PED));
            qCDebug(logicIo) << "Master Black Change from RCP | value:" << model->getValue(ABS,V_PED);
            break;
        case E_WBLUE_CHANGE:
            /*Blue white chage from RCP*/
            model->setValue(INC, V_W_BLUE, loadedEvent.data[0]);
            alignSlots(V_W_BLUE);
            txSocket.send(model->getValue(ABS,V_HEADNR), BLUE_GAIN_ADJ_UP, model->getValue(ABS,V_W_BLUE));
            qCDebug(logicIo) << "W Blue Change from RCP | value:" << model->getValue(ABS,V_W_BLUE);
            break;
        case E_WRED_CHANGE:
            /*Red white chage from RCP*/
            model->setValue(INC, V_W_RED, loadedEvent.data[0]);
            alignSlots(V_W_RED);
            txSocket.send(model->getValue(ABS,V_HEADNR), RED_GAIN_ADJ_UP, model->getValue(ABS,V_W_RED));
            qCDebug(logicIo) << "W Red Change from RCP | value:" << model->getValue(ABS,V_W_RED);
            break;
        case E_BBLUE_CHANGE:
            /*Blue black chage from RCP*/
            model->setValue(INC, V_B_BLUE, loadedEvent.data[0]);
            alignSlots(V_B_BLUE);
            txSocket.send(model->getValue(ABS,V_HEADNR), BLUE_PED_UP, model->getValue(ABS,V_B_BLUE));
            qCDebug(logicIo) << "B Blue Change from RCP | value:" << model->getValue(ABS,V_B_BLUE);
            break;
        case E_BRED_CHANGE:
            /*Red black chage from RCP*/
            model->setValue(INC, V_B_RED, loadedEvent.data[0]);
            alignSlots(V_B_RED);
            txSocket.send(model->getValue(ABS,V_HEADNR), RED_PED_UP, model->getValue(ABS,V_B_RED));
            qCDebug(logicIo) << "B Red Change from RCP | value:" << model->getValue(ABS,V_B_RED);
            break;
        case E_GAIN_CHANGE:
            /*Gain chage from RCP*/
            model->setValue(INC, V_GAIN, loadedEvent.data[0]);
            alignSlots(V_GAIN);
            txSocket.send(model->getValue(ABS,V_HEADNR), CAMERA_GAIN_UP, model->getValue(ABS,V_GAIN));
            qCDebug(logicIo) << "Gain Change from RCP | value:" << model->getValue(ABS,V_GAIN);
            break;
        case E_SHUTTER_CHANGE:
            /*Shutter chage from RCP*/
            model->setValue(INC, V_SHUTTER, loadedEvent.data[0]);
            alignSlots(V_SHUTTER);
            txSocket.send(model->getValue(ABS,V_HEADNR), SHUTTER_UP, model->getValue(ABS,V_SHUTTER));
            qCDebug(logicIo) << "Shutter Change from RCP | value:" << model->getValue(ABS,V_SHUTTER);
            break;
        case E_WRITE_SAVEFILE:
            /*Write savefile by user input on Touch*/
            writeSavefile();
            qCDebug(logicIo) << "Wrote Savefile |";
            break;
        case E_LOAD_SAVEFILE:
            /*Load savefile by user input on touch*/
            loadSavefile();
            qCDebug(logicIo) << "Loaded Savefile |";
            break;
        case E_WRITE_AUTOSAVE:
            /*Autosave after a certain time*/
            writeAutosave();
            qCDebug(logicIo) << "Wrote autosave |";
            break;
        case E_LOAD_AUTOSAVE:
            /*Load autsave on startUp*/
            loadAutosave();
            qCDebug(logicIo) << "Loaded autosave |";
            break;
        case E_WIDESET:
            /*Widset for bounce mode from user input on touch*/
            txSocket.send(model->getValue(ABS,V_HEADNR),BNCE_ZOOM_WIDE_SET);
            qCDebug(logicIo) << "Send WideSet in Bounce Mode |";
            break;
        case E_CAMERA_ANSWER:
            /*Answer from a camera in the net*/
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
                    qCDebug(rxHeadIo) << "Answer from Camera | HeadNr:" << from << "Command:" << command << "Data:" << data;
                }
            }
            break;
        case E_RX_ADJ_RCP_CMD:
            /*Adjustings from a RCP in the net*/
            from = loadedEvent.data[0];         //to which camera head the cmd was sent
            command = loadedEvent.data[1];
            data = loadedEvent.data[2];

            //update model, if headnumber exists in slots
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(model->getValue(i,ABS,V_HEADNR) == from){

                    if(model->getValueFromBBMCommand(command) > 0){  //there could be commandtypes without values
                       model->setValue(i, ABS, model->getValueFromBBMCommand(command), data);
                    }
                    qCDebug(rxRcpIo) << "Answer from RCP | HeadNr:" << from << "Command:" << command << "Data:" << data;
                }
            }
            break;
        case E_SETUP_HEAD:
            /*Initial Event from Controller*/
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
            qCDebug(logicIo) << "Head init done |";
            break;
        case E_CALIB_HEAD:
            /*Calib Head from user input on touch*/
            txSocket.send(model->getValue(ABS,V_HEADNR),CALIBRATE_HEAD);
            break;
        case E_SET_LOWER_LIMIT:
            /*lower limit from user input on touch*/
            txSocket.send(model->getValue(ABS,V_HEADNR),TILT_LOWER_LIMIT);
            break;
        case E_SET_UPPER_LIMIT:
            /*upper limit from user input on touch*/
            txSocket.send(model->getValue(ABS,V_HEADNR),TILT_UPPER_LIMIT);
            break;
        case E_CLEAR_LIMIT:
            /*clear limits from user input on touch*/
            txSocket.send(model->getValue(ABS,V_HEADNR),TILT_CLEAR_LIMIT);
            break;
        case E_BOUNCE_BLINK:
            /*Blik timer callback*/
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
            /*Connection to Xpt Router from user input on touch*/
            if(model->getXptEnabled()){
                contr_err = xptSocket.init(XptInterface::XptType(model->getXptType()),model->getXptIpAdress()); // Initialize socket with the corresponding xpt type
                if(contr_err<0){
                    contr_err = errno;
                    logSystemError(contr_err, "Could not initialize xpt-interface");
                qCWarning(xptIo)<< "Could not initialize Xpt Socket| Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                }
                qCDebug(xptIo)<< "Xpt Socket initialized | Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                contr_err = xptSocket.connectToXpt(); //try to connect
                if(contr_err < 0){
                    contr_err = errno;
                    logSystemError(contr_err, "Could not connect to Xpt");
                    qCWarning(xptIo)<< "Could not connect to Xpt Socket| Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                    model->updateXptEnableStatus(false); //initial connection failed
                    model->setXptConnected(false);
                }else {
                    model->setXptConnected(true);
                    xptWatchdog.start();    // start watchdog for preiodic ping
                    qCDebug(xptIo)<< "Connected to Xpt | Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                    }

            }
           else{
            contr_err = xptSocket.disconnect();
            if(contr_err < 0){
                contr_err = errno;
                qCWarning(xptIo)<< "Could not disconnect from Xpt Socket| Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                logSystemError(contr_err, "Could not disconnect xpt");
            }
                model->setXptConnected(false);
                qCDebug(xptIo)<< "Disconnected from Xpt | Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                xptWatchdog.stop(); //stop timer
            }
            break;

        case E_XPT_WATCHDOG:
            /*Watchdog for periodic xpt ping*/
            if(model->getXptEnabled())
            {
                int connection;
                connection = xptSocket.checkConnection(); //ping
                /*No answer form XPT router*/
                if (connection <= 0 ) {
                    xptConnectionAttempts++;
                    xptSocket.disconnect();     //disconnect
                    if (xptSocket.connectToXpt() < 0) { //reconnect
                        qCDebug(xptIo)<< "Try to reconnect Xpt | Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress() << ", Connection attempt:" << xptConnectionAttempts;
                        model->setXptConnected(false);  //show that not connected
                    }
                    /*stop periodic ping after 20 attemts*/
                    if(xptConnectionAttempts > 20) {
                        xptConnectionAttempts=0;
                        model->setXptEnabled(false);
                        model->updateXptEnableStatus(false);
                        xptWatchdog.stop();
                        qCDebug(xptIo)<< "Xpt unreachable | Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress() << ", Connection attempt:" << xptConnectionAttempts;
                        break;
                    }
                     //model->setXptConnected(false);
                   qDebug()<<"no ACK received";

                }
                /*Xpt router answered*/
                else if(connection == 1 && model->getXptConnected()==false) {
                    model->setXptConnected(true);
                    break;
                }


            }
            break;


        case E_SPP_START:
            /*Sheduled Preset Positioning start fron user input on touch*/
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
            qCDebug(logicIo)<< "SPP started by user |";
            break;

        case E_PRESET_REACHED:
        {
            /*Preset reached from a camera in the net*/
            headNr = loadedEvent.data[0];
            int presetNr = loadedEvent.data[1];
            qCDebug(presetIo)<< "Preset reached | HeadNr:" << headNr << "PresetNr:" << presetNr;

            /*Use it for Scheduled Preset Positioning*/
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(headNr == model->getValue(i,ABS,V_HEADNR) ){      //if sender head matches any current slot head & spp flag is set
                   model->setCamFlag(i,F_PRESET_MOVE,false); //debug
                   if(model->getCamFlag(i,F_SPP_ON)){
                    //set state
                    switch (model->getSppState(i)) {
                    case S_SPP_GOTO1:
                        if(presetNr == model->getValue(OFFSET,V_SPP1)){
                            model->setSppState(i, S_SPP_WAIT1);     //if reached, go to wait state 1
                        qCDebug(logicIo)<< "SPP preset reached | Current state:" << S_SPP_GOTO1 << ", Next State:" << S_SPP_WAIT1 << ", Reached preset:" << presetNr;
                        }
                        break;
                    case S_SPP_GOTO2:
                        if(presetNr == model->getValue(OFFSET,V_SPP2)){
                            model->setSppState(i, S_SPP_WAIT2);     //of reached go to wait state 2
                            qCDebug(logicIo)<< "SPP preset reached | Current state:" << S_SPP_GOTO2 << ", Next State:" << S_SPP_WAIT2 << ", Reached preset:" << presetNr;
                        }
                        break;
                    default:
                        break;
                    }

                    //actions
                    sppTimer[i].start();
                    qCDebug(logicIo)<< "SPP wait Timer started| SlotNr:" << i;
                }}
            }

            break;
        }
        case E_SPP_WAIT_DONE:
            /*Scheduled Preset Positioning timer elapsed*/
            // from anywhere
            from = loadedEvent.data[0];                         // slotNr
            headNr = model->getValue(from,ABS,V_HEADNR);

            //set state
            switch (model->getSppState(from)) {
            case S_SPP_WAIT1:
                model->setSppState(from, S_SPP_GOTO2);
                sppPreset = model->getValue(from,ABS,V_SPP2);
                qCDebug(logicIo)<< "SPP wait time elapsed | Current state:" << S_SPP_WAIT1 << ", Next State:" << S_SPP_GOTO2 ;
                break;
            case S_SPP_WAIT2:
                model->setSppState(from, S_SPP_GOTO1);
                sppPreset = model->getValue(from,ABS,V_SPP1);
                qCDebug(logicIo)<< "SPP wait time elapsed | Current state:" << S_SPP_WAIT2 << ", Next State:" << S_SPP_GOTO1 ;
                break;
            default:
                break;
            }

            //action
            sppTimer[from].stop();
            qCDebug(logicIo)<< "SPP wait Timer stopped| SlotNr:" << from;
            txSocket.send(headNr, GOTO_PRESET, sppPreset+1, model->getValue(ABS,V_TRANS_SPEED) );
            qCDebug(presetIo) << "Goto Preset | HeadNr:" << headNr<< " ,PresetNr:" << sppPreset+1 << ", Transition speed:" << model->getValue(ABS,V_TRANS_SPEED);

            model->setActivePreset(from, sppPreset);
            if(from == model->getActiveCameraSlot()){       // only set button led if according slot is active
                presetbus.setLed(SPP_COLOR, sppPreset);
            }
            // wait for response
            break;

        case E_SPP_ABORT:
            /*Spp abort from user input*/
            // only from current slot
            from = model->getActiveCameraSlot();        //slotNr

            //actions
            sppTimer[from].stop();
            qCDebug(logicIo)<< "SPP wait Timer stopped after abort| SlotNr:" << from;
            presetbus.setLed(PRESET_COLOR, model->getActivePreset());

            //set states
            model->setCamFlag(from, F_SPP_ON, false);
            model->setSppState(from, S_SPP_IDLE);
            break;
        case E_REQUEST_SETTINGS:
            /*Request all camera settings from user input on touch*/
            requestCameraSettings(model->getActiveCameraSlot());
            break;
        case E_REQ_SETTINGS_TIMER:
            /*Timer to request remaining Telegrams*/
            slotNr = loadedEvent.data[0];
            checkSettingsRequest(slotNr);
            break;

        case E_FAST_IRIS:
            /*Fast Iris mode from user input with push on rotary encoder button*/
            /*Can be activated when Iris field is selected and the button was pressed, only works with cam type 6 */
            if(model->getCamtype()==6 && model->getRotaryField()==V_IRIS){
                if (model->getFastIris() == true) {
                    model->setFastIris(false);
                    qCDebug(logicIo) << "Fast Iris inactive |";
                }               //toggle
                else {
                    model->setFastIris(true);
                    qCDebug(logicIo) << "Fast Iris active |";
                }
            }
            /*A push on the rotary encoders button can also clear the used presets.
             * but only if store preset was pressed before and the used presets are shown*/
            else {
                if (model->getCamFlag(F_PRST_IN_STORE)) {
                    model->clearUsedPresets();
                    presetbus.showStored(0,model->getActivePreset());
                    qCDebug(logicIo) << "Preset session cleared|";
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
