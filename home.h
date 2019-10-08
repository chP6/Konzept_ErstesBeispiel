#ifndef HOME_H
#define HOME_H

#include "model.h"
#include "controller.h"
#include "events.h"
#include <QObject>
#include "config.h"


class Home : public QObject
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
    Q_PROPERTY(int preset READ preset NOTIFY updateView)
    Q_PROPERTY(bool presetMoving READ presetMoving NOTIFY presetMovingChanged)
    Q_PROPERTY(bool bounce READ bounce WRITE setBounce NOTIFY bounceChanged)
    Q_PROPERTY(bool sppStart READ sppStart WRITE setSppStart NOTIFY sppStartChanged)
    Q_PROPERTY(bool fpm READ fpm WRITE setFpm NOTIFY fpmChanged)

public:
    explicit Home(Model *model, Controller *controller);
    ~Home();

    /*PT Speed Dial*/
    QList<QVariant> ptSpeed();
    QList<QVariant> zoomSpeed();
    QList<QVariant> transitionSpeed();
    QList<QVariant> ramp();
    QList<QVariant> spp1();
    QList<QVariant> spp2();
    QList<QVariant> sppWaitTime();
    int headNr();
    int preset();
    Q_INVOKABLE void storePreset();

    void updateAll();

    bool presetMoving();
    bool bounce();
    bool sppStart();
    bool fpm();

public slots:

    void setBounce(bool bounce);
    void setSppStart(bool sppStart);
    void setFpm(bool fpm);


signals:
    void updateView();
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

private:
    Model* model;
    Controller* controller;

    bool m_presetMoving;

    bool m_bounce;

    bool m_sppStart;

    bool m_fpm;

private slots:


};

#endif // HOME_H
