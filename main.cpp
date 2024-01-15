#include "mainwindow.h"
#include <QApplication>
#include <qicon.h>

auto main(int argc, char *argv[]) -> int {
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowTitle("Protocol Debugger");
  // w.setWindowIcon(QIcon("./favicon.ico"));
  w.show();
  return QApplication::exec();
}
