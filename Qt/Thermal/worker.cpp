#include "worker.h"

#include <QtSerialPort/QSerialPort>
#include <QThread>

#include <QDebug>
#include <QTime>
#include <QTimer>
#include <climits>
#include <QPainter>
#include <QMutex>

QSerialPort *serial;
QTimer *timer;
QMutex mutex;
QImage *thermalGradient;

Worker::Worker() {
    thermalGradient = new QImage(600, 1, QImage::Format_RGB32);
    QPainter painter(thermalGradient);
    QLinearGradient gradient(QPointF(0,0), QPointF(600, 0));
    gradient.setColorAt(0.0, qRgb(0,   0,   42));
    gradient.setColorAt(0.1, qRgb(32,  0,   132));
    gradient.setColorAt(0.2, qRgb(108, 0,   156));
    gradient.setColorAt(0.3, qRgb(173, 0,   153));
    gradient.setColorAt(0.4, qRgb(206, 24,  126));
    gradient.setColorAt(0.5, qRgb(229, 68,  25));
    gradient.setColorAt(0.6, qRgb(242, 108, 1));
    gradient.setColorAt(0.7, qRgb(251, 152, 0));
    gradient.setColorAt(0.8, qRgb(254, 200, 0));
    gradient.setColorAt(0.9, qRgb(255, 235, 77));
    gradient.setColorAt(1.0, qRgb(255, 255, 246));
    painter.fillRect(thermalGradient->rect(), gradient);
}

void Worker::connectPort(const QString &portName) {
    serial = new QSerialPort();
    serial->setPortName(portName);
    serial->setBaudRate(115200);
    if (!serial->open(QIODevice::ReadWrite)) {
        emit status(tr("Can't open %1, error code %2")
                   .arg(portName).arg(serial->error()));
    } else {
        emit status(tr("Connected to %1")
                   .arg(portName));
    }
}

void Worker::startFastThermometer() {
    if (timer == NULL) {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateFastThermometer()));
        timer->start(25);
    }
}

void Worker::updateFastThermometer() {
    requestData("a");
    requestData("o");
}

void Worker::stopFastThermometer() {
    if (timer != NULL) {
        timer->stop();
        delete timer;
        timer = NULL;
    }
}

QImage* Worker::constructThermalImage(int *tempData, const int rows, const int cols) {
    QImage *image = new QImage(rows, cols, QImage::Format_RGB32);

    int min = INT_MAX, max = INT_MIN;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int val = tempData[(i * rows) + j];
            if (val == -1) continue;
            if (val > max) max = val;
            if (val < min) min = val;
        }
    }

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (tempData[(row * cols) + col] == -1) {
                image->setPixel(col, rows - row - 1, qRgb(0, 0, 0));
                continue;
            }
            int val = 599 - ((max - tempData[(row * cols) + col]) / (float)(max - min)) * 599;
            image->setPixel(col, rows - row - 1, thermalGradient->pixel(val, 0));
        }
    }

    return image;
}

void Worker::startThermalImaging() {
    const int verticalZero = 5 * 10;
    const int rows = 30, cols = 30;
    const int scanningDegree = 40 * 10;
    const int rotationDegree = scanningDegree / rows;
    QImage *image;
    ThermalData *data;

    int *tempData = new int[rows * cols];
    for (int i = 0; i < rows * cols; ++i)
        tempData[i] = -1;

    //bool invert = false;
    for (int row = 0; row < rows; ++row) {
        int desiredVerticalRotation = (row * rotationDegree) - (scanningDegree / 2) + verticalZero;
        setVerticalRotation(desiredVerticalRotation);
        qDebug() << "Vertical rotation:" << desiredVerticalRotation;
        for (int col = 0; col < cols; ++col) {
        //    if (!invert) {
                setHorizontalRotation((col * rotationDegree) - (scanningDegree / 2));
                if (col == 0) QThread::msleep(300); // wait for servo rotation
                QThread::msleep(60);
                tempData[(row * cols) + col] = getObjectTempRaw();
                image = constructThermalImage(tempData, rows, cols);
                data = new ThermalData(image, rows, cols, tempData);
                emit signalDisplayThermalImage(data);
        //    } else {
        //        setHorizontalRotation(-((col * rotationDegree) - (scanningDegree / 2)));
        //        QThread::msleep(60);
        //        tempData[(row * cols) + (cols - col - 1)] = getObjectTempRaw();
        //    }
        }
        //invert = !invert;
    }

    sendDataSelf("r");

    image = constructThermalImage(tempData, rows, cols);
    data = new ThermalData(image, rows, cols, tempData);
    emit signalDisplayThermalImage(data);
}

int Worker::getObjectTempRaw() {
    return requestDataSelf("o");
}

int Worker::setHorizontalRotation(const int degree) {
    return sendDataSelf("h " + QString::number(degree));
}

int Worker::setVerticalRotation(const int degree) {
    return sendDataSelf("v " + QString::number(degree));
}

void Worker::requestData(const QString &command) {
    mutex.lock();

    // write request
    QByteArray request = command.toLocal8Bit();
    serial->write(request);
    serial->write("\n");

    // read response
    if (serial->waitForBytesWritten(10000)) {
        if (serial->waitForReadyRead(10000)) {
            QByteArray response = serial->readAll();
            while (serial->waitForReadyRead(50) || !response.endsWith('\n')) {
                response += serial->readAll();
            }
            response.chop(1);
            emit signalResponse(request, response);
        } else {
            emit status(tr("Timeout waiting for response %1")
                         .arg(QTime::currentTime().toString()));
        }
    } else {
        emit status(tr("Timeout waiting for write request: %1")
                     .arg(QTime::currentTime().toString()));
    }

    mutex.unlock();
}

int Worker::requestDataSelf(const QString &command) {
    int ret;

    mutex.lock();

    // write request
    QByteArray request = command.toLocal8Bit();
    serial->write(request);
    serial->write("\n");

    // read response
    if (serial->waitForBytesWritten(10000)) {
        if (serial->waitForReadyRead(10000)) {
            QByteArray response = serial->readAll();
            while (serial->waitForReadyRead(10) || !response.endsWith('\n')) {
                response += serial->readAll();
            }
            response.chop(1);
            ret = response.toInt();
        } else {
            ret = -2;
        }
    } else {
        ret = -1;
    }

    mutex.unlock();

    return ret;
}

int Worker::sendDataSelf(const QString &data) {
    int ret;

    mutex.lock();

    // write data
    QByteArray dat = data.toLocal8Bit();
    serial->write(dat);
    serial->write("\n");

    // wait for the write operation to complete
    if (serial->waitForBytesWritten(10000)) {
        ret = 0;
    } else {
        ret = -1;
    }

    mutex.unlock();

    return ret;
}
