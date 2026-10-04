#ifndef COLLATZ_H
#define COLLATZ_H

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <QObject>
#include <chrono>

class CollatzSequence {
public:
    explicit CollatzSequence(size_t startNumber);
    CollatzSequence& operator=(const CollatzSequence& other);
    size_t getNumbersInSequence();
    size_t getStartNumber() {return this->_startNumber;}
    size_t calculate();
    size_t const static MAXNUM = static_cast<size_t>((SIZE_MAX - 1) / 3);
    static void ThreadFunc(std::mutex* threadsMutex, CollatzSequence* referenceObject, size_t maxNumber, int threadsCount,  int threadNumber);
    static CollatzSequence mainFunc(size_t maxNumber, int threadCount);

private:
    size_t _startNumber;
    size_t _numbersInSequence = 0;
    bool _overFlow = false;
};

class CollatzWorker : public QObject {
    Q_OBJECT

public:
    explicit CollatzWorker(QObject* parent = nullptr);

public slots:
    void run(std::size_t maxNumber, int threadCount);

signals:
    void finished(std::size_t startNumber, std::size_t numbersInSequence, std::size_t time);
};

#endif // COLLATZ_H
