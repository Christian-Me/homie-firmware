# homie-firmware

Early alpha of my own firmware for my automation projects.

## intro

As handy as all the great frameworks like ESPEasy, Tasmota ... sooner or later I had issues with some details. As I use the homie convention for my communication via MQTT the choices are limited too. After several singe task projects I decided to start with a new approach collecting all the bits and pieces into my own modular firmware.

## modules

### core

- [X] MQTT communication following the homie convention
- [X] OTA updates over MQTT
- [X] Syslog event logging

### sensors

- [X] ADS1115 4ch 16 bit AD converter
- [X] BME280 temperature, humidity and pressure sensor
- [ ] BH1750 light sensor (LUX meter)

### actuators

- [ ] RELAY / MOSFET switch
- [ ] DC Motor actuator
- [ ] LED dimmers, bulbs and strips
- [ ] RGBW addressable LED
- [ ] TUYA serial dimmers