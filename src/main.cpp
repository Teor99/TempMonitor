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

/*
1. In the header BlynkSimpleEsp8266.h, replace
BlynkWifi Blynk(_blynkTransport);
with
extern BlynkWifi Blynk;

2. In the main .ino file, right below the #include <BlynkSimpleEsp8266.h>, add
BlynkWifi Blynk(_blynkTransport);

3. Shamelessly include the BlynkSimpleEsp8266 header in as many places as you want with no remorse.
*/
#include <BlynkSimpleEsp8266.h>
BlynkWifi Blynk(_blynkTransport);

// DS18b20
#include <DallasTemperature.h>
#include <OneWire.h>

// DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Relay logic classes
#include "SimpleRelay.h"
#include "SmartRelay.h"

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
#define BLYNK_VPIN_RELAY1_STATE_LED V5
#define BLYNK_VPIN_RELAY1_ERROR_LED V30

#define BLYNK_VPIN_DS18B20_2 V6
#define BLYNK_VPIN_RELAY2_TEMP_CONTROL_SWITCH V7
#define BLYNK_VPIN_RELAY2_TARGET_TEMP_SLIDER V8
#define BLYNK_VPIN_RELAY2_HISTERESIS_SLIDER V9
#define BLYNK_VPIN_RELAY2_STATE_LED V10
#define BLYNK_VPIN_RELAY2_ERROR_LED V31

#define BLYNK_VPIN_DHT11_TEMP V20
#define BLYNK_VPIN_DHT11_HUMIDITY V21

#define BLYNK_VPIN_RELAY3_SWITCH V25
#define BLYNK_VPIN_RELAY4_SWITCH V26

// connection and login data
#define BLYNK_TOKEN "please_enter_your_blynk_token"
#define WIFI_SSID "please_enter_your_wifi_ssid"
#define WIFI_PASSWORD "please_enter_your_wifi_password"
#define SERVER_IP_ADDRESS IPAddress(192, 168, 0, 101)
#define SERVER_PORT 8080

BlynkTimer timer;

// DS18b20
OneWire oneWire(PIN_DS18B20);
DallasTemperature dsBus(&oneWire);
DeviceAddress tempSensor1Address = {0x28, 0xF7, 0x46, 0x57, 0x04, 0xE1, 0x3C, 0xF0};
DeviceAddress tempSensor2Address = {0x28, 0x89, 0x89, 0x57, 0x04, 0x26, 0x3C, 0xEC};

// DHT11
DHT_Unified dht(PIN_DHT11, DHT11);

// LEDS
WidgetLED relay1StateLed(BLYNK_VPIN_RELAY1_STATE_LED);
WidgetLED relay1ErrorLed(BLYNK_VPIN_RELAY1_ERROR_LED);
WidgetLED relay2StateLed(BLYNK_VPIN_RELAY2_STATE_LED);
WidgetLED relay2ErrorLed(BLYNK_VPIN_RELAY2_ERROR_LED);

SmartRelay smartRelay1(PIN_RELAY1,
                       true,
                       &relay1StateLed,
                       &relay1ErrorLed,
                       &dsBus,
                       tempSensor1Address,
                       BLYNK_VPIN_DS18B20_1);
SmartRelay smartRelay2(PIN_RELAY2,
                       true,
                       &relay2StateLed,
                       &relay2ErrorLed,
                       &dsBus,
                       tempSensor2Address,
                       BLYNK_VPIN_DS18B20_2);
SimpleRelay relay3(PIN_RELAY3, true);
SimpleRelay relay4(PIN_RELAY4, true);

// Обработчик нажатия на переключатель контроля по температуре для реле1
BLYNK_WRITE(BLYNK_VPIN_RELAY1_TEMP_CONTROL_SWITCH) {
    param.asInt() ? smartRelay1.enableTempConrol() : smartRelay1.disableTempConrol();
}

// Обработчик нажатия на переключатель контроля по температуре для реле2
BLYNK_WRITE(BLYNK_VPIN_RELAY2_TEMP_CONTROL_SWITCH) {
    param.asInt() ? smartRelay2.enableTempConrol() : smartRelay2.disableTempConrol();
}

// Обработчик на изменение положения слайдера заданной температуры для реле1
BLYNK_WRITE(BLYNK_VPIN_RELAY1_TARGET_TEMP_SLIDER) {
    smartRelay1.setTargerTemp(param.asFloat());
}

// Обработчик на изменение положения слайдера заданной температуры для реле2
BLYNK_WRITE(BLYNK_VPIN_RELAY2_TARGET_TEMP_SLIDER) {
    smartRelay2.setTargerTemp(param.asFloat());
}

// Обработчик на изменение положения слайдера гистерезиса для реле1
BLYNK_WRITE(BLYNK_VPIN_RELAY1_HISTERESIS_SLIDER) {
    smartRelay1.setHisteresis(param.asFloat());
}

// Обработчик на изменение положения слайдера гистерезиса для реле2
BLYNK_WRITE(BLYNK_VPIN_RELAY2_HISTERESIS_SLIDER) {
    smartRelay2.setHisteresis(param.asFloat());
}

// Обработчик нажатия на переключатель реле3
BLYNK_WRITE(BLYNK_VPIN_RELAY3_SWITCH) {
    param.asInt() ? relay3.enableRelay() : relay3.disableRelay();
}

// Обработчик нажатия на переключатель реле4
BLYNK_WRITE(BLYNK_VPIN_RELAY4_SWITCH) {
    param.asInt() ? relay4.enableRelay() : relay4.disableRelay();
}

// Обновление интерфейса Android приложения при подключении к серверу
BLYNK_CONNECTED() {
    Blynk.syncAll();
}

// Обновление состояния через заданный интервал
void myTimerEvent() {
    // Обновить состояние контролируемых по температуре реле
    dsBus.requestTemperatures();
    // #1
    smartRelay1.updateState();
    smartRelay1.sendTempToClient();
    // #2
    smartRelay2.updateState();
    smartRelay2.sendTempToClient();

    // DHT11
    sensors_event_t event;
    // temperature
    dht.temperature().getEvent(&event);
    Blynk.virtualWrite(BLYNK_VPIN_DHT11_TEMP, event.temperature);
    Serial.print("DHT11 temp: ");
    Serial.print(event.temperature);
    // humidity
    dht.humidity().getEvent(&event);
    Blynk.virtualWrite(BLYNK_VPIN_DHT11_HUMIDITY, event.relative_humidity);
    Serial.print(" hum: ");
    Serial.println(event.relative_humidity);
}

void setup() {
    // Debug console
    Serial.begin(9600);

    // DHT11
    dht.begin();

    // DS18B20
    dsBus.begin();
    // 9 - 0.5°C
    // 10 - 0.25°C
    // 11 - 0.125°C
    // 12 - 0.0625°C
    dsBus.setResolution(9);

    // Соединение с сервером Blynk
    Blynk.begin(BLYNK_TOKEN, WIFI_SSID, WIFI_PASSWORD, SERVER_IP_ADDRESS, SERVER_PORT);

    // Таймер для обновления состояния
    timer.setInterval(1000L, myTimerEvent);
}

void loop() {
    Blynk.run();
    timer.run();
}
