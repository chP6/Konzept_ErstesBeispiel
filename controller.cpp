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
#include <algorithm>


/*Signal from xptSocktet 'inputLabelsChanged' to controller 'onXptLabelChanged'*/
void Controller::onXptLableChanged()
{

    /*update Model and View*/
    model->setXptNumberOfInputs(xptSocket->getNumberOfInputs());
    model->setXptNumberOfOutputs(xptSocket->getNumberOfOutputs());
    model->setXptInputLables(xptSocket->getInputLabels());
    model->setXptOutputLables(xptSocket->getOutputLabels());
    model->updateView();
}

void Controller::onAppQuit()
{
    qCDebug(logicIo)<<"This is the end";
}

Controller::Controller(Model& model)
{
    /*set the model for the controller*/
    setModel(model);

    /*UDP Socket initialize port 9000*/
    contr_err = txSocket.init(9000);
    if(contr_err<0){
        contr_err = errno;
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

    axesUpdater.init(E_SEND_AXES_UPDATES, *this);
    axesUpdater.setInterval(AXES_UPDATE_INTERVAL_MS * 1000);

    /*push the first event in the queue*/
    queueEvent(E_SETUP_HEAD);


}

/*Set model*/
void Controller::setModel(Model &model){
    this->model = &model;
}

/*Set poller*/
void Controller::setPoller(Poller &poller){
    this->poller = &poller;
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

    /* control Settings */
    savefile.beginGroup("controls");
    for (axis_t axis : { kAxisPan, kAxisTilt, kAxisZoom, kAxisFocus, kAxisTravelling })
        savefile.setValue("controlAxis" + QString::number((int)axis), model->getControl(axis));
    savefile.endGroup();

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
    /* control Settings */
    savefile.beginGroup("controls");
    for (axis_t axis : { kAxisPan, kAxisTilt, kAxisZoom, kAxisFocus, kAxisTravelling })
        model->setControl(axis, (control_t)savefile.value("controlAxis" + QString::number((int)axis)).toInt());
    savefile.endGroup();

    /*write according to group*/
    savefile.beginGroup("xpt");
        model->setXptType(savefile.value("xptType").toInt());
        model->setXptIpField(ABS,0,savefile.value("xptIpField1").toInt());
        model->setXptIpField(ABS,1,savefile.value("xptIpField2").toInt());
        model->setXptIpField(ABS,2,savefile.value("xptIpField3").toInt());
        model->setXptIpField(ABS,3,savefile.value("xptIpField4").toInt());
        model->setXptNumberOfInputs(savefile.value("xptNumberOfInputs").toInt());
        model->setXptNumberOfOutputs(savefile.value("xptNumberOfOutputs").toInt());
        model->setXptDestinationAbs(savefile.value("xptDestination").toInt());
        savefile.endGroup();

    for (int i=0;i<NUMBER_OF_SLOTS;i++) {
        savefile.beginGroup("slot_"+QString::number(i));

        model->setCamType(i,savefile.value("camType").toInt());             //get min max etc...
        model->setActivePreset(i,savefile.value("activePreset").toInt());
        //model->setUsedPreset(i,savefile.value("usedPresets").toInt());    //better start a new session after reboot
        model->setXptSlot(i);
        model->setXptSlotSource(savefile.value("xptSource").toInt());

        for (int k=0;k<NUMBER_OF_FLAGS;k++) {
            if(k != F_BOUNCING){
            model->setCamFlag(i,k,savefile.value("flag_"+QString::number(k)).toBool());
            }
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

/*write the savefile to /opt/savefile*/
int Controller::writeSavefile(){
    QSettings savefile(SAVEFILE_PATH, QSettings::NativeFormat);
    settingsWrite(savefile);
    return savefile.status();
}

/*load the savefile from /opt/savefile*/
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

/*load the autosavefile from /opt/autosave*/
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

    /*if it's all empty, everything has been received and all is good. Stop the timer for periodic requests*/
    if(remainingRequests.empty()){
        reqSettingsTimer[slotNr].stop();
        model->setCamFlag(F_RECEIVED_ALL, true);
    }
    /*if some requests still open, re-request*/
    else if(model->getCamFlag(slotNr, F_CONNECTED)){
        for (size_t i=0;i<remainingRequests.size();i++){
            int headNr = model->getValue(slotNr,ABS,V_HEADNR);          // get the headnumber according to the slot
            int command = model->getTxCommand(remainingRequests[i]);    //get the command to request again
            txSocket.request(headNr, command);                          //request it
            qCDebug(requestIo)<<"Rerequest| SlotNr:"<< slotNr << ", Command:" << command;
        }
    }
}

/*request all settings e.g. after startup or new connection or user input*/
void Controller::requestCameraSettings(int slot){
    int headNr = model->getValue(slot,ABS,V_HEADNR);
    int command;
    model->clearRemainingTelegrams(slot);
    if(slot == model->getActiveCameraSlot()){model->receiveAllNew();}   //Make all GUI labels yellow
    for (int i=0;i<ROW_ENTRIES;i++) {
        command = model->getRequestCommand(slot,i);     //check if requestable & push, else -1
        if(command>0){
          txSocket.request(headNr, command);
          qCDebug(requestIo)<<"Initial request| HeadNr:"<< headNr << ", Command:" << command;
        }
    }
    /*Start timer to re-request settings which did not arrive*/
    if(model->getCamFlag(slot,F_CONNECTED)){
        reqSettingsTimer[slot].start();
    }
    qCDebug(requestIo)<<"Request timer started| SlotNr:"<< slot;
}


/*model alignement, if other cameraslots have the same headnumber as the current slot*/
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


/*start processQueue in a seperate thread*/
void Controller::startQueueProcessThread(){
    usleep(800000);
    eventQueue.initCleanup();   // clean queue from unwanted startup events from joystick
    applicationRunning = true;
    t1 = std::thread(&Controller::processQeue, this);       //1.Arg: function type that will be called, 2.Arg: pointer to object (this)
    //t1.detach();
    axesUpdater.start();
}

void Controller::stopQueueProcessThread()
{
    txSocket.~Networkinterface();
    for (int i = 0;i<NUMBER_OF_SLOTS;i++) {
        sppTimer[i].stop();
        reqSettingsTimer[i].stop();
    }
    xptWatchdog.stop();
    blinkTimer.stop();
    applicationRunning = false;
    eventQueue.qeueEvent(E_CLEAR); //fire event to stop thread
}


/*processes the events in the eventQueue*/
void Controller::processQeue(){
    event_s loadedEvent;

    int from, type, command, data, headNr, sppPreset=0, time, slotNr;

    //struct timeval  tv1, tv2;
    model->setUpView();

    while(applicationRunning){
        eventQueue.pullEvent(loadedEvent);      //blocks if queue is empty
//        gettimeofday(&tv1, NULL);

        switch (loadedEvent.evt) {
        case E_CLEAR:
            /*debugging no function*/
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
                }
                else{
                    model->setValue(INC,field,loadedEvent.data[0]);     //Last Element
                }

                alignSlots(field);

                /*send, if associated commandtype exists*/
                if(model->getTxCommand(field) > 0){  //there could be values without commandtypes
                    if (model->getRotaryField() == V_IRIS)
                        txSocket.send(model->getValue(ABS,V_HEADNR),model->getTxCommand(field),model->getValue(NORM,field));
                    else
                        txSocket.send(model->getValue(ABS,V_HEADNR),model->getTxCommand(field),model->getValue(ABS,field));
                }

                /*the headnumer has been changed*/
                if(field==V_HEADNR){
                    model->setCamFlag(F_CAMERA_KNOWN, false);
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

            model->setCamFlag(F_PRESET_MOVE,false);

            if(model->getCamFlag(F_PAN_INVERT)){x=10000-x;} //dirextion of the x axis inverted
            y = loadedEvent.data[1];
            if(model->getCamFlag(F_TILT_INVERT)){y=10000-y;} //direction of the y axis inverted

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

            /*debug*/
            qCDebug(logicIo)<<"Joystick Pan/Tilt| x-Axis:"<< x << ", y-Axis:" << y;
            break;

         case E_SET_ZOOM:
            /*Zoom Commands from the z axis of the joystick*/
            int z;
            z=loadedEvent.data[0];

            model->setCamFlag(F_PRESET_MOVE,false);

            if(!model->getCamFlag(F_ZOOM_INVERT)){z = 254 - z;} //direction of the zoom inverted

            /*Joystick creeps around the zero point*/
            if(z==127){
                usleep(CREEP_T);
            }

            txSocket.send(model->getValue(ABS,V_HEADNR),ZOOM_FOCUS_SET, z); //send zoom information to the camera

            /*Bounce mode can be aborted with the use of the zoom*/
            if(model->getCamFlag(F_BOUNCING) ){
                model->setCamFlag(F_BOUNCING,false);
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                model->setCamFlag(F_BOUNCE_ABORTED,true);
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
            if(model->getCamFlag(F_FOCUS_INVERT)){focus=(-focus);} //The direction of the focus can be inverted
            model->setValue(INC, V_FOCUS, focus);
            txSocket.send(model->getValue(ABS,V_HEADNR), FOCUS_SET_ABSOLUTE, model->getValue(ABS, V_FOCUS));
            qCDebug(logicIo)<<"Focus| delta:"<< focus << ", value:"<<model->getValue(ABS,V_FOCUS);
            break;
        case E_AUTOFOCUS:
            /*Trigger of the autofocus, send it right away*/
            txSocket.send(model->getValue(ABS,V_HEADNR), SET_FOCUS_PUSH);
            qCDebug(logicIo)<< "Autofocus pressed |";
            break;
//        case E_AUTOFOCUS_ANSWER:
//            /*Answer from the camera after autofocus to get the new value into the model*/
//            model->setValue(ABS,V_FOCUS,loadedEvent.data[0]);
//            qCDebug(rxHeadIo) << "Focus value received | value:" << loadedEvent.data[0];
//            break;
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
            /*Move camera to preset position*/
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

            /*If Scheduled preset positioning is on, abort it and go to the slected preset, then break*/
            if(model->getCamFlag(F_SPP_ON)){
                queueEvent(E_SPP_ABORT);
                presetbus.setLed(PRESET_COLOR,activePreset);
                txSocket.send(headNr, GOTO_PRESET, activePreset+1,transitionSpeed);
                model->setCamFlag(F_PRESET_MOVE,true);
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
                    presetbus.setLed(PRESET_COLOR,activePreset);
                    /*TeleSet seems to be obsolete, to do: ask, reimplement, test*/
                    //if (model->getCamFlag(F_BOUNCE_ABORTED)) {
                        /*If bounce was aborted, don't do a teleset, just restart it*/
                        txSocket.send(headNr,BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Restart it
                        //model->setCamFlag(F_BOUNCE_ABORTED,false);
                        qCDebug(logicIo) << "Bounce restarted by key press (no TeleSet) |" << "HeadNr: " << headNr << "Zoom Speed:" << model->getValue(ABS, V_BOUNCE_ZOOM_SPEED);
                    //}
                   // else {
                        /*teleset and restart*/
                        //txSocket.send(headNr,BNCE_ZOOM_TELE_SET);    //TeleSet
                        //txSocket.send(headNr,BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED)); //Start it
                       // qCDebug(logicIo) << "Bounce started by key press (TeleSet fired) |" << "HeadNr: " << headNr << "Zoom Speed:" << model->getValue(ABS, V_BOUNCE_ZOOM_SPEED);
                    //}

                    model->setCamFlag(F_BOUNCING,true);
                    blinkTimer.start();

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
                    //txSocket.send(headNr,BNCE_ZOOM_START,model->getValue(ABS,V_BOUNCE_ZOOM_SPEED));  //Stop It
                    //model->setCamFlag(F_BOUNCE_ABORTED,false);
                    qCDebug(logicIo) << "Bounce stopped by key press|" << "HeadNr: " << headNr;
              }
            }

            /*Fast transition flag is set. Recall preset with fastest transition rate and break*/
            if(model->getCamFlag(F_FASTTRANS)){
                presetbus.setLed(PRESET_COLOR,activePreset);
                txSocket.send(headNr, GOTO_PRESET, activePreset+1,1);
                model->setCamFlag(F_PRESET_MOVE,true);
                qCDebug(presetIo) << "Goto Preset | HeadNr:" << headNr<< " ,PresetNr:" << activePreset+1 << ", Transition speed: (FPM)";
                break;
            }

            /*Normal recall of a Preset*/
            presetbus.setLed(PRESET_COLOR,activePreset);
            txSocket.send(headNr, GOTO_PRESET, activePreset+1,transitionSpeed);
            model->setCamFlag(F_PRESET_MOVE,true);
            qCDebug(presetIo) << "Goto Preset | HeadNr:" << headNr<< " ,PresetNr:" << activePreset+1 << ", Transition speed:" << transitionSpeed;
            break;

        }

        case E_EXT_PRESET_CHANGE:
        {
            /*Preset change from another OCP in the net*/
            from = loadedEvent.data[0];
            data = loadedEvent.data[1];
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if (model->getValue(i,ABS,V_HEADNR) == from){
                    if(!(i == model->getActiveCameraSlot())){
                        if(model->getCamFlag(i,F_BOUNCING)){
                            model->setCamFlag(i,F_BOUNCING,false);
                            qCDebug(logicIo) << "Bounce stopped by external Preset change|" << "HeadNr: " << from;
                            }
                        if (model->getCamFlag(i,F_SPP_ON)) {
                            model->setCamFlag(i,F_SPP_ON,false);
                            qCDebug(logicIo) << "SPP stopped by external Preset change|" << "HeadNr: " << from;
                            }

                            model->setActivePreset(i,data-1);
                            qCDebug(presetIo) << "External Preset Change on inactive Slot | HeadNr:" << from<< " ,PresetNr:" << data;
                            }
                    else {
                        //model->setActivePreset(i,data-1);
                        presetbus.setLed(PRESET_COLOR,model->getActivePreset());
                        qCDebug(presetIo) << "External Preset Change on active Slot | HeadNr:" << from<< " ,PresetNr:" << data;
                    }
                }

            }
            break;

        }
        case E_CAMERA_CHANGE:
        {
            if(!(model->getCamFlag(F_PRESET_MOVE))){
                txSocket.send(model->getValue(ABS,V_HEADNR), TILT_PAN, 5000, 5000);
            }
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
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            }
            else if(model->getCamFlag(F_PRST_IN_STORE)){
                presetbus.showStored(model->getUsedPreset(),model->getActivePreset());
            }
            else{
                presetbus.setLed(PRESET_COLOR,model->getActivePreset());
            }
            /*Send command to the xpt router, if enabled and connected*/
            if(model->getXptEnabled() && model->getXptConnected()){
                int source = model->getXptSlotSource(model->getActiveCameraSlot());
                int destination = model->getXptDestination();
                contr_err = xptSocket->sendChange(source, destination);
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
                if (model->getValue(i,ABS,V_HEADNR) == from) {
                    /*Camera exists in one or more slots -> Clear cameraWaitingFlags*/
                    model->setCameraWaitingflag(i,false);
                }

                if (model->getValue(i,ABS,V_HEADNR) == from && !model->getCamFlag(i,F_CAMERA_KNOWN)) {
                    /*Camera exists in a slot & KNOWN flag is not set -> request camera settings*/
                    model->setCamType(i, type);     //Set camera type and load all min,max settings
                    requestCameraSettings(i);

                    model->setCamFlag(i,F_CAMERA_KNOWN,true);
                    model->setUpView();     //grays out ui buttons for not supported camera settings, set yellow for requests
                    model->updateView();
                    qCDebug(logicIo) << "New camera registered | " << "HeadNr:" << from << "Cam Type:" << type ;
                }
            }
            qCDebug(rxWatchdogIo) << "Watchdog received from Camera | "<< "HeadNr:" << from << "Cam Type:" << type << "Time:"<< QTime::currentTime().toString();;
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

            /*Write answer values to respective properties in model, if headnumber exists*/
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(model->getValue(i,ABS,V_HEADNR) == from){
                    int property = model->getValueFromBBMCommand(command);
                    if(property > 0){  //there could be commandtypes without values
                        if (property == IRIS_OPEN) {
                            model->setValue(i, NORM, model->getValueFromBBMCommand(command), data);
                        } else {
                            model->setValue(i, ABS, model->getValueFromBBMCommand(command), data);
                        }
                       model->setRequestReceived(i,model->getValueFromBBMCommand(command));
//                       if(!model->getCamFlag(i,F_RECEIVED_ALL)){
//                         checkSettingsRequest(i);
//                       }
                    }
                    /*debug*/
                    qCDebug(rxHeadIo) << "Answer from Camera | HeadNr:" << from << "Command:" << command << "Data:" << data;
                }
            }
            break;
        case E_RX_ADJ_RCP_CMD:
            /*Update model with values sent by an adjacent RCP in the net*/
            from = loadedEvent.data[0];         //to which camera head the cmd was sent
            command = loadedEvent.data[1];
            data = loadedEvent.data[2];

            /*update model, if headnumber exists in slots*/
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(model->getValue(i,ABS,V_HEADNR) == from){
                    int property = model->getValueFromBBMCommand(command);

                    if(property > 0){  //there could be commandtypes without values
                        if (property == IRIS_OPEN) {
                            model->setValue(i, NORM, model->getValueFromBBMCommand(command), data);
                        } else {
                            model->setValue(i, ABS, model->getValueFromBBMCommand(command), data);
                        }
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
            system("echo 'Controllerapplication ready....' >/dev/kmsg");
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
                //contr_err = xptSocket.init(XptInterface::XptType(model->getXptType()),model->getXptIpAdress()); // Initialize socket with the corresponding xpt type
                switch (model->getXptType()) {
                case I_XPT_TYPE_BLACKMAGIC: xptSocket = new BmdInterface();break;
                case I_XPT_TYPE_ROSS: xptSocket = new RossInterface();break;
                }

                /*Signal connection from xptSocktet 'inputLabelsChanged' to controller 'onXptLabelChanged'*/
                QObject::connect(xptSocket, SIGNAL(inputLabelsChanged()),
                                 this, SLOT(onXptLableChanged()));

                contr_err = xptSocket->init(model->getXptIpAdress());
                    if(contr_err<0){
                        contr_err = errno;
                        logSystemError(contr_err, "Could not initialize xpt-interface");
                        qCWarning(xptIo)<< "Could not initialize Xpt Socket| Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                    }
                qCDebug(xptIo)<< "Xpt Socket initialized | Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();

                contr_err = xptSocket->connectToXpt(); //try to connect
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

            contr_err = xptSocket->disconnect();
            if(contr_err < 0){
                contr_err = errno;
                qCWarning(xptIo)<< "Could not disconnect from Xpt Socket| Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                logSystemError(contr_err, "Could not disconnect xpt");
            }
                model->setXptConnected(false);
                qCDebug(xptIo)<< "Disconnected from Xpt | Xpt Type:" << model->getXptType() << ", Xpt adress:" << model->getXptIpAdress();
                xptWatchdog.stop(); //stop timer
                delete xptSocket;
            }
            break;

        case E_XPT_WATCHDOG:
            /*Watchdog for periodic xpt ping*/
            if(model->getXptEnabled())
            {
                int connection;
                connection = xptSocket->checkConnection(); //ping
                /*No answer form XPT router*/
                if (connection <= 0 ) {
                    xptConnectionAttempts++;
                    xptSocket->disconnect();     //disconnect
                    if (xptSocket->connectToXpt() < 0) { //reconnect
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
                        delete xptSocket;
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
            /*Sheduled Preset Positioning start from user input on touch*/
            from = model->getActiveCameraSlot();
            headNr = model->getValue(ABS,V_HEADNR);
            sppPreset = model->getValue(ABS,V_SPP1);
            time = model->getValue(ABS,V_SPP_WAIT_TIME);

            /*Set goto1 state & spp_on flag*/
            model->setCamFlag(from, F_SPP_ON, true);
            model->setSppState(from, S_SPP_GOTO1);

            /*Configure timer with user wait time between positions*/
            if(time==0){
                sppTimer[model->getActiveCameraSlot()].setInterval(1*10000);    // if no wait time, make 10us, 0 would stop timer
            }
            else{
                sppTimer[model->getActiveCameraSlot()].setInterval(time*1000*1000);
            }
            /*Send goto preset command, head will respond when position reached*/
            txSocket.send(headNr, GOTO_PRESET, sppPreset+1,model->getValue(ABS,V_TRANS_SPEED));
            model->setActivePreset(sppPreset);
            presetbus.setLed(SPP_COLOR, sppPreset);
            qCDebug(logicIo)<< "SPP started by user |";
            break;

        case E_PRESET_REACHED:
        {
            /*Preset reached from a camera in the net*/
            headNr = loadedEvent.data[0];
            int presetNr = loadedEvent.data[1];

            qCDebug(presetIo)<< "Preset reached | HeadNr:" << headNr << "PresetNr:" << presetNr;

            /*Check if respective camera is in spp mode*/
            for (int i=0;i<NUMBER_OF_SLOTS;i++) {
                if(headNr == model->getValue(i,ABS,V_HEADNR) ){      //if sender head matches any current slot head & spp flag is set
                   model->setCamFlag(i,F_PRESET_MOVE,false);
                   if(model->getCamFlag(i,F_SPP_ON)){
                        /*Act according to current spp state*/
                        switch (model->getSppState(i)) {
                        case S_SPP_GOTO1:
                            /*Position 1 done, set state to wait1*/
                            if(presetNr == model->getValue(i,ABS,V_SPP1)+1){
                                model->setSppState(i, S_SPP_WAIT1);     //if reached, go to wait state 1
                                model->getValue(i,ABS,V_SPP_WAIT_TIME) == 0 ? queueEvent(E_SPP_WAIT_DONE,i) : sppTimer[i].start();
                            qCDebug(logicIo)<< "SPP preset reached | Current state:" << S_SPP_GOTO1 << ", Next State:" << S_SPP_WAIT1 << ", Reached preset:" << presetNr;
                            }
                            break;
                        case S_SPP_GOTO2:
                            /*Position 2 done, set state to wait2*/
                            if(presetNr == model->getValue(i,ABS,V_SPP2)+1){
                                model->setSppState(i, S_SPP_WAIT2);     //if reached go to wait state 2
                                model->getValue(i,ABS,V_SPP_WAIT_TIME) == 0 ? queueEvent(E_SPP_WAIT_DONE,i) : sppTimer[i].start();
                                qCDebug(logicIo)<< "SPP preset reached | Current state:" << S_SPP_GOTO2 << ", Next State:" << S_SPP_WAIT2 << ", Reached preset:" << presetNr;
                            }
                            break;
                        default:
                            break;
                        }

                        /*Start spp timer*/
                        //sppTimer[i].start();
                        qCDebug(logicIo)<< "SPP wait Timer started| SlotNr:" << i;
                    }
                }
            }
        }
        break;

        case E_SPP_WAIT_DONE:
            /*Scheduled Preset Positioning timer elapsed*/
            // from anywhere
            from = loadedEvent.data[0];                         // slotNr
            headNr = model->getValue(from,ABS,V_HEADNR);

            /*Act according to current spp state*/
            switch (model->getSppState(from)) {
            case S_SPP_WAIT1:
                /*Waittime after Position 1 done, set state to goto2*/
                model->setSppState(from, S_SPP_GOTO2);
                sppPreset = model->getValue(from,ABS,V_SPP2);
                qCDebug(logicIo)<< "SPP wait time elapsed | Current state:" << S_SPP_WAIT1 << ", Next State:" << S_SPP_GOTO2 ;
                break;
            case S_SPP_WAIT2:
                /*Waittime after Position 2 done, set state to goto1*/
                model->setSppState(from, S_SPP_GOTO1);
                sppPreset = model->getValue(from,ABS,V_SPP1);
                qCDebug(logicIo)<< "SPP wait time elapsed | Current state:" << S_SPP_WAIT2 << ", Next State:" << S_SPP_GOTO1 ;
                break;
            default:
                break;
            }

            /*Stop spp timer, goto respective preset*/
            sppTimer[from].stop();
            qCDebug(logicIo)<< "SPP wait Timer stopped| SlotNr:" << from;
            model->setCamFlag(from,F_PRESET_MOVE,true);
            txSocket.send(headNr, GOTO_PRESET, sppPreset+1, model->getValue(from,ABS,V_TRANS_SPEED) );
            qCDebug(presetIo) << "Goto Preset | HeadNr:" << headNr<< " ,PresetNr:" << sppPreset+1 << ", Transition speed:" << model->getValue(ABS,V_TRANS_SPEED);

            model->setActivePreset(from, sppPreset);
            if(from == model->getActiveCameraSlot()){       // only set button led if according slot is active
                presetbus.setLed(SPP_COLOR, sppPreset);
            }
            /*head will report when position reached*/
            break;

        case E_SPP_ABORT:
            /*Spp abort from user input*/
            // only from current slot
            from = model->getActiveCameraSlot();        //slotNr

            /*Actions*/
            sppTimer[from].stop();
            qCDebug(logicIo)<< "SPP wait Timer stopped after abort| SlotNr:" << from;
            presetbus.setLed(PRESET_COLOR, model->getActivePreset());

            /*Clear spp flag, set state to idle*/
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

        case E_CONTROL_INPUT: {
            control_t control = (control_t)loadedEvent.data[0];
            for (axis_t axis : { kAxisPan, kAxisTilt, kAxisZoom, kAxisFocus, kAxisTravelling }) {
                /* bool absolute: currently only focus axis is absolute,
                 * but this shall be configurable in the future */
                bool absolute = axis == kAxisFocus;

                int data = loadedEvent.data[1];
                data = std::max(data, -INT16_MAX);
                data = std::min(data, INT16_MAX);
                data = model->getCamFlag(
                    axis == kAxisPan   ? F_PAN_INVERT :
                    axis == kAxisTilt  ? F_TILT_INVERT :
                    axis == kAxisZoom  ? F_ZOOM_INVERT :
                    axis == kAxisFocus ? F_FOCUS_INVERT :
                                         F_TRAVELLING_INVERT
                ) ? -data : data;

                if (model->getControl(axis) == control)
                    model->setAxis(axis, absolute ? data : data / 256, absolute);
            }
            /* Sheduled Preset Positioning can be aborted by control input */
            if (model->getCamFlag(F_SPP_ON)) {
                queueEvent(E_SPP_ABORT);
                qCDebug(logicIo) << "SPP stopped by control input";
            }
        } break;

        case E_SEND_AXES_UPDATES: {

            for (int i = 0; i < NUMBER_OF_SLOTS; i++) {
                int16_t axes[kAxisMax];
                bool useNewTelegrams = true;

                /* check axes updates on absolute values */
                if (model->getAxisUpdates(i, axes, false)) {

                    if (useNewTelegrams) {

                        { /* pan, tilt, zoom & focus */
                            int d[4] = { AXIS_NO_VALUE_REL, AXIS_NO_VALUE_REL, AXIS_NO_VALUE_REL, AXIS_NO_VALUE_REL};
                            int i = 0; bool sendTelegram = false;
                                for (axis_t axis : { kAxisPan, kAxisTilt, kAxisZoom, kAxisFocus }) {
                                if (axes[axis] != AXIS_NO_VALUE_REL)
                                    sendTelegram = true;
                                d[i++] = axes[axis];
                            }
                            if (sendTelegram)
                                txSocket.send(model->getValue(ABS, V_HEADNR), PAN_TILT_ZOOM_FOCUS, d);
                        }
                        { /* travelling (and further axes in the future...) */
                            int d[4] = { AXIS_NO_VALUE_REL, AXIS_NO_VALUE_REL, AXIS_NO_VALUE_REL, AXIS_NO_VALUE_REL};
                            int i = 0; bool sendTelegram = false;
                            for (axis_t axis : { kAxisTravelling }) {
                                if (axes[axis] != AXIS_NO_VALUE_REL)
                                    sendTelegram = true;
                                d[i++] = axes[axis];
                            }
                            if (sendTelegram)
                                txSocket.send(model->getValue(ABS, V_HEADNR), DOLLY_LIFT, d);
                        }

                    } else {

                        /* old BTT_TILT_PAN telegram */
                        bool panTiltUpdate = false;

                        /* hack: store previous pan/tilt values since old BTT_TILT_PAN
                           telegram does not support <no value> */
                        static int x[NUMBER_OF_SLOTS] = {};
                        static int y[NUMBER_OF_SLOTS] = {};

                        if (axes[kAxisPan] != AXIS_NO_VALUE_REL) {
                            x[i] = ((axes[kAxisPan] * 5000) / 127) + 5000;
                            panTiltUpdate = true;
                        }
                        if (axes[kAxisTilt] != AXIS_NO_VALUE_REL) {
                            y[i] = ((axes[kAxisTilt] * 5000) / 127) + 5000;
                            panTiltUpdate = true;
                        }
                        if (panTiltUpdate)
                            txSocket.send(model->getValue(i, ABS, V_HEADNR), TILT_PAN, x[i], y[i]);

                        if (axes[kAxisZoom] != AXIS_NO_VALUE_REL)
                            txSocket.send(model->getValue(ABS, V_HEADNR), ZOOM_FOCUS_SET, axes[kAxisZoom] + 127);
                    }
                }

                /* check axes updates on absolute values */
                if (model->getAxisUpdates(i, axes, true)) {

                    if (useNewTelegrams) {

                        /* pan/tilt absolute */
                        if (axes[kAxisPan] != AXIS_NO_VALUE_ABS || axes[kAxisTilt] != AXIS_NO_VALUE_ABS)
                            txSocket.send(model->getValue(ABS,V_HEADNR), PAN_TILT_SET_ABSOLUTE, axes[kAxisPan], axes[kAxisTilt]);

                        /* zoom/focus absolute */
                        if (axes[kAxisZoom] != AXIS_NO_VALUE_ABS || axes[kAxisFocus] != AXIS_NO_VALUE_ABS)
                            txSocket.send(model->getValue(ABS,V_HEADNR), ZOOM_FOCUS_SET_ABSOLUTE, axes[kAxisZoom], axes[kAxisFocus]);

                        /* travelling absolute */
                        if (axes[kAxisTravelling] != AXIS_NO_VALUE_ABS)
                            txSocket.send(model->getValue(ABS,V_HEADNR), DOLLY_LIFT_SET_ABSOLUTE, axes[kAxisTravelling], AXIS_NO_VALUE_ABS);

                    } else {

                        /* focus */
                        if (axes[kAxisFocus] != AXIS_NO_VALUE_ABS) {
                            int fmin = model->getMin(i, V_FOCUS);
                            int fmax = model->getMax(i, V_FOCUS);
                            int f = (((int32_t)axes[kAxisFocus] + INT16_MAX) * (fmax - fmin)) / (2 * INT16_MAX);
                            txSocket.send(model->getValue(ABS,V_HEADNR), FOCUS_SET_ABSOLUTE, f);

                            qDebug("old focus: %lx", f);
                        }
                    }
                }
            }
            break;
        }
        case E_USB_OCP_CHANGE:{
            /*Chage from USB RCP*/
            int property = loadedEvent.data[0];
            if(model->getTxCommand(property) > 0 && model->getValue(ABS,property) > 0){
                model->setValue(INC, property, loadedEvent.data[1]);
                alignSlots(property);
                txSocket.send(model->getValue(ABS,V_HEADNR), model->getTxCommand(property), model->getValue(ABS,property));
                qCDebug(logicIo) << "Change from RCP | property:"<< property <<"value:" << model->getValue(ABS,property);
            }
            break;
        }

        default:
            break;
        }
//        gettimeofday(&tv2, NULL);
//        qDebug("Work: %f seconds",
//             (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//             (double) (tv2.tv_sec - tv1.tv_sec));
    }
}
