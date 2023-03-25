#ifndef SmartRelay_h
#define SmartRelay_h

#include <Arduino.h>
#include <BlynkSimpleEsp8266.h>
#include <DallasTemperature.h>

#define ERROR_COUNT_LIMIT 10
#define ON 1
#define OFF 0

class SmartRelay {
   public:
    SmartRelay(uint8_t pin,
               bool isInvertedLogicLevel,
               uint8_t stateLedVpin,
               uint8_t errorLedVpin,
               DallasTemperature* dsBus,
               DeviceAddress* deviceAddress,
               int vpinTemp) {
        this->pinRelay = pin;
        this->isInvertedLogicLevel = isInvertedLogicLevel;
        this->vpinTemp = vpinTemp;
        this->dsBus = dsBus;
        this->deviceAddress = deviceAddress;

        this->stateLed = new WidgetLED(stateLedVpin);
        this->stateLed->off();
        this->errorLed = new WidgetLED(errorLedVpin);
        this->errorLed->off();

        this->errorCount = 0;

        pinMode(this->pinRelay, OUTPUT);
        setState(OFF);
    }

    void updateState() {
        currentTemp = dsBus->getTempC(*deviceAddress);

        if (currentTemp == DEVICE_DISCONNECTED_C) {
            if (errorCount < ERROR_COUNT_LIMIT) {
                errorCount++;

                if (errorCount >= ERROR_COUNT_LIMIT) {
                    setState(OFF);
                    errorLed->on();
                }
            }
        } else {
            errorCount = 0;
            errorLed->off();

            if (isTempControlOn) {
                if (targetTemp > currentTemp + histeresis / 2) {
                    // целевая температура выше текущей - включаем реле
                    setState(ON);
                } else if (targetTemp < currentTemp - histeresis / 2) {
                    // целевая температура ниже текущей - выключаем реле
                    setState(OFF);
                }
            } else {
                setState(OFF);
            }
        }
    }

    void setTempConrolState(bool isTempControlOn) {
        this->isTempControlOn = isTempControlOn;
    }

    void setTargerTemp(float targetTemp) {
        this->targetTemp = targetTemp;
    }

    void setHisteresis(float histeresis) {
        this->histeresis = histeresis;
    }

    void sendTempToClient() {
        if (currentTemp != DEVICE_DISCONNECTED_C) {
            Blynk.virtualWrite(vpinTemp, currentTemp);
        }
    }

   private:
    uint8_t pinRelay;
    int vpinTemp;
    bool isInvertedLogicLevel;
    DeviceAddress* deviceAddress;
    DallasTemperature* dsBus;
    WidgetLED* stateLed;
    WidgetLED* errorLed;
    bool isTempControlOn;
    float targetTemp;
    float currentTemp;
    float histeresis;
    uint32_t errorCount;

    void setState(uint8_t newState) {
        newState = isInvertedLogicLevel ? !newState : newState;
        digitalWrite(pinRelay, newState);
        stateLed->setValue(newState);
    }
};

#endif
