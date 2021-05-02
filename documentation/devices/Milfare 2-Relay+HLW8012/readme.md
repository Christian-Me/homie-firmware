# Milfra 2 relay, 2 switch and power meter module

## brief description

Searching for additional sonoff minis i stumbled upon this small modules to be placed behind wall switches (if you have the space) for less the 10€. I knew that the 

* This is a basic implementation of all the functionalities

**Be aware!** you are playing with mains power. Never connect to mains and your computer at the same time! Check live a neutral before connecting.

## hardware

* two pcbs with a 2mm board to board connector
* ESP8266ex on the digital board
* 1MB spi flash chip :(
* 1 green led wired to GPIO0
* 2mm programming header with TX/RX, GPIO0, reset, 3.3V and GND
* 2 relays (one is connected to TX! WTF)
* 2 mains input sensing GPIOs to connect switches
* HLW8012 power meter sensor on the mains board
* a buzzer (drawing a lot of power so my FTDI232 board can't deliver that via the build in 3.3V regulator! Perhaps you have to provide extra 5V via the board to board interconnection)

## flashing

* you can pop out the digital pcb which is glued in with double sided tape and solder tx,rx and ground wires on the pads from the back
* if the 3.3v power supply from your USB to serial adapter you can feed in 5V via the interconnection
* flash your `config.json` and your firmware via serial
* OTA via MQTT is supported

## plugins used

* **a_GPIO** to switch the relays
* **i_GPIO** for the switch inputs
* **a_PWM** to play tones on the buzzer
* **s_HLW8012** to measure power consumption

## code

### custom functions

*none*

### callback functions 

*none*

### setup

```C++
```

### loop

*none*