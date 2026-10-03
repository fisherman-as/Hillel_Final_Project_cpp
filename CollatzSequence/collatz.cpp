#include "collatz.h"

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
