#include "SmartRelay.h"

#include <Arduino.h>
#include <DallasTemperature.h>

#define ERROR_COUNT_LIMIT 10

SmartRelay::SmartRelay(uint8_t pin,
                    bool isInvertedLogicLevel,
                    WidgetLED* stateLed,
                    WidgetLED* errorLed,
                    DallasTemperature* dsBus,
                    uint8_t* deviceAddress,
                    int vpinTemp)
            : SimpleRelay(pin, isInvertedLogicLevel) {
    this->dsBus = dsBus;
    this->vpinTemp = vpinTemp;
    this->deviceAddress = deviceAddress;
    this->stateLed = stateLed;
    this->errorLed = errorLed;

    disableRelay();
    disableErrorState();
}

void SmartRelay::updateState() {
    currentTemp = dsBus->getTempC(deviceAddress);

    if (currentTemp == DEVICE_DISCONNECTED_C) {
        if (errorCount < ERROR_COUNT_LIMIT) {
            errorCount++;

            if (errorCount >= ERROR_COUNT_LIMIT) {
                enableErrorState();
            }
        }
    } else {
        disableErrorState();
    }

    if (isTempControlOn && !isError) {
        if (targetTemp > currentTemp + histeresis / 2) {
            // целевая температура выше текущей - включаем реле
            enableRelay();
        } else if (targetTemp < currentTemp - histeresis / 2) {
            // целевая температура ниже текущей - выключаем реле
            disableRelay();
        }
    } else {
        disableRelay();
    }
}

void SmartRelay::enableRelay() {
    stateLed->setValue(255);
    SimpleRelay::enableRelay();
}

void SmartRelay::disableRelay() {
    stateLed->setValue(0);
    SimpleRelay::disableRelay();
}

void SmartRelay::enableTempConrol() {
    this->isTempControlOn = true;
}

void SmartRelay::disableTempConrol() {
    this->isTempControlOn = false;
}

void SmartRelay::setTargerTemp(float targetTemp) {
    this->targetTemp = targetTemp;
}

void SmartRelay::setHisteresis(float histeresis) {
    this->histeresis = histeresis;
}

void SmartRelay::sendTempToClient() {
    if (currentTemp != DEVICE_DISCONNECTED_C) {
        Serial.print("Temp: ");
        Serial.print(currentTemp);
        Serial.print(" Vpin: ");
        Serial.println(vpinTemp);
        Blynk.virtualWrite(vpinTemp, currentTemp);
    }
}

void SmartRelay::enableErrorState() {
    this->isError = true;
    disableRelay();
    errorLed->on();
}

void SmartRelay::disableErrorState() {
    errorLed->off();
    errorCount = 0;
}
