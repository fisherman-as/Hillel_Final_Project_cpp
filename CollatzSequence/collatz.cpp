#include "collatz.h"
#include <thread>
#include <vector>
#include <chrono>

extern bool stopRequested;

CollatzSequence::CollatzSequence(size_t startNumber) : _numbersInSequence(0), _startNumber(startNumber) {
}

CollatzSequence& CollatzSequence::operator=(const CollatzSequence& other)
{
    if (this != &other) {
        this->_startNumber = other._startNumber;
        this->_numbersInSequence = other._numbersInSequence;
        this->_overFlow = other._overFlow;
    }
    return *this;
}

size_t CollatzSequence::getNumbersInSequence() {
    return this->_numbersInSequence;
}

size_t CollatzSequence::calculate() {
    size_t result = this->_startNumber;
    while (result > 1) {
        if (result % 2 == 0) {
            result = result / 2;
        } else {
            if (result > MAXNUM) {
                this->_overFlow = true;
                return this->_numbersInSequence;
            }
            result = 3 * result + 1;
        }
        this->_numbersInSequence++;
    }
    return this->_numbersInSequence;
}

void CollatzSequence::ThreadFunc(std::mutex* threadsMutex, CollatzSequence* referenceObject, size_t maxNumber, int threadsCount,  int threadNumber) {
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

CollatzSequence CollatzSequence::mainFunc(size_t maxNumber, int threadCount) {
    std::mutex threadsMutex;
    CollatzSequence referenceObject(2);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back(ThreadFunc, &threadsMutex, &referenceObject, maxNumber, threadCount, i);
    }

    for (int i = 0; i < threadCount; i++) {
        threads[i].join();
    }
    return referenceObject;
}

CollatzWorker::CollatzWorker(QObject* parent) : QObject(parent) {
}

void CollatzWorker::run(std::size_t maxNumber, int threadCount) {
    auto startTime = std::chrono::high_resolution_clock::now();
    CollatzSequence result = CollatzSequence::mainFunc(maxNumber, threadCount);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::size_t time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    emit finished(result.getStartNumber(), result.getNumbersInSequence(), time);
}
