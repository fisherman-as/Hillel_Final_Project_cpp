#ifndef COLLATZ_H
#define COLLATZ_H

#include <cstddef>
#include <cstdint>

class CollatzSequence {
public:
    explicit CollatzSequence(size_t startNumber);
    CollatzSequence& operator=(const CollatzSequence& other);
    size_t getNumbersInSequence();
    size_t getStartNumber() {return this->_startNumber;}
    size_t calculate();
    size_t const static MAXNUM = static_cast<size_t>((SIZE_MAX - 1) / 3);

private:
    size_t _startNumber;
    size_t _numbersInSequence;
    bool _overFlow = false;
};

#endif // COLLATZ_H
