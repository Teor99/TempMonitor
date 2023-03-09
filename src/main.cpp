/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// DS18b20
#include <DallasTemperature.h>
#include <OneWire.h>

// DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// маски реальных пинов nodeMCU
#define PIN_DHT11 D1
#define PIN_DS18B20 D2

#define PIN_RELAY1 D5
#define PIN_RELAY2 D6
#define PIN_RELAY3 D0
#define PIN_RELAY4 D7

// маски виртуальных пинов BLYNK
#define BLYNK_VPIN_DS18B20_1 V1
#define BLYNK_VPIN_RELAY1_TEMP_CONTROL_SWITCH V2
#define BLYNK_VPIN_RELAY1_TARGET_TEMP_SLIDER V3
#define BLYNK_VPIN_RELAY1_HISTERESIS_SLIDER V4
#define BLYNK_VPIN_RELAY1_LED V5

#define BLYNK_VPIN_DS18B20_2 V6
#define BLYNK_VPIN_RELAY2_TEMP_CONTROL_SWITCH V7
#define BLYNK_VPIN_RELAY2_TARGET_TEMP_SLIDER V8
#define BLYNK_VPIN_RELAY2_HISTERESIS_SLIDER V9
#define BLYNK_VPIN_RELAY2_LED V10

#define BLYNK_VPIN_DHT11_TEMP V20
#define BLYNK_VPIN_DHT11_HUMIDITY V21

#define BLYNK_VPIN_RELAY3_SWITCH V25
#define BLYNK_VPIN_RELAY4_SWITCH V26

#define BLYNK_VPIN_SAVE_CONFIG_BUTTON V30

// уровень логического сигнала для реле (на случай, если реле управляется инвертированым лог. сигналом)
bool isInvertRelayLogicLevel = true;

char auth[] = "please_enter_your_blynk_token";
char ssid[] = "please_enter_your_wifi_ssid";
char pass[] = "please_enter_your_wifi_password";
#define SERVER_IP_ADDRESS IPAddress(192, 168, 0, 101)
#define SERVER_PORT 8080

BlynkTimer timer;

// DS18b20
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);
DeviceAddress tempSensor1Address = {0x28, 0x76, 0x1F, 0x56, 0xB5, 0x01, 0x3C, 0xA6};
DeviceAddress tempSensor2Address = {0x28, 0xFF, 0x04, 0xD4, 0x44, 0x16, 0x03, 0x53};

// DHT11
DHT_Unified dht(PIN_DHT11, DHT11);

// LEDS
WidgetLED relay1Led(BLYNK_VPIN_RELAY1_LED);
WidgetLED relay2Led(BLYNK_VPIN_RELAY2_LED);

// Структура для хранения параметров контроля по температуре для реле
struct RelayControlStateStruct {
    bool isTempControlOn;
    bool isRelayOn;
    float targetTemp;
    float currentTemp;
    float histeresis;
};

RelayControlStateStruct relay1ControlState;
RelayControlStateStruct relay2ControlState;

// Установить состояние пина реле
void setRelayState(uint8_t pin, uint8_t logicLevel) {
    digitalWrite(pin, isInvertRelayLogicLevel ? !logicLevel : logicLevel);
}

// Обработчик нажатия на переключатель контроля по температуре для реле1
BLYNK_WRITE(BLYNK_VPIN_RELAY1_TEMP_CONTROL_SWITCH) {
    relay1ControlState.isTempControlOn = param.asInt();
}

// Обработчик нажатия на переключатель контроля по температуре для реле2
BLYNK_WRITE(BLYNK_VPIN_RELAY2_TEMP_CONTROL_SWITCH) {
    relay2ControlState.isTempControlOn = param.asInt();
}

// Обработчик нажатия на переключатель реле3
BLYNK_WRITE(BLYNK_VPIN_RELAY3_SWITCH) {
    setRelayState(PIN_RELAY3, param.asInt());
}

// Обработчик нажатия на переключатель реле4
BLYNK_WRITE(BLYNK_VPIN_RELAY4_SWITCH) {
    setRelayState(PIN_RELAY4, param.asInt());
}

// Обработчик на изменение положения слайдера заданной температуры для реле1
BLYNK_WRITE(BLYNK_VPIN_RELAY1_TARGET_TEMP_SLIDER) {
    relay1ControlState.targetTemp = param.asFloat();
}

// Обработчик на изменение положения слайдера заданной температуры для реле2
BLYNK_WRITE(BLYNK_VPIN_RELAY2_TARGET_TEMP_SLIDER) {
    relay2ControlState.targetTemp = param.asFloat();
}

// Обработчик на изменение положения слайдера гистерезиса для реле1
BLYNK_WRITE(BLYNK_VPIN_RELAY1_HISTERESIS_SLIDER) {
    relay1ControlState.histeresis = param.asFloat();
}

// Обработчик на изменение положения слайдера гистерезиса для реле2
BLYNK_WRITE(BLYNK_VPIN_RELAY2_HISTERESIS_SLIDER) {
    relay2ControlState.histeresis = param.asFloat();
}

// Обновление интерфейса Android приложения при подключении к серверу
BLYNK_CONNECTED() {
    Blynk.syncAll();
}

void updateRelayState(RelayControlStateStruct* relayControlState) {
    if (relayControlState->isTempControlOn == false || relayControlState->currentTemp == DEVICE_DISCONNECTED_C) {
        // контроль по температуре выключен или ошибка по температуре - выключаем реле
        relayControlState->isRelayOn = false;
    } else if (relayControlState->targetTemp > relayControlState->currentTemp + relayControlState->histeresis / 2) {
        // целевая температура выше текущей - включаем реле
        relayControlState->isRelayOn = true;
    } else if (relayControlState->targetTemp < relayControlState->currentTemp - relayControlState->histeresis / 2) {
        // целевая температура ниже текущей - выключаем реле
        relayControlState->isRelayOn = false;
    }
}

// Обновление состояния через заданный интервал
void myTimerEvent() {
    // DS18B20
    sensors.requestTemperatures();
    // #1
    const float temp1 = sensors.getTempC(tempSensor1Address);
    relay1ControlState.currentTemp = temp1;
    Blynk.virtualWrite(BLYNK_VPIN_DS18B20_1, temp1);
    // #2
    const float temp2 = sensors.getTempC(tempSensor2Address);
    relay2ControlState.currentTemp = temp2;
    Blynk.virtualWrite(BLYNK_VPIN_DS18B20_2, temp2);

    // DHT11
    sensors_event_t event;
    // temperature
    dht.temperature().getEvent(&event);
    Blynk.virtualWrite(BLYNK_VPIN_DHT11_TEMP, event.temperature);
    // humidity
    dht.humidity().getEvent(&event);
    Blynk.virtualWrite(BLYNK_VPIN_DHT11_HUMIDITY, event.relative_humidity);

    // Обновить состояние контролируемых по температуре реле
    // #1
    updateRelayState(&relay1ControlState);
    setRelayState(PIN_RELAY1, relay1ControlState.isRelayOn);
    relay1Led.setValue(relay1ControlState.isRelayOn * 255);
    // #2
    updateRelayState(&relay2ControlState);
    setRelayState(PIN_RELAY2, relay2ControlState.isRelayOn);
    relay2Led.setValue(relay2ControlState.isRelayOn * 255);
}

void setup() {
    // Debug console
    Serial.begin(9600);

    // DHT11
    dht.begin();

    // DS18B20
    sensors.begin();
    // 9 - 0.5°C
    // 10 - 0.25°C
    // 11 - 0.125°C
    // 12 - 0.0625°C
    sensors.setResolution(9);

    // инициализация пинов nodeMCU
    pinMode(PIN_RELAY1, OUTPUT);
    pinMode(PIN_RELAY2, OUTPUT);
    pinMode(PIN_RELAY3, OUTPUT);
    pinMode(PIN_RELAY4, OUTPUT);
    setRelayState(PIN_RELAY1, 0);
    setRelayState(PIN_RELAY2, 0);
    setRelayState(PIN_RELAY3, 0);
    setRelayState(PIN_RELAY4, 0);

    // Соединение с сервером Blynk
    Blynk.begin(auth, ssid, pass, SERVER_IP_ADDRESS, SERVER_PORT);

    // Таймер для обновления состояния
    timer.setInterval(1000L, myTimerEvent);
}

void loop() {
    Blynk.run();
    timer.run();
}
