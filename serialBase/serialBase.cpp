#include "serialBase.h"

SerialBase::SerialBase(QObject *parent) : QSerialPort(parent) {
  this->timerSerial = new QTimer(this);
  this->timerSerial->setSingleShot(true);
  this->timerSerial->setInterval(10);
  this->buffer.clear();
  this->policy = None;
  this->pointer = 0;
  QObject::connect(timerSerial, &QTimer::timeout, this,
                   &SerialBase::timerUpService);
  QObject::connect(this, &QSerialPort::readyRead, this,
                   &SerialBase::baseReceiveData);
}

void SerialBase::baseSendData(const QByteArray data) {
  qDebug() << "send";
  this->write(data);
};
void SerialBase::baseReceiveData() {
  auto temp = this->readAll();
  if (!this->stopBytes.isEmpty() && this->policy & serialPolicy::WaitByte) {
    for (auto &byte : temp) {
      this->buffer.append(byte);
      if (byte == this->stopBytes.at(pointer)) {
        pointer = pointer + 1;
      }
      if (pointer == this->stopBytes.length()) {
        baseEmitData();
        return;
      }
    }
  }
  if (this->policy & serialPolicy::WaitTime) {
    timerSerial->start();
  }
};
void SerialBase::baseEmitData() {
  timerSerial->stop();
  QByteArray temp = this->buffer;
  this->buffer.clear();
  emit baseDataReady(temp);
};

void SerialBase::timerUpService() {
  timerSerial->stop();
  baseEmitData();
};
void SerialBase::setWaitTime(int msec) {
  this->timerSerial->setInterval(msec);
};
void SerialBase::setStopBytes(QByteArray stopBytes) {
  this->stopBytes = stopBytes;
  this->pointer = 0;
}
void SerialBase::setStopPolicy(serialPolicy stopIf) { this->policy = stopIf; }
