#include "setupdialog.h"
#include "ui_setupdialog.h"

#include <QtSerialPort/QSerialPortInfo>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->serialPortComboBox->addItem(info.portName());
}

SetupDialog::~SetupDialog()
{
    delete ui;
}

QString SetupDialog::getSelectedSerialPort() {
    return ui->serialPortComboBox->currentText();
}
