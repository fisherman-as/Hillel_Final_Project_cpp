#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "collatz.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void startCalculationSignal(std::size_t maxNumber, int threadCount);
    void stopCalculationSignal();

private slots:
    void Start();
    void Stop();
    void Exit();
    void SliderValueChanged();
    void calculationFinished(std::size_t startNumber, std::size_t numbersInSequence, std::size_t time);
    void calculationStoppedSlot();

private:
    Ui::MainWindow *ui;
    QThread* workerThread;
    CollatzWorker* worker;
};
#endif // MAINWINDOW_H
