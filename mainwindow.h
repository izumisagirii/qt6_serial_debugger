#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QTime>
#include <qserialport.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <serialBase.h>

#define SERIAL_M_THREAD

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  SerialBase serial;
#ifdef SERIAL_M_THREAD
  QThread serialThread;
#endif
  void initSerialPortName();
  QString ByteArrayToHexString(QByteArray data);
  QByteArray HexStringToByteArray(QString HexString);

private slots:
  void on_button_openPort_clicked();
  void on_button_send_clicked();
  void on_button_clearSend_clicked();
  void on_check_waitTime_stateChanged(int checked);
  void on_check_endByte_stateChanged(int checked);
  void serialPort_readyRead(QByteArray result);
signals:
  void sendData(const QByteArray data);
};
#endif // MAINWINDOW_H
