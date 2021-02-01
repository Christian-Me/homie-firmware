# homie-firmware

Early alpha of my own firmware for my automation projects on ESP8266 and ESP32 platform
it is based on the great homie-esp8266 library.

It is still a work in progress but I'm happy like it is working now

<a href="https://homieiot.github.io/">
  <img src="https://homieiot.github.io/img/works-with-homie.png" alt="works with MQTT Homie">
</a>

## intro

As handy as all the great frameworks like ESPEasy, Tasmota ... sooner or later I had issues with some details. As I use the homie convention for my communication via MQTT the choices are limited too. After several single task projects I decided to start with a new approach collecting all the bits and pieces into my own modular firmware or perhaps I should call it framework?

Perhaps I'm reinventing the wheel here. But why not?

**Any ideas, comments, issues, help or contributions are welcome.**

## features

- small footprint 400k with 3 plugins (OTA should be possible even on 1MB flash). Will be reduced by removing a lot of debugging log messages.
- around 40k free RAM using 3 Nodes with 6 parameters (plenty of headroom for own code). Thy to use `PROGMEM` wherever possible and useful.
- plugins linked on demand (only used plugins go into binary)
- build in sample timer per property
- threshold value to send data if a property changed
- build in timer to send data regardless of a change
- oversampling (rolling average to remove noise)
- write your own code using c and not a limited scripting interpreter
- OTA via MQTT upload thanks to the homie-ESP8266 library (you can use DHCP for your devices)
- crash detection (to be improved) to enable OTA even when your code causes an exception resulting in a boot loop
- logging to console and syslog. Console logging can switch log level and extra info by key press. 

## get started.

This project is intended to be compiled using [platformIO](https://platformio.org/). I highly recommend using vs-code as your IDE not only because this is the official IDE for platformIO but it is a great tool for beginners and advanced users

### create your on project

- create your project in `platformio.ini` with a unique `build_flag`
- add your project into `projects.h` and include your `device-yours.h` file
- edit your `device-yours.h` file. Here is where all your device specific things should happen. Take a look into `device-testbench.h` for reference (Update: use `device-PLANT-control.h` because the API had changed. I will update the test bench soon.)
- edit your own `config.json` with your device configuration (see example)
- upload your `config.json` via usb or flash the initial-upload build to use the config web interface or upload your config using curl
- flash your firmware via usb/serial or OTA if you have flashed the initial-upload build
- use the serial console or syslog for debugging

## platforms

- [X] ESP8266 (minimum 1MB flash)
- [X] ESP8285 (used in many of the shelf devices, 1MB flash)
- [ ] ESP32

## modules

the following list shows the planned and implemented devices. Basically all devices I used before or planning to use in the near future. Think the list grows quickly. Hopefully the ticks too. 

**Update:** as the API for plugins is still work in progress.

### core

- [X] MQTT communication following the homie convention
- [X] OTA updates over MQTT with crash detection
- [X] unified console logging with dynamic log levels
- [X] Syslog event logging for remote logging
- [X] two timers per property (sample data and send data)
- [X] dynamic data transmission per value threshold. Configurable per property.
- [X] data averaging to limit sensor noise. Configurable per property.
- [ ] auto sleep / deep sleep based on the timer settings


### sensors

- [X] ADS1115 4ch 16 bit AD converter
- [X] BME280 temperature, humidity and pressure sensor
- [ ] BME680 air quality (IAQ), temperature, humidity and pressure sensor
- [X] BH1750 light sensor (LUX meter)

### actuators

- [ ] RELAY / MOSFET switch (GPIO)
- [ ] DC Motor actuator
- [X] LED dimmers, bulbs and strips (PWM)
- [ ] RGBW addressable LED
- [ ] TUYA serial mcus

### human interface devices HID

- [ ] RGBW addressable LED (Neopixels)
- [ ] ePaper Display
- [ ] TFT Matrix display (SPI)
- [ ] LCD Matrix display (SPI)
- [ ] LCD Character display (I2C)
- [ ] OLED Display (I2C)


- [ ] Push Button
- [ ] i2c rotary encoder (with RGB led)

## commercial devices

- [ ] Sonoff mini / S20 (1xRELAY, Push Button, indicator LED, ESP8285)
- [ ] RGBW Light Bulb
- [X] H801 5ch 12-24V constant voltage LED (strip) dimmer
- [ ] Addressable LED Light Strips (Neopixel)
- [ ] TUYA serial dimmers