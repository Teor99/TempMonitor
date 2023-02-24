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

#define PIN_T1_RELAY D5
#define PIN_T2_RELAY D6

// маски виртуальных пинов BLYNK
#define BLYNK_VPIN_RELAY_1_SWITCH V1
#define BLYNK_VPIN_RELAY_2_SWITCH V2

#define BLYNK_VPIN_DS18B20_1 V3
#define BLYNK_VPIN_DS18B20_2 V4

#define BLYNK_VPIN_DHT11_TEMP V5
#define BLYNK_VPIN_DHT11_HUMIDITY V6

// уровень логического сигнала для реле (на случай, если реле управляется инвертированым лог. сигналом)
bool isInvertRelayLogicLevel = true;

char auth[] = "please_enter_your_blynk_token";
char ssid[] = "please_enter_your_wifi_ssid";
char pass[] = "please_enter_your_wifi_password";

BlynkTimer timer;

// DS18b20
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);
DeviceAddress tempSensor1 = {0x28, 0x76, 0x1F, 0x56, 0xB5, 0x1, 0x3C, 0xA6};
DeviceAddress tempSensor2 = {0x28, 0xFF, 0x4, 0xD4, 0x44, 0x16, 0x03, 0x53};

// DHT11
DHT_Unified dht(PIN_DHT11, DHT11);

// Установить состояние пина реле
void setRelayState(uint8_t pin, uint8_t logicLevel) {
    digitalWrite(pin, isInvertRelayLogicLevel ? !logicLevel : logicLevel);
}

// Прочитать состояние пина реле
int getRelayState(uint8_t pin) {
    return isInvertRelayLogicLevel ? !digitalRead(pin) : digitalRead(pin);
}

BLYNK_WRITE(BLYNK_VPIN_RELAY_1_SWITCH) {
    setRelayState(PIN_T1_RELAY, param.asInt());
}

BLYNK_WRITE(BLYNK_VPIN_RELAY_2_SWITCH) {
    setRelayState(PIN_T2_RELAY, param.asInt());
}

// Обновление интерфейса Android приложения при подключении к серверу
BLYNK_CONNECTED() {
    Blynk.syncAll();
}

// опрос датчиков
void myTimerEvent() {
    // запрсить температуру с датчиков DS18B20
    sensors.requestTemperatures();
    Blynk.virtualWrite(BLYNK_VPIN_DS18B20_1, sensors.getTempC(tempSensor1));
    Blynk.virtualWrite(BLYNK_VPIN_DS18B20_2, sensors.getTempC(tempSensor2));

    // DHT11
    // temperature
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    Blynk.virtualWrite(BLYNK_VPIN_DHT11_TEMP, event.temperature);
    Blynk.virtualWrite(BLYNK_VPIN_DHT11_HUMIDITY, event.relative_humidity);
}

void setup() {
    // Debug console
    Serial.begin(9600);

    // DHT11
    dht.begin();

    // инициализация пинов nodeMCU
    pinMode(PIN_T1_RELAY, OUTPUT);
    pinMode(PIN_T2_RELAY, OUTPUT);
    setRelayState(PIN_T1_RELAY, 0);
    setRelayState(PIN_T2_RELAY, 0);

    // Blynk.begin(auth, ssid, pass);
    // You can also specify server:
    // Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
    Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 0, 101), 8080);

    // Setup a function to be called every second
    timer.setInterval(1000L, myTimerEvent);
}

void loop() {
    Blynk.run();
    timer.run();  // Initiates BlynkTimer
}