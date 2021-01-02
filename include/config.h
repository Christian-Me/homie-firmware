#ifndef CONFIG_H__
#define CONFIG_H__

// all configuration should be concentrated here

// general
const int PIN_LED = 13;     // default on board signal LED
const int PIN_BUTTON = 0;   // default pushbutton

// matrix displays

// LED BACKLIGHT DIMMING
// DO NOT CONNECT LED Pin to a GPIO PIN directly. Make sure a transistor switches the high current of the LED!
#define BACKLIGHT_PIN 4           // GPIO04 on TTGO-T-DISPLAY by LILYGO (transistor present)
#define BACKLIGHT_BRIGHTNESS 128  // default brightness
#define SCREENSAVER_TIMEOUT 30    // Screensaver timeout in seconds

// human interface device

// buttons
#define BUTTON1_PIN  35
#define BUTTON2_PIN  0

// device specific

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

#endif