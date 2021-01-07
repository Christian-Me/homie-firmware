# homie-firmware

Early alpha of my own firmware for my automation projects.

## intro

As handy as all the great frameworks like ESPEasy, Tasmota ... sooner or later I had issues with some details. As I use the homie convention for my communication via MQTT the choices are limited too. After several single task projects I decided to start with a new approach collecting all the bits and pieces into my own modular firmware.

## platforms

- [X] ESP8266 (minimum 1MB flash)
- [X] ESP8285 (used in many of the shelf devices, 1MB flash)
- [ ] ESP32

## modules

the following list shows the planned and implemented devices. Basically all devices I used before or planning to use in the near future. Think the list grows quickly. Hopefully the ticks too.

### core

- [X] MQTT communication following the homie convention
- [X] OTA updates over MQTT
- [X] Syslog event logging

### sensors

- [X] ADS1115 4ch 16 bit AD converter
- [X] BME280 temperature, humidity and pressure sensor
- [X] BME680 air quality (IAQ), temperature, humidity and pressure sensor
- [X] BH1750 light sensor (LUX meter)

### actuators

- [ ] RELAY / MOSFET switch
- [ ] DC Motor actuator
- [X] LED dimmers, bulbs and strips
- [ ] RGBW addressable LED
- [ ] TUYA serial dimmers

### human interface devices HID

- [X] RGBW addressable LED (Neopixels)
- [ ] ePaper Display
- [ ] LCD Matrix display
- [ ] OLED Display

- [X] Push Button
- [ ] i2c rotary encoder (with RGB led)

## commercial devices

- [ ] Sonoff mini / S20 (1xRELAY, Push Button, indicator LED, ESP8285)
- [ ] RGBW Light Bulb
- [X] H801 5ch 12-24V constant voltage LED (strip) dimmer
- [ ] Addressable LED Light Strips (Neopixel)