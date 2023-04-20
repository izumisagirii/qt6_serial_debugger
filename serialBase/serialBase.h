#ifndef SERIALBASE_H
#define SERIALBASE_H

#include <QDebug>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
class SerialBase : public QSerialPort {
  Q_OBJECT
public:
  explicit SerialBase(QObject *parent = nullptr);
  //~SerialBase() { delete timerSerial; }
  enum serialPolicy {
    None,
    WaitTime,
    WaitByte,
    Both,
  };
  void setWaitTime(int msec);
  void setStopBytes(QByteArray stopBytes);
  void setStopPolicy(serialPolicy stopIf);
signals:
  void baseDataReady(QByteArray result);

private:
  QTimer *timerSerial;
  QByteArray buffer;
  QByteArray stopBytes;
  serialPolicy policy;
  qsizetype pointer;
  void baseEmitData();
public slots:
  void baseSendData(const QByteArray data);
private slots:
  void timerUpService();
  void baseReceiveData();
};

#endif