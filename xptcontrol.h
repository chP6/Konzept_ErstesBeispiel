#ifndef XPTCONTROL_H
#define XPTCONTROL_H


#include "model.h"
#include "controller.h"
#include "config.h"
#include "events.h"
#include <QObject>

class XptControl : public QObject
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
    explicit XptControl(Model* model, Controller* controller);
    ~XptControl();

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

private slots:


private:

    Model* model;
    Controller* controller;
    QStringList m_ipAdress;
    QList<int> m_xptSources;
    int m_xptDestination;
    bool m_connected;
    int m_xptType;
};

#endif // XPTCONTROL_H
