#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QImage>

#include "thermaldata.h"

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller();
    ~Controller();
    void connectPort(const QString &);
    void requestData(const QString &);
    void startFastThermometer();
    void stopFastThermometer();
    void startThermalImaging();
    QImage getLastThermalImage();
    float getTemperatureAtPosition(float verticalPercentage, float horizontalPercentage);

signals:
    // for the worker thread
    void operate(const QString &);
    void signalConnectPort(const QString &);
    void signalRequestData(const QString &);
    void signalStartFastThermometer();
    void signalStopFastThermometer();
    void signalStartThermalImaging();
    // for the UI thread
    void signalStatusChange(const QString &);
    void signalDataReceived(const QByteArray &request, const QString &response);
    void signalDisplayThermalImage(QImage *image);

public slots:
    void handleStatus(const QString &);
    void handleResponse(const QByteArray &, const QByteArray &);
    void handleThermalData(ThermalData *data);
};

#endif // CONTROLLER_H
