#ifndef THERMOMETERDIALOG_H
#define THERMOMETERDIALOG_H

#include <QDialog>

namespace Ui {
class ThermometerDialog;
}

class ThermometerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThermometerDialog(QWidget *parent = 0);
    ~ThermometerDialog();
    void setAmbientTemp(QString);
    void setObjectTemp(QString);

private:
    Ui::ThermometerDialog *ui;
};

#endif // THERMOMETERDIALOG_H
