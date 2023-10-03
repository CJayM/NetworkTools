#pragma once

#include <QFuture>
#include <QFutureWatcher>
#include <QMainWindow>

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
  void onPauseClicked();

  void onFutureProgress(int progressValue);
  void onResultReadyAt(int index);
  void onStarted();
  void onFinished();

private:
  Ui::MainWindow *ui;
  QFuture<std::tuple<QString, int, int>> future_;
  QList<std::pair<QString, int>> addresses_;
  QFutureWatcher<std::tuple<QString, int, int>> *watcher = nullptr;
};
