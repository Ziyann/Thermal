#include "mainwindow.h"
#include "ui_mainwindow.h"

QT_USE_NAMESPACE

#include <QLabel>

#include "setupdialog.h"

#include <QPainter>

#include <QDebug>

Controller *controller;

QImage getScale();

MainWindow::MainWindow(Controller *c) :
    QMainWindow(0),
    ui(new Ui::MainWindow)
{
    controller = c;

    ui->setupUi(this);

    // Controller -> Main window
    connect(controller, &Controller::signalDataReceived,
            this, &MainWindow::processResponse);
    connect(controller, &Controller::signalStatusChange,
            this, &MainWindow::processStatus);
    connect(controller, &Controller::signalDisplayThermalImage,
            this, &MainWindow::processThermalImage);

    SetupDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        controller->connectPort(dlg.getSelectedSerialPort());
        ui->statusBar->showMessage("Status: Connecting...");
    } else {
        exit(0);
    }

    ui->thermalImage->setStyleSheet("QLabel { background-color : black; }");
    ui->thermalImage->setMouseTracking(true);
    ui->thermalImage->installEventFilter(this);

    ui->thermalScale->setPixmap(QPixmap::fromImage(getScale()).scaled(ui->thermalScale->size(),
                                                                 Qt::KeepAspectRatio,
                                                                 Qt::FastTransformation));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched != ui->thermalImage)
        return false;
    if (event->type() != QEvent::MouseMove)
        return false;
    const QMouseEvent* const me = static_cast<const QMouseEvent*>(event);
    const QPoint p = me->pos();
    float verticalPercentage = (float) p.y() / ui->thermalImage->height();
    float horizontalPercentage = (float) p.x() / ui->thermalImage->width();
    qDebug() << controller->getTemperatureAtPosition(verticalPercentage, horizontalPercentage);
    return false;
}

void MainWindow::processThermalImage(QImage *image) {
    ui->thermalImage->setPixmap(QPixmap::fromImage(*image).scaled(ui->thermalImage->size(),
                                                                 Qt::KeepAspectRatio,
                                                                 Qt::SmoothTransformation));
}

void MainWindow::processResponse(const QByteArray &command, const QString &response)
{
    switch(command[0]) {
    case 'o':
        thermoDlg.setObjectTemp(response);
        break;
    case 'a':
        thermoDlg.setAmbientTemp(response);
        break;
    }
}

void MainWindow::processStatus(const QString &s)
{
    ui->statusBar->showMessage(tr("Status: %1.").arg(s));
}

void MainWindow::on_thermometerButton_clicked()
{
    controller->startFastThermometer();
    thermoDlg.exec();
    controller->stopFastThermometer();
}

void MainWindow::on_pushButton_clicked()
{
    controller->startThermalImaging();
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    QImage image = controller->getLastThermalImage();

    if (checked) {
        ui->thermalImage->setPixmap(QPixmap::fromImage(image).scaled(ui->thermalImage->size(),
                                                                     Qt::KeepAspectRatio,
                                                                     Qt::SmoothTransformation));
    } else {
        ui->thermalImage->setPixmap(QPixmap::fromImage(image).scaled(ui->thermalImage->size(),
                                                                     Qt::KeepAspectRatio,
                                                                     Qt::FastTransformation));
    }
}

void MainWindow::on_checkBox_clicked()
{

}

QImage getScale() {
        QImage thermalGradient(30, 600, QImage::Format_RGB32);
        QPainter painter(&thermalGradient);
        QLinearGradient gradient(QPointF(0,600), QPointF(0, 0));
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
        painter.fillRect(thermalGradient.rect(), gradient);

        return thermalGradient;
}
