#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "model.h"
#include "controller.h"
#include <QDebug>
#include <QMetaMethod>
#include <QColor>
#include <QCoreApplication>

using namespace Config;
class Model;

class Backend : public QObject
{
    Q_OBJECT
public:
    Backend();
    Backend(Model* model, Controller* controller);
    virtual ~Backend();
    virtual void update();
    QList<QVariant> getDialValues(properties_t property);
protected:
    Model *m_model;
    Controller *m_controller;

};
/********************************************************************************************/
class Home : public Backend
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> ptSpeed READ ptSpeed NOTIFY ptSpeedChanged)
    Q_PROPERTY(QList<QVariant> zoomSpeed READ zoomSpeed NOTIFY zoomSpeedChanged)
    Q_PROPERTY(QList<QVariant> transitionSpeed READ transitionSpeed NOTIFY transitionSpeedChanged)
    Q_PROPERTY(QList<QVariant> ramp READ ramp NOTIFY rampChanged)
    Q_PROPERTY(QList<QVariant> spp1 READ spp1 NOTIFY spp1Changed)
    Q_PROPERTY(QList<QVariant> spp2 READ spp2 NOTIFY spp2Changed)
    Q_PROPERTY(QList<QVariant> sppWaitTime READ sppWaitTime NOTIFY sppWaitTimeChanged)
    Q_PROPERTY(int headNr READ headNr NOTIFY headNrChanged)
    Q_PROPERTY(int preset READ preset NOTIFY presetChanged)
    Q_PROPERTY(bool presetMoving READ presetMoving NOTIFY presetMovingChanged)
    Q_PROPERTY(bool bounce READ bounce WRITE setBounce NOTIFY bounceChanged)
    Q_PROPERTY(bool sppStart READ sppStart WRITE setSppStart NOTIFY sppStartChanged)
    Q_PROPERTY(bool fpm READ fpm WRITE setFpm NOTIFY fpmChanged)
    Q_PROPERTY(bool cameraConnected READ cameraConnected NOTIFY cameraConnectedChanged)
    Q_PROPERTY(bool serverConnected READ serverConnected NOTIFY serverConnectedChanged)
 public:
    Home();
    Home(Model *model, Controller *controller);

    /* Dials */
    QList<QVariant> ptSpeed(){ return getDialValues(PtSpeed); }
    QList<QVariant> zoomSpeed() { return getDialValues(BounceZoomSpeed);}
    QList<QVariant> transitionSpeed(){return getDialValues(TransSpeed);}
    QList<QVariant> ramp(){return getDialValues(Ramp);}
    QList<QVariant> spp1(){return getDialValues(Spp1);}
    QList<QVariant> spp2(){return getDialValues(Spp2);}
    QList<QVariant> sppWaitTime(){return getDialValues(SppwWaitTime);}
    int headNr();
    int preset(){return m_model->getActivePreset();}
    Q_INVOKABLE void storePreset(){m_controller->queueEvent(E_STORE_PRESET);}

    /*Flags*/
    bool presetMoving(){return m_model->getCamFlag(PresetMoving);}
    bool bounce(){return m_model->getCamFlag(BounceEnabled);}
    bool sppStart(){return m_model->getCamFlag(SppEnabled);}
    bool fpm(){return m_model->getCamFlag(FastTransEnabled);}
    bool cameraConnected(){return m_model->getCamFlag(CameraConnected);}
    bool serverConnected(){return m_model->getServerStatus();}

public slots:
    void setBounce(bool bounce){m_model->setCamFlag(BounceEnabled,bounce);}
    void setSppStart(bool sppStart){
        m_model->setCamFlag(SppEnabled,sppStart);
        sppStart ? m_controller->queueEvent(E_SPP_START) : m_controller->queueEvent(E_SPP_ABORT);
    }
    void setFpm(bool fpm){m_model->setCamFlag(FastTransEnabled,fpm);}

signals:
    void presetChanged();
    void ptSpeedChanged();
    void zoomSpeedChanged();
    void transitionSpeedChanged();
    void rampChanged();
    void spp1Changed();
    void spp2Changed();
    void sppWaitTimeChanged();
    void headNrChanged();
    void presetMovingChanged();
    void bounceChanged();
    void sppStartChanged();
    void fpmChanged();
    void cameraConnectedChanged();
    void serverConnectedChanged();
};
/********************************************************************************************/
class CameraView : public Backend
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> iris READ getIris NOTIFY irisChanged)
    Q_PROPERTY(QList<QVariant> blackLevel READ blackLevel NOTIFY blackLevelChanged)
    Q_PROPERTY(QList<QVariant> gammaTable READ gammaTable NOTIFY gammaTableChanged)
    Q_PROPERTY(QList<QVariant> gamma READ gamma NOTIFY gammaChanged)
    Q_PROPERTY(QList<QVariant> kneePoint READ kneePoint NOTIFY kneePointChanged)
    Q_PROPERTY(QList<QVariant> knee READ knee NOTIFY kneeChanged)
    Q_PROPERTY(QList<QVariant> saturation READ saturation NOTIFY saturationChanged)
    Q_PROPERTY(QList<QVariant> colorTemperature READ colorTemperature NOTIFY colorTemperatureChanged)
    Q_PROPERTY(QList<QVariant> gain READ gain NOTIFY gainChanged)
    Q_PROPERTY(QList<QVariant> detail READ detail NOTIFY detailChanged)
    Q_PROPERTY(QList<QVariant> shutter READ shutter NOTIFY shutterChanged)
    Q_PROPERTY(QList<QVariant> ndFilter READ ndFilter NOTIFY ndFilterChanged)
    Q_PROPERTY(QList<QVariant> redWhite READ redWhite NOTIFY redWhiteChanged)
    Q_PROPERTY(QList<QVariant> blueWhite READ blueWhite NOTIFY blueWhiteChanged)
    Q_PROPERTY(QList<QVariant> redBlack READ redBlack NOTIFY redBlackChanged)
    Q_PROPERTY(QList<QVariant> blueBlack READ blueBlack NOTIFY blueBlackChanged)
   public:
    CameraView();
    CameraView(Model* model, Controller* controller);

    /*Dials */
    QList<QVariant> getIris(){return getDialValues(Iris);}
    QList<QVariant> blackLevel(){return getDialValues(Ped);}
    QList<QVariant> gammaTable(){return getDialValues(GammaTable);}
    QList<QVariant> gamma(){return getDialValues(Gamma);}
    QList<QVariant> kneePoint(){return getDialValues(KneePoint);}
    QList<QVariant> knee(){return getDialValues(Knee);}
    QList<QVariant> saturation(){return getDialValues(Color);}
    QList<QVariant> colorTemperature(){return getDialValues(ColorTemp);}
    QList<QVariant> gain(){return getDialValues(Gain);}
    QList<QVariant> detail(){return getDialValues(Detail);}
    QList<QVariant> shutter(){return getDialValues(Shutter);}
    QList<QVariant> ndFilter(){return getDialValues(NdFilter);}
    QList<QVariant> redWhite(){return getDialValues(WhiteRed);}
    QList<QVariant> blueWhite(){return getDialValues(WhiteBlue);}
    QList<QVariant> redBlack(){return getDialValues(BlackRed);}
    QList<QVariant> blueBlack(){return getDialValues(BlackBlue);}

    Q_INVOKABLE void setValue(int property, int value){
        m_model->setValue(Absolute,Property(property),value);
        m_controller->queueEvent(E_INCREASE,0);
    }
    Q_INVOKABLE void setField(int property){m_model->setRotaryField(properties_t(property));}

signals:
    void updateView();
    void blackLevelChanged();
    void irisChanged();
    void gammaTableChanged();
    void gammaChanged();
    void kneePointChanged();
    void kneeChanged();
    void saturationChanged();
    void colorTemperatureChanged();
    void gainChanged();
    void detailChanged();
    void shutterChanged();
    void ndFilterChanged();
    void redWhiteChanged();
    void blueWhiteChanged();
    void redBlackChanged();
    void blueBlackChanged();
};
/********************************************************************************************/
class XptControl : public Backend
{
    Q_OBJECT
    Q_PROPERTY(QStringList inputLabels READ inputLabels NOTIFY inputLabelsChanged)
    Q_PROPERTY(QList<int> xptSources READ xptSources WRITE setXptSources NOTIFY xptSourcesChanged)
    Q_PROPERTY(QStringList outputLabels READ outputLabels NOTIFY outputLabelsChanged)
    Q_PROPERTY(int xptDestination READ xptDestination WRITE setXptDestination NOTIFY xptDestinationChanged)
    Q_PROPERTY(QStringList ipAdress READ ipAdress WRITE setIpAdress NOTIFY ipAdressChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(int xptType READ xptType WRITE setXptType NOTIFY xptTypeChanged)

public:
    XptControl();
    XptControl(Model* model, Controller* controller);

    QStringList inputLabels();
    QList<int> xptSources();
    QStringList outputLabels();
    int xptDestination();
    QStringList ipAdress();
    Q_INVOKABLE void connectXpt(bool checked);
    void updateAll();
    bool connected();
    int xptType();
public slots:
    void setXptSources(QList<int> xptSources);
    void setXptDestination(int xptDestination);
    void setIpAdress(QStringList ipAdress);
    void setXptType(int xptType);
signals:
    void inputLabelsChanged();
    void xptSourcesChanged();
    void outputLabelsChanged();
    void xptDestinationChanged();
    void ipAdressChanged();
    void connectedChanged(bool connected);
    void xptTypeChanged();
};

class Controls : public Backend
{
    Q_OBJECT
    Q_PROPERTY(bool invertPan READ invertPan WRITE setInvertPan NOTIFY invertPanChanged)
    Q_PROPERTY(bool invertTilt READ invertTilt WRITE setInvertTilt NOTIFY invertTiltChanged)
    Q_PROPERTY(bool invertZoom READ invertZoom WRITE setInvertZoom NOTIFY invertZoomChanged)
    Q_PROPERTY(bool invertFocus READ invertFocus WRITE setInvertFocus NOTIFY invertFocusChanged)
    Q_PROPERTY(bool invertTravelling READ invertTravelling WRITE setInvertTravelling NOTIFY invertTravellingChanged)
    Q_PROPERTY(QList<int> control READ control NOTIFY controlChanged)
public:
    Controls();
    Controls(Model* model, Controller* controller);

    bool invertPan(){return m_model->getCamFlag(PanInverted);}
    bool invertTilt(){return m_model->getCamFlag(TiltInverted);}
    bool invertZoom(){return m_model->getCamFlag(ZoomInverted);}
    bool invertFocus(){return m_model->getCamFlag(FocusInverted);}
    bool invertTravelling(){return m_model->getCamFlag(TravellingInverted);}
    Q_INVOKABLE void setControl(int axis, int control){m_model->setControl(axis_t(axis),control_t(control));}
    Q_INVOKABLE int getControl(int axis){return m_model->getControl(axis_t(axis));}
    QList<int> control(){
        QList<int> list;
        for (axis_t axes : {KAxisPan, KAxisTilt, KAxisZoom, KAxisFocus, KAxisTravelling}) {
            list.append(m_model->getControl(axes));
        }
        return list;
    }
public slots:
    void setInvertPan(bool invertPan){m_model->setCamFlag(PanInverted,invertPan);}
    void setInvertTilt(bool invertTilt){m_model->setCamFlag(TiltInverted, invertTilt);}
    void setInvertZoom(bool invertZoom){m_model->setCamFlag(ZoomInverted, invertZoom);}
    void setInvertFocus(bool invertFocus){m_model->setCamFlag(FocusInverted, invertFocus);}
    void setInvertTravelling(bool invertTravelling){m_model->setCamFlag(TravellingInverted, invertTravelling);}
signals:
    void invertPanChanged();
    void invertTiltChanged();
    void invertZoomChanged();
    void invertFocusChanged();
    void invertTravellingChanged();
    void controlChanged();
};
/********************************************************************************************/
class Others : public Backend
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> headPower READ headPower NOTIFY headPowerChanged)
    Q_PROPERTY(QList<QVariant> mirror READ mirror NOTIFY mirrorChanged)
    Q_PROPERTY(QColor color WRITE setColor)
    Q_PROPERTY(QString errors READ errors NOTIFY errorChanged)

public:
    Others();
    Others(Model* model, Controller* controller);
    QList<QVariant> headPower(){return getDialValues(HeadPower);}
    QList<QVariant> mirror(){return getDialValues(Mirror);}
    QString errors();
    Q_INVOKABLE void setUpperLimit(){m_controller->queueEvent(E_SET_UPPER_LIMIT);}
    Q_INVOKABLE void setLowerLimit(){m_controller->queueEvent(E_SET_LOWER_LIMIT);}
    Q_INVOKABLE void calibrateHead(){m_controller->queueEvent(E_CALIB_HEAD);}
    Q_INVOKABLE void clearLimits(){m_controller->queueEvent(E_CLEAR_LIMIT);}
    Q_INVOKABLE void requestValues(){m_controller->queueEvent(E_REQUEST_SETTINGS);}
    Q_INVOKABLE void loadSaveFile(){m_controller->queueEvent(E_LOAD_AUTOSAVE);}
    Q_INVOKABLE void saveSaveFile(){m_controller->queueEvent(E_WRITE_AUTOSAVE);}
    void update() override;
public slots:
    void setColor(QColor color){
        m_model->color = color.rgb();
        m_controller->queueEvent(E_COLOR_CHANGED);
    }
signals:
    void headPowerChanged();
    void mirrorChanged();
    void errorChanged();
private:
    QStringList m_errors;

};


#endif // BACKEND_H
