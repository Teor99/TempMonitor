#include "SimpleRelay.h"

SimpleRelay::SimpleRelay(uint8_t pin, bool isInvertedLogicLevel) {
    this->pin = pin;
    this->isInvertedLogicLevel = isInvertedLogicLevel;

    pinMode(this->pin, OUTPUT);
    disableRelay();
}

void SimpleRelay::enableRelay() {
    digitalWrite(pin, isInvertedLogicLevel ? 0 : 1);
}

void SimpleRelay::disableRelay() {
    digitalWrite(pin, isInvertedLogicLevel ? 1 : 0);
}
