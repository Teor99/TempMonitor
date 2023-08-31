#ifndef SimpleRelay_h
#define SimpleRelay_h

#include <Arduino.h>

class SimpleRelay {
   public:
    SimpleRelay(uint8_t pin, bool isInvertedLogicLevel);
    void enableRelay();
    void disableRelay();

   protected:
    uint8_t pin;
    bool isInvertedLogicLevel;
};

#endif
