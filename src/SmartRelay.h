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
               uint8_t* deviceAddress,
               int vpinTemp) {
        this->pinRelay = pin;
        this->isInvertedLogicLevel = isInvertedLogicLevel;
        this->vpinTemp = vpinTemp;
        this->dsBus = dsBus;
        this->deviceAddress = deviceAddress;
        this->stateLed = new WidgetLED(stateLedVpin);
        this->errorLed = new WidgetLED(errorLedVpin);

        pinMode(this->pinRelay, OUTPUT);
        setPinState(OFF);
        setErrorState(OFF);
    }

    void updateState() {
        currentTemp = dsBus->getTempC(deviceAddress);

        if (currentTemp == DEVICE_DISCONNECTED_C) {
            if (errorCount < ERROR_COUNT_LIMIT) {
                errorCount++;

                if (errorCount >= ERROR_COUNT_LIMIT) {
                    setErrorState(ON);
                }
            }
        } else {
            setErrorState(OFF);
        }

        if (isTempControlOn && getErrorState() == false) {
            if (targetTemp > currentTemp + histeresis / 2) {
                // целевая температура выше текущей - включаем реле
                setPinState(ON);
            } else if (targetTemp < currentTemp - histeresis / 2) {
                // целевая температура ниже текущей - выключаем реле
                setPinState(OFF);
            }
        } else {
            setPinState(OFF);
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

    void setErrorState(bool isError) {
        this->isError = isError;

        if (isError) {
            setPinState(OFF);
            errorLed->on();
        } else {
            errorLed->off();
            errorCount = 0;
        }
    }

    bool getErrorState() {
        return this->isError;
    }

   private:
    uint8_t pinRelay;
    int vpinTemp;
    bool isInvertedLogicLevel;
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

    void setPinState(bool newState) {
        stateLed->setValue(newState * 255);
        digitalWrite(pinRelay, isInvertedLogicLevel ? !newState : newState);
    }
};

#endif
