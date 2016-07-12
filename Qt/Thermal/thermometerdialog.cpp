#include "thermometerdialog.h"
#include "ui_thermometerdialog.h"

ThermometerDialog::ThermometerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ThermometerDialog)
{
    ui->setupUi(this);
}

ThermometerDialog::~ThermometerDialog()
{
    delete ui;
}

void ThermometerDialog::setAmbientTemp(QString temperature) {
    ui->ambientTempLbl->setText(temperature);
}

void ThermometerDialog::setObjectTemp(QString temperature) {
    ui->objectTempLbl->setText(temperature);
}
