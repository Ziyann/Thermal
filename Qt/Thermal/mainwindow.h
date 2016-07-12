#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QMouseEvent>

#include "controller.h"
#include "thermometerdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Controller *controller);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void showThermalImage(const QImage &);
    ThermometerDialog thermoDlg;
    bool eventFilter(QObject* watched, QEvent* event);

private slots:
    void processResponse(const QByteArray &, const QString &);
    void processStatus(const QString &);
    void processThermalImage(QImage *image);
    void on_thermometerButton_clicked();
    void on_pushButton_clicked();
    void on_checkBox_clicked();
    void on_checkBox_toggled(bool checked);
};

#endif // MAINWINDOW_H
