#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <thread>
#include "collatz.h"

std::atomic<bool> stopRequested = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , workerThread(new QThread(this))
    , worker(new CollatzWorker())
{
    ui->setupUi(this);

    //EXIT BUTTON
    connect(ui->ExitButton, &QPushButton::clicked, this, &MainWindow::Exit);

    //SLIDER
    int threadsQuantity = std::thread::hardware_concurrency();
    ui->ThreadsQuantitySlider->setRange(1, threadsQuantity);
    ui->ThreadsQuantitySlider->setSliderPosition(threadsQuantity);
    ui->textBrowser_2->setText(QString::number(ui->ThreadsQuantitySlider->sliderPosition()));
    connect(ui->ThreadsQuantitySlider, &QSlider::valueChanged, this, &MainWindow::SliderValueChanged );

    //START BUTTON
    connect(ui->StartButton, &QPushButton::clicked, this, &MainWindow::Start);
    worker->moveToThread(workerThread);
    connect(this, &MainWindow::startCalculationSignal, worker, &CollatzWorker::run);
    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &CollatzWorker::finished, this, &MainWindow::calculationFinished);
    workerThread->start();

    //STOP BUTTON
    connect(ui->StopButton, &QPushButton::clicked, this, &MainWindow::Stop);
    ui->StopButton->setEnabled(false);
    connect(this, &MainWindow::finishedSignal, this, &MainWindow::calculationFinishedSlot);
    // connect(this, &MainWindow::finishedSignal, worker, &CollatzWorker::setStopRequested);
    // connect(worker, &CollatzWorker::setStopRequested, worker, &CollatzWorker::setStopRequested);
}

MainWindow::~MainWindow()
{
    workerThread->quit();
    workerThread->wait();
    delete ui;
}

void MainWindow::Start() {
    ui->StartButton->setEnabled(false);
    ui->StopButton->setEnabled(true);
    ui->textBrowser->clear();
    stopRequested = false;

    size_t maxNumber = ui->spinBox->value();
    int threadCount = ui->ThreadsQuantitySlider->value();

    emit startCalculationSignal(maxNumber, threadCount);
}

void MainWindow::Stop() {
    ui->StopButton->setEnabled(false);
    ui->StartButton->setEnabled(true);
    stopRequested = true;
    emit finishedSignal(stopRequested);
}

void MainWindow::Exit() {
    this->close();
}

void MainWindow::SliderValueChanged() {
    ui->textBrowser_2->setText(QString::number(ui->ThreadsQuantitySlider->sliderPosition()));
}

void MainWindow::calculationFinished(std::size_t startNumber, std::size_t numbersInSequence, std::size_t time) {
    if (!stopRequested) {
        ui->textBrowser->setText(QString("The number with the longest Collatz sequence:\n""Start number: %1\n"
                                         "Sequence length: %2\n""Time: %3 ms\n").arg(startNumber).arg(numbersInSequence).arg(time));
    }
    ui->StartButton->setEnabled(true);
    ui->StopButton->setEnabled(false);
}

void MainWindow::calculationFinishedSlot(bool stopRequested) {
    ui->textBrowser->setText(QString("Stopped by user...\n"));
}
