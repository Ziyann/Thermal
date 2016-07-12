#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QImage>

#include "thermaldata.h"

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();

signals:
    void signalResponse(const QByteArray &request, const QByteArray &response);
    void status(const QString &s);
    void signalDisplayThermalImage(ThermalData *data);

public slots:
    void connectPort(const QString &);
    void requestData(const QString &);
    void startFastThermometer();
    void stopFastThermometer();
    void updateFastThermometer();
    void startThermalImaging();

private:
    int requestDataSelf(const QString&);
    int sendDataSelf(const QString&);
    int setVerticalRotation(const int degree);
    int setHorizontalRotation(const int degree);
    int getObjectTempRaw();
    QImage* constructThermalImage(int tempData[], const int rows, const int cols);
};

#endif // WORKER_H
