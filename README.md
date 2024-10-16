# Мониторинг температуры на основе Blynk и nodeMCU

<img src="./docs/prewiev.jpg" width="854" height="480">

## Ссылки

[https://www.blynk.cc](http://www.blynk.cc)

[https://docs.blynk.cc/](https://docs.blynk.cc/)

[https://examples.blynk.cc/](https://examples.blynk.cc/)

## Версии использованные при сборке проекта

Файлы лежат в папке [docs](./docs) взяты с сайта [4pda.to](https://4pda.to/forum/index.php?showtopic=818763)

-   Blynk мобильный клиент v2.27.24
-   Blynk сервер v0.41.17
-   Blynk ардуино библиотека v0.6.1

Перед сборкой:
1. скопировать папку Blynk из `docs\to_libdeps.7z` в папку `.pio\libdeps\d1_mini`
2. в конце файла `.pio\libdeps\d1_mini\Blynk\src\BlynkSimpleEsp8266.h` заменить строку `BlynkWifi Blynk(_blynkTransport);` на `extern BlynkWifi Blynk;` чтобы избавиться от ошибки `src\main.cpp:47:11: error: redefinition of 'BlynkWifi Blynk'` [link](https://arduino.stackexchange.com/questions/58358/how-to-avoid-multiple-definition-of-blynk-error)

## Таблица виртуальных пинов

| Название                                                     | Виртуальный пин |
| ------------------------------------------------------------ | :-------------: |
| температура с датчика DS18B20 #1                             |       V1        |
| переключатель (вкл/выкл) контроля по температуре для реле #1 |       V2        |
| целевая температура для реле #1                              |       V3        |
| гистерезис для реле #1                                       |       V4        |
| светодиод состояния пина для реле #1                         |       V5        |
| светодиод состояния ошибки для реле #1                       |       V30       |
| -                                                            |        -        |
| температура с датчика DS18B20 #2                             |       V6        |
| переключатель (вкл/выкл) контроля по температуре для реле #2 |       V7        |
| целевая температура для реле #2                              |       V8        |
| гистерезис для реле #2                                       |       V9        |
| светодиод состояния пина для реле #2                         |       V10       |
| светодиод состояния ошибки для реле #2                       |       V31       |
| -                                                            |        -        |
| температура с датчика DHT11                                  |       V20       |
| влажность с датчика DHT11                                    |       V21       |
| -                                                            |        -        |
| переключатель (вкл/выкл) реле #3                             |       V25       |
| переключатель (вкл/выкл) реле #4                             |       V26       |

## Распиновка платы nodeMCU

![alt text](./docs/nodemcu_v3_pinout.png)

## Схема подключения датчиков

![alt text](./docs/sensors.png)

## Схема подключения реле

![alt text](./docs/relays.png)
