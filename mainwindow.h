#pragma once

#include <QFuture>
#include <QMainWindow>
//#include <QProcess>

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

private slots:
  void onScanClicked();
  //  void onReadyRead();
  //  void onFinish(int exitCode, QProcess::ExitStatus exitStatus);
  //  void onError(QProcess::ProcessError error);
  void onFutureProgress(int progressValue);
  void onResultReadyAt(int index);
  void onStarted();
  void onFinished();

private:
  Ui::MainWindow *ui;
  int addr_ = 1;
  void startPing(int addr);
  //  void readSuccess(QString data);
  bool isScanned_ = false;
  QFuture<std::tuple<QString, int, int>> future_;
  QList<std::pair<QString, int>> addresses_;
};
