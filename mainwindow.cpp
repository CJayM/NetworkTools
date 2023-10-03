#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QProcess>

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
  if (gitProcess == nullptr) {
    ui->progressBar->show();
    ui->pushButton->setText("Stop");
    startPing(ui->spinBox->value());
  } else {
    ui->pushButton->setText("Scan");
    ui->progressBar->hide();
    gitProcess->kill();
    gitProcess->deleteLater();
    gitProcess = nullptr;
  }
}

void MainWindow::readSuccess(QString normal) {
  if (normal.isNull())
    return;

  auto lines = normal.split("\r\n");
  auto end = lines.size();
  int pos = 0;
  while (pos < end) {
    auto line = lines[pos];
    if (line.isEmpty() == false) {

      if (line.startsWith("Ping statistics for ")) {
        ++pos;
        auto text = lines[pos];
        auto percents = text.split(" ")[14].mid(1);
        if (percents != "100%") {
          qDebug() << percents;
          auto row = ui->tableWidget->rowCount();
          ui->tableWidget->insertRow(row);
          ui->tableWidget->setItem(
              row, 0, new QTableWidgetItem(QString("192.168.1.%1").arg(addr_)));
          ui->tableWidget->setItem(row, 1, new QTableWidgetItem(percents));
        }
      }
    }

    auto len = ui->spinBox_2->value() - ui->spinBox->value();
    auto percent = 100.0 / len * (addr_ - ui->spinBox->value());
    ui->progressBar->setValue(percent);

    ++pos;
  }

  qDebug() << normal;
}

void MainWindow::onFinish(int exitCode, QProcess::ExitStatus exitStatus) {
  auto normal = gitProcess->readAllStandardOutput();
  auto error = gitProcess->readAllStandardError();
  if (error.isNull() == false) {
    qDebug() << "ERROR: " << error;
    //	ui->statusBar()->showMessage(QString("ERROR: %1").arg(QString(error)));
  }
  if (normal.isNull() == false) {
    readSuccess(normal);
    if (addr_ < ui->spinBox_2->value())
      startPing(addr_ + 1);
    else
      qDebug() << "FINISH";
  } else {
    gitProcess->deleteLater();
    gitProcess = nullptr;
  }
}

void MainWindow::onError(QProcess::ProcessError error) {
  qDebug() << "ERROR" << error;
  if (gitProcess != nullptr)
    qDebug() << gitProcess->readAllStandardOutput();
  //  gitProcess->deleteLater();
  //  gitProcess = nullptr;
}

void MainWindow::startPing(int addr) {
  addr_ = addr;
  ui->statusbar->showMessage(
      QString("Looking for %1").arg(QString("192.168.1.%1").arg(addr)));

  QString program = "ping";
  QStringList arguments;
  arguments << "-n"
            << "2"
            << "-w"
            << "10" << QString("192.168.1.%1").arg(addr);

  if (gitProcess != nullptr)
    gitProcess->deleteLater();

  gitProcess = new QProcess(this);

  connect(
      gitProcess,
      static_cast<void (QProcess::*)(
          int exitCode, QProcess::ExitStatus exitStatus)>(&QProcess::finished),
      this, &MainWindow::onFinish);
  //  connect(gitProcess, &QProcess::readyRead, this, &MainWindow::onReadyRead);
  connect(
      gitProcess,
      static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
      this, &MainWindow::onError);

  gitProcess->start(program, arguments);
}
