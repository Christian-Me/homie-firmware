#ifndef ADS1115_H__
#define ADS1115_H__
#include <Arduino.h>

// basic configuration

#define FIRMWARE_NAME "ADS1115"
#define FIRMWARE_VERSION "0.0.3"

// modules to include

#define S_ADS115
    #define ADS1115_SAMPLE_RATE 500 // sample every 500ms
    #define ADS1115_TIMEOUT 0       // only send on value change
    #define ADS1115_OVERSAMPLING 5  // average 5 samples
    #define ADS1115_THRESHOLD 10    // 10mv noise threshhold

#define S_BME280
    #define BME280_SAMPLE_RATE 60000    // sample every minute
    #define BME280_TIMEOUT 0            // only send on value change
    #define BME280_OVERSAMPLING 0       // no
    #define BME280_THRESHOLD_TEMP 0.1   // 0.1 Degree  (0.01 supported) 
    #define BME280_THRESHOLD_HUMI 0.1   // 0.1 Percent (0.008 supported) 
    #define BME280_THRESHOLD_PRES 5     // 0.1 hPa     (0.2 1.7cm! supported) 

#define S_BH1750
    #define BH1750_SAMPLE_RATE 5000     // sample every 5sec
    #define BH1750_TIMEOUT 0            // only send on value change
    #define BH1750_OVERSAMPLING 5       // average 5 samples
    #define BH1750_THRESHOLD 1          // 1 LUX noise threshhold

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
const int PIN_LED = LED_BUILTIN;     // default on board signal LED
const int PIN_BUTTON = 0;            // default pushbutton

#endif