#ifndef SmartRelay_h
#define SmartRelay_h

#include "SimpleRelay.h"

class SmartRelay : SimpleRelay {
   public:
    SmartRelay(uint8_t pin,
               bool isInvertedLogicLevel,
               WidgetLED* stateLed,
               WidgetLED* errorLed,
               DallasTemperature* dsBus,
               uint8_t* deviceAddress,
               int vpinTemp);

    void updateState();
    void enableRelay();
    void disableRelay();

    void enableTempConrol();
    void disableTempConrol();

    void setTargerTemp(float targetTemp);
    void setHisteresis(float histeresis);
    void sendTempToClient();

    void enableErrorState();
    void disableErrorState();

   private:
    int vpinTemp;
    uint8_t* deviceAddress;
    DallasTemperature* dsBus;
    WidgetLED* stateLed;
    WidgetLED* errorLed;
    bool isError;
    bool isTempControlOn;
    float targetTemp;
    float currentTemp;
    float histeresis;
    uint32_t errorCount;
};

#endif
