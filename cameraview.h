#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QObject>
#include "model.h"
#include "controller.h"
#include <QDebug>

class CameraView : public QObject
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
    ~CameraView();

    /*Dials */
    QList<QVariant> getIris();
    QList<QVariant> blackLevel();
    QList<QVariant> gammaTable();
    QList<QVariant> gamma();
    QList<QVariant> kneePoint();
    QList<QVariant> knee();
    QList<QVariant> saturation();
    QList<QVariant> colorTemperature();
    QList<QVariant> gain();
    QList<QVariant> detail();
    QList<QVariant> shutter();
    QList<QVariant> ndFilter();
    QList<QVariant> redWhite();
    QList<QVariant> blueWhite();
    QList<QVariant> redBlack();
    QList<QVariant> blueBlack();

    Q_INVOKABLE void setValue(int property, int value);
    Q_INVOKABLE void setField(int property);

    void updateAll();

public slots:
    void onNewSignalReceived(int property);

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

private:
    Model* model;
    Controller* controller;
};

#endif // CAMERABACKEND_H
