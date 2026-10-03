#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <thread>
#include <vector>
#include <mutex>
#include "collatz.h"

std::atomic<bool> stopRequested = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->StartButton, &QPushButton::clicked, this, &MainWindow::Start);
    connect(ui->StopButton, &QPushButton::clicked, this, &MainWindow::Stop);
    connect(ui->ExitButton, &QPushButton::clicked, this, &MainWindow::Exit);
    ui->StopButton->setEnabled(false);

    int threadsQuantity = std::thread::hardware_concurrency();
    ui->ThreadsQuantitySlider->setRange(1, threadsQuantity);
    ui->textBrowser_2->setText(QString::number(1));
    connect(ui->ThreadsQuantitySlider, &QSlider::valueChanged, this, [this](int value) {MainWindow::SliderValueChanged(value);} );

}

MainWindow::~MainWindow()
{
    delete ui;
}

static void ThreadFunc(std::mutex* threadsMutex, CollatzSequence* referenceObject, size_t maxNumber, int threadsCount,  int threadNumber) {
    for (size_t i = threadNumber; i < maxNumber; i += threadsCount) {
        if (stopRequested) {
            return;
        }
        CollatzSequence object(i);
        size_t numbersAmount =  object.calculate();
        std::lock_guard<std::mutex> guard(*threadsMutex);
        if (numbersAmount > referenceObject->getNumbersInSequence()) {
            *referenceObject = object;
        }
    }
}

void MainWindow::Start() {
    ui->StartButton->setEnabled(false);
    ui->StopButton->setEnabled(true);

    std::mutex threadsMutex;
    size_t maxNumber = ui->spinBox->value();
    CollatzSequence referenceObject = CollatzSequence(2);

    int threadCount = ui->ThreadsQuantitySlider->value();
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back(ThreadFunc, &threadsMutex, &referenceObject, maxNumber, threadCount, i);
    }
    for (int i = 0; i < threadCount; i++) {
        threads[i].join();
    }
    ui->textBrowser->setText(QString::number(referenceObject.getStartNumber()));
}

void MainWindow::Stop() {
    ui->StopButton->setEnabled(false);
    ui->StartButton->setEnabled(true);
    stopRequested = true;
}

void MainWindow::Exit() {
    this->close();
}

void MainWindow::SliderValueChanged(int value)
{
    ui->textBrowser_2->setText(QString::number(value));
}

