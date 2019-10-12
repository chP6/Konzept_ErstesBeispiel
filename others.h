#ifndef OTHERS_H
#define OTHERS_H

#include "model.h"
#include "controller.h"
#include "events.h"
#include <QObject>
#include <QColor>
#include <QDebug>

class Others : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> headPower READ headPower NOTIFY headPowerChanged)
    Q_PROPERTY(QList<QVariant> mirror READ mirror NOTIFY mirrorChanged)
    Q_PROPERTY(QColor color WRITE setColor)

public:
    Others();
    explicit Others(Model* model, Controller* controller);
    ~Others();

    QList<QVariant> headPower();
    QList<QVariant> mirror();
    Q_INVOKABLE void setUpperLimit();
    Q_INVOKABLE void setLowerLimit();
    Q_INVOKABLE void calibrateHead();
    Q_INVOKABLE void clearLimits();
    Q_INVOKABLE void requestValues();
    Q_INVOKABLE void loadSaveFile();
    Q_INVOKABLE void saveSaveFile();
    void updateAll();

public slots:
    void setColor(QColor color);


signals:
    void headPowerChanged();
    void mirrorChanged();

private slots:



private:

    Model* model;
    Controller* controller;
    QList<QVariant> m_headPower;
    QList<QVariant> m_mirror;
    QColor m_color;
};

#endif // OTHERS_H
