#ifndef SimpleRelay_h
#define SimpleRelay_h

#include <Arduino.h>

#define ON 1
#define OFF 0

class SimpleRelay {
   public:
    SimpleRelay(uint8_t pin, bool isInvertedLogicLevel) {
        this->pin = pin;
        this->isInvertedLogicLevel = isInvertedLogicLevel;

        pinMode(this->pin, OUTPUT);
        setPinState(OFF);
    }

    void setPinState(bool newState) {
        digitalWrite(pin, isInvertedLogicLevel ? !newState : newState);
    }

   private:
    uint8_t pin;
    bool isInvertedLogicLevel;
};

#endif
