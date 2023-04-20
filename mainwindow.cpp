#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "serialBase.h"
#include <qmainwindow.h>
#include <qtmetamacros.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->initSerialPortName();
  ui->box_baudRate->addItems({"2400", "9600", "19200", "57600", "115200",
                              "230400", "460800", "921600", "2250000",
                              "4500000"});
  ui->box_baudRate->setEditable(true);
  ui->box_dataBits->addItems({"8", "7", "6", "5"});
  ui->box_stopBit->addItems({"1", "1.5", "2"});
  ui->box_parityBit->addItems({"None", "Even", "Odd"});
  ui->box_flowControl->addItems({"None", "Hardware", "Software"});
  ui->button_openPort->setText("打开串口");
#ifdef SERIAL_M_THREAD
  // 将串口控制移入子线程
  serial.moveToThread(&this->serialThread);
  // 按照当前写法不需要销毁工作对象，因为会在析构函数自动销毁，否则会double free
  // 段错误
  //  connect(&serialThread, &QThread::finished, &serial,
  //  &QObject::deleteLater);
#endif
  connect(&serial, &SerialBase::baseDataReady, this,
          &MainWindow::serialPort_readyRead);
  connect(this, &MainWindow::sendData, &serial, &SerialBase::baseSendData);
#ifdef SERIAL_M_THREAD
  serialThread.start();
#endif
}

MainWindow::~MainWindow() {
#ifdef SERIAL_M_THREAD
  serialThread.quit();
  serialThread.wait();
#endif
  delete ui;
}

void MainWindow::initSerialPortName() {
  // 清空下拉框
  ui->box_portName->clear();
  // 通过QSerialPortInfo查找可用串口
  foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
    QString showName = info.portName() + ":" + info.description();
    ui->box_portName->addItem(showName);
  }
}

/*
 * @breif 将字节序列转换为对应的16进制字符串
 */
QString MainWindow::ByteArrayToHexString(QByteArray data) {
  QString ret(data.toHex().toUpper());
  int len = ret.length() / 2;
  for (int i = 1; i < len; i++) {
    ret.insert(2 * i + i - 1, " ");
  }
  return ret;
}

/*
 * @breif 将16进制字符串转换为对应的字节序列
 */
QByteArray MainWindow::HexStringToByteArray(QString HexString) {
  bool ok;
  QByteArray ret;
  HexString = HexString.trimmed();
  HexString = HexString.simplified();
  QStringList sl = HexString.split(" ");

  foreach (QString s, sl) {
    if (!s.isEmpty()) {
      char c = s.toInt(&ok, 16) & 0xFF;
      if (ok) {
        ret.append(c);
      } else {
        QMessageBox::warning(0, tr("错误："),
                             QString("非法的16进制字符: \"%1\"").arg(s));
      }
    }
  }
  return ret;
}
void MainWindow::serialPort_readyRead(QByteArray result) {
  // 获取当前时间字符串
  QDateTime current_date_time = QDateTime::currentDateTime();
  QString dateStr = current_date_time.toString("[yyyy-MM-dd hh:mm:ss.zzz]");

  QString bufferStr = ByteArrayToHexString(result);

  QString displayStr = dateStr + "\n[HEX]" + bufferStr + "\n[BA]" + result;

  // 显示
  ui->browser_dataReceive->append(displayStr);
}

void MainWindow::on_button_openPort_clicked() {
  if (!serial.isOpen()) {
    // 设置串口名
    QString portName = (ui->box_portName->currentText()).split(":").at(0);
    // qDebug() << portName;
    serial.setPortName(portName);

    // 设置波特率
    serial.setBaudRate(ui->box_baudRate->currentText().toInt());

    // 设置停止位
    switch (ui->box_stopBit->currentIndex()) {
    case 0:
      serial.setStopBits(QSerialPort::OneStop);
      break;
    case 1:
      serial.setStopBits(QSerialPort::OneAndHalfStop);
      break;
    case 2:
      serial.setStopBits(QSerialPort::TwoStop);
      break;
    default:;
    }

    // 设置数据位数
    switch (ui->box_dataBits->currentIndex()) {
    case 0:
      serial.setDataBits(QSerialPort::Data8);
      break;
    case 1:
      serial.setDataBits(QSerialPort::Data7);
      break;
    case 2:
      serial.setDataBits(QSerialPort::Data6);
      break;
    case 3:
      serial.setDataBits(QSerialPort::Data5);
      break;
    default:;
    }

    // 设置奇偶校验
    switch (ui->box_parityBit->currentIndex()) {
    case 0:
      serial.setParity(QSerialPort::NoParity);
      break;
    case 1:
      serial.setParity(QSerialPort::EvenParity);
      break;
    case 2:
      serial.setParity(QSerialPort::OddParity);
      break;
    default:;
    }

    // 设置流控制
    switch (ui->box_flowControl->currentIndex()) {
    case 0:
      serial.setFlowControl(QSerialPort::FlowControl::NoFlowControl);
      break;
    case 1:
      serial.setFlowControl(QSerialPort::FlowControl::HardwareControl);
      break;
    case 2:
      serial.setFlowControl(QSerialPort::FlowControl::SoftwareControl);
      break;
    default:;
    }

    // 打开串口
    if (!serial.open(QIODevice::ReadWrite)) {
      QMessageBox::about(NULL, "提示", "无法打开串口！");
      return;
    }

    // 下拉菜单控件失能
    ui->box_portName->setEnabled(false);
    ui->box_baudRate->setEnabled(false);
    ui->box_dataBits->setEnabled(false);
    ui->box_parityBit->setEnabled(false);
    ui->box_stopBit->setEnabled(false);
    ui->box_flowControl->setEnabled(false);

    ui->button_openPort->setText(QString("关闭串口"));
    // 发送按键使能
    ui->button_send->setEnabled(true);
  } else {
    // 关闭串口
    serial.close();

    // 下拉菜单控件使能
    ui->box_portName->setEnabled(true);
    ui->box_baudRate->setEnabled(true);
    ui->box_dataBits->setEnabled(true);
    ui->box_parityBit->setEnabled(true);
    ui->box_stopBit->setEnabled(true);
    ui->box_flowControl->setEnabled(true);

    ui->button_openPort->setText(QString("打开串口"));
    // 发送按键失能
    ui->button_send->setEnabled(false);
  }
}

void MainWindow::on_button_send_clicked() {
  // 获取界面上的数据并转换成utf8格式的字节流
  QByteArray data;

  if (ui->check_hexSend->isChecked())
    data = HexStringToByteArray(ui->text_dataSend->toPlainText());
  else
    data = ui->text_dataSend->toPlainText().toUtf8();

  emit sendData(data);
}
void MainWindow::on_button_clearSend_clicked() { ui->text_dataSend->clear(); }

void MainWindow::on_check_waitTime_stateChanged(int checked) {
  if (checked) {
    serial.setStopPolicy(ui->check_endByte->isChecked()
                             ? SerialBase::serialPolicy::Both
                             : SerialBase::serialPolicy::WaitTime);
    serial.setWaitTime(ui->lineEdit_waitTime->text().toInt());
  } else {
    serial.setStopPolicy(ui->check_endByte->isChecked()
                             ? SerialBase::serialPolicy::WaitByte
                             : SerialBase::serialPolicy::None);
  }
}

void MainWindow::on_check_endByte_stateChanged(int checked) {
  if (checked) {
    serial.setStopPolicy(ui->check_waitTime->isChecked()
                             ? SerialBase::serialPolicy::Both
                             : SerialBase::serialPolicy::WaitByte);
    serial.setStopBytes(HexStringToByteArray(ui->lineEdit_setByte->text()));
  } else {
    serial.setStopPolicy(ui->check_waitTime->isChecked()
                             ? SerialBase::serialPolicy::WaitTime
                             : SerialBase::serialPolicy::None);
  }
}