#pragma once

#include <QMainWindow>
#include <QProcess>

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
  void onFinish(int exitCode, QProcess::ExitStatus exitStatus);
  void onError(QProcess::ProcessError error);

private:
  Ui::MainWindow *ui;
  QProcess *gitProcess = nullptr;
  int addr_ = 1;
  void startPing(int addr);
  void readSuccess(QString data);
};
