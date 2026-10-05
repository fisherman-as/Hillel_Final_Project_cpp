#ifndef COLLATZ_H
#define COLLATZ_H

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <QObject>

class CollatzSequence {
public:
    explicit CollatzSequence(size_t startNumber);
    CollatzSequence& operator=(const CollatzSequence& other);
    size_t getNumbersInSequence();
    size_t getStartNumber() {return this->_startNumber;}
    size_t calculate(std::atomic<std::size_t>* pAllNumbersArray, std::atomic<bool>* stopRequested);
    size_t const static MAXNUM = static_cast<size_t>((SIZE_MAX - 1) / 3);
    static void ThreadFunc(std::mutex* threadsMutex, CollatzSequence* referenceObject,
                           size_t maxNumber, int threadsCount,  int threadNumber,
                           std::atomic<std::size_t>* pAllNumbersArray, std::atomic<bool>* stopRequested);
    static CollatzSequence mainFunc(size_t maxNumber, int threadCount, std::atomic<bool>* stopRequested);

private:
    size_t _startNumber;
    size_t _numbersInSequence = 1;
    bool _overFlow = false;
};

class CollatzWorker : public QObject {
    Q_OBJECT

public:
    explicit CollatzWorker(QObject* parent = nullptr);

public slots:
    void run(std::size_t maxNumber, int threadCount);
    void stop();

signals:
    void finished(std::size_t startNumber, std::size_t numbersInSequence, std::size_t time);
    void stopped();

private:
    std::atomic<bool> stopRequested{false};
};

#endif // COLLATZ_H
