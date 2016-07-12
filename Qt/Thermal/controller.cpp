#include "controller.h"

#include <QThread>
#include <QDebug>
#include "worker.h"
#include "mainwindow.h"
#include "commands.h"

#include <QPainter>

QThread workerThread;

ThermalData *lastData;
int imageRows, imageCols;

Controller::Controller() {
    Worker *worker = new Worker();
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);

    // controller -> worker
    connect(this, &Controller::signalConnectPort,
            worker, &Worker::connectPort);
    connect(this, &Controller::signalRequestData,
            worker, &Worker::requestData);
    connect(this, &Controller::signalStartFastThermometer,
            worker,&Worker::startFastThermometer);
    connect(this, &Controller::signalStopFastThermometer,
            worker, &Worker::stopFastThermometer);
    connect(this, &Controller::signalStartThermalImaging,
            worker, &Worker::startThermalImaging);

    // worker -> controller
    connect(worker, &Worker::status,
            this, &Controller::handleStatus);
    connect(worker, &Worker::signalResponse,
            this, &Controller::handleResponse);
    connect(worker, &Worker::signalDisplayThermalImage,
            this, &Controller::handleThermalData);

    workerThread.start();
}

Controller::~Controller() {
    workerThread.quit();
    workerThread.wait();
}

void Controller::connectPort(const QString &portName) {
    emit signalConnectPort(portName);
}

void Controller::requestData(const QString &command) {
    emit signalRequestData(command);
}

void Controller::startFastThermometer() {
    emit signalStartFastThermometer();
}

void Controller::stopFastThermometer() {
    emit signalStopFastThermometer();
}

void Controller::startThermalImaging() {
    emit signalStartThermalImaging();
}

void Controller::handleStatus(const QString &result) {
    emit signalStatusChange(result);
}

void Controller::handleResponse(const QByteArray &request, const QByteArray &response) {
    QString resp;
    if (request == CMD_GET_AMB_TEMP || request == CMD_GET_OBJ_TEMP) {
        float temp = response.toFloat();
        temp /= 100;
        resp = QString::number(temp);
        resp += " °C";
    }
    emit signalDataReceived(request, resp);
}

void Controller::handleThermalData(ThermalData *data) {
    if (lastData != NULL) {
        if (lastData->thermalImage != NULL)
            delete lastData->thermalImage;
        delete lastData;
    }

    lastData = data;

    emit signalDisplayThermalImage(lastData->thermalImage);
}

QImage Controller::getLastThermalImage() {
    return *lastData->thermalImage;
}

float Controller::getTemperatureAtPosition(float verticalPercentage, float horizontalPercentage) {
    if (lastData == NULL) return 0;
    int row = (int)((1.0 - verticalPercentage) * (float)lastData->rows);
    int col = (int)(horizontalPercentage * (float)lastData->cols);
    return lastData->temperatureData[(row * lastData->rows) + (col)] / 100.0;
}
