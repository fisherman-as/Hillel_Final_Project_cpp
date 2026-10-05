#include "collatz.h"
#include <thread>
#include <vector>
#include <chrono>

extern std::atomic<bool> stopRequested;

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

size_t CollatzSequence::calculate(std::atomic<std::size_t>* pAllNumbersArray) {
    size_t number = this->_startNumber;

    while (number > 1) {
        if (number % 2 == 0) {
            number = number >> 1;
        } else {
            if (number > MAXNUM) {
                this->_overFlow = true;
                return this->_numbersInSequence;
            }
            number = 3 * number + 1;
        }
        this->_numbersInSequence++;

        if (number < this->_startNumber && number > 4 && pAllNumbersArray[number] != 0) {
            this->_numbersInSequence += pAllNumbersArray[number];
            return this->_numbersInSequence;
        }
    }

    pAllNumbersArray[this->_startNumber] = this->_numbersInSequence;
    return this->_numbersInSequence;
}

void CollatzSequence::ThreadFunc(std::mutex* threadsMutex, CollatzSequence* referenceObject, size_t maxNumber, int threadsCount,  int threadNumber, std::atomic<std::size_t>* pAllNumbersArray) {
    for (size_t startNumber = threadNumber; startNumber <= maxNumber; startNumber += threadsCount) {
        if (stopRequested.load()) {
            return;
        }
        CollatzSequence object(startNumber);
        size_t numbersAmount =  object.calculate(pAllNumbersArray);
        std::lock_guard<std::mutex> guard(*threadsMutex);
        if (numbersAmount > referenceObject->getNumbersInSequence()) {
            *referenceObject = object;
        }
    }
}

CollatzSequence CollatzSequence::mainFunc(size_t maxNumber, int threadCount) {
    std::mutex threadsMutex;
    std::atomic<std::size_t>* pAllNumbersArray = new std::atomic<std::size_t>[maxNumber + 1] {0};
    CollatzSequence referenceObject(2);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (int threadNumber = 1; threadNumber <= threadCount; threadNumber++) {
        threads.emplace_back(ThreadFunc, &threadsMutex, &referenceObject, maxNumber, threadCount, threadNumber, pAllNumbersArray);
    }

    for (int i = 0; i < threadCount; i++) {
        threads[i].join();
    }
    delete[] pAllNumbersArray;
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
