#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QProcess>
#include <QtConcurrent>
#include <utility>

QList<std::pair<QString, int>> collectAddresses(QString pref, int from,
                                                int to) {
  QList<std::pair<QString, int>> result;

  int pos = from;
  int indx = 0;
  while (pos < to) {
    result.append(std::make_pair(QString("%1%2").arg(pref).arg(pos), indx));
    ++pos;
    ++indx;
  }

  return result;
}

std::pair<QString, int> parsePingOutput(QString data, QString addr) {
  if (data.isNull())
    return std::make_pair<QString, int>("", -1);

  QString percent;
  int percentValue = -1;

  auto lines = data.split("\r\n");
  auto end = lines.size();
  int pos = 0;

  while (pos < end) {
    auto line = lines[pos];
    if (line.isEmpty() == false) {
      if (line.startsWith("Ping statistics for ")) {
        ++pos;
        auto text = lines[pos];
        percent = text.split(" ")[14].mid(1);
        percentValue = percent.mid(0, percent.size() - 1).toInt();
        break;
      }
    }

    ++pos;
  }

  return std::make_pair(addr, percentValue);
}

std::tuple<QString, int, int> ping(std::pair<QString, int> pair) {
  auto addr = pair.first;
  auto pos = pair.second;

  QString program = "ping";
  QStringList arguments;
  arguments << "-n"
            << "2"
            << "-w"
            << "10" << addr;

  QProcess process;
  process.start(program, arguments);
  bool isOk = process.waitForFinished(10000);
  auto data = process.readAllStandardOutput();
  auto res = parsePingOutput(data, addr);
  return std::make_tuple(res.first, res.second, pos);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->progressBar->hide();
  ui->tableWidget->setColumnCount(2);

  connect(ui->pushButton, &QPushButton::clicked, this,
          &MainWindow::onScanClicked);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onScanClicked() {
  if (isScanned_) {
    isScanned_ = false;
    ui->pushButton->setText("Scan");
    ui->progressBar->hide();
  } else {
    isScanned_ = true;
    ui->progressBar->show();
    ui->pushButton->setText("Stop");
    addresses_ = collectAddresses(ui->lineEdit->text(), ui->spinBox->value(),
                                  ui->spinBox_2->value());

    future_ = QtConcurrent::mapped(addresses_, ping);

    auto watcher = new QFutureWatcher<std::tuple<QString, int, int>>(this);
    connect(watcher, &QFutureWatcher<QString>::progressValueChanged, this,
            &MainWindow::onFutureProgress);
    connect(watcher, &QFutureWatcher<QString>::canceled, this,
            []() { qDebug() << "Canceled"; });
    connect(watcher, &QFutureWatcher<QString>::paused, this,
            []() { qDebug() << "Paused"; });
    connect(watcher, &QFutureWatcher<QString>::destroyed, this,
            []() { qDebug() << "Destroyed"; });
    connect(watcher, &QFutureWatcher<QString>::finished, this,
            &MainWindow::onFinished);

    connect(watcher, &QFutureWatcher<QString>::resultReadyAt, this,
            &MainWindow::onResultReadyAt);

    connect(watcher, &QFutureWatcher<QString>::resumed, this,
            []() { qDebug() << "Resumed"; });
    connect(watcher, &QFutureWatcher<QString>::started, this,
            &MainWindow::onStarted);

    watcher->setFuture(future_);
  }
}

void MainWindow::onFutureProgress(int progressValue) {
  ui->progressBar->setValue(100.0 / addresses_.size() * progressValue);
}

void MainWindow::onResultReadyAt(int index) {
  auto result = future_.resultAt(index);
  ui->statusbar->showMessage(
      QString("Processing output from %1...").arg(std::get<0>(result)));
  auto row = ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(row);
  ui->tableWidget->setItem(row, 0, new QTableWidgetItem(std::get<0>(result)));
  ui->tableWidget->setItem(
      row, 1, new QTableWidgetItem(QString::number(std::get<1>(result))));
}

void MainWindow::onStarted() {
  ui->progressBar->show();
  ui->pushButton->setText("Stop");
  ui->statusbar->showMessage(QString("Starting..."));
}

void MainWindow::onFinished() {
  ui->pushButton->setText("Scan");
  ui->progressBar->hide();
  ui->statusbar->showMessage(QString("Finish"));
}
