#ifndef ADS1115_H__
#define ADS1115_H__
#include <Arduino.h>
#define HOMIE_DISABLE_LOGGING // homie internal log output disabled
#include "homieSyslog.h"
#define MAX_DATAPOINTS 7 // define the maximum datapoints per nodehere (of all your nodes)
#define MAX_ACTUATORS 5  // define the maximum actuators per node here (of all your nodes)
#include "../include/datatypes.h"

// basic configuration

#define FIRMWARE_NAME "TESTBENCH"
#define FIRMWARE_VERSION "0.0.3"
#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG

// modules to include

#define S_BME280

const SensorNode BME280sensorNode PROGMEM = {"BME280","Bosch BME280", "enviornment", 3, {
    {"temperature","Temperature", "°C", "float", true, "-40:85",0.1,30,6000,0}, // sample every 30seconds. Send if value change by 0.1 or 10 minutes pass. No oversampling
    {"humidity","Humidity","%", "float", true, "0:100",1,30,6000,0},
    {"pressure","Pressure","hPa", "float", true, "300:1100",1,30,6000,0},
}};

#define S_BH1750
#define BME280_FORCED_MODE false
const SensorNode BH1750sensorNode PROGMEM = {"BH1750","BH1750 Light Sensor", "enviornment", 1, {
    {"illumination","Light Amount", "lx", "float", true, "0:65528",5,30,600,5},
}};

// ADS1115 4ch 16bit ADC
#define S_ADS1115
#define MAX_ADS1115_DATAPOINTS 4

// calbacks

void soilBeforeRead(char* id, SenstorData sensorData) {
    digitalWrite(D8,HIGH);
    delay(500);
}

void soilAfterRead(char* id, SenstorData sensorData) {
    digitalWrite(D8,LOW);
}

void soilProcessValue(char* id, SenstorData sensorData) {
    
}

SensorNode ADS1115sensorNode PROGMEM = {"ADS1115","TI ADS1115 a/d converter", "ad converter", 4, {
    {"SOIL-C","Capacitive Soil", "mV", "float", true, "-0.3:3.6",10,10,3600,5},
    {"POT-1","Potentiometer", "mV", "float", true, "-0.3:3.6",5,1,6000,0},
    {"SOIL-R","Resistive Soil", "mV", "float", true, "-0.3:3.6",10,600,3600,0,soilBeforeRead,soilAfterRead,soilProcessValue},  // sample minute. Send if value change by 2 or a hour had passed. No oversampling. Switch on sensor before reding
    {"VCC","VCC", "mV", "float", true, "-0.3:3.6",2,10,600,0},
}};

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

//callbacks

bool deviceSetup(void) {
    pinMode(D8,OUTPUT);
    digitalWrite(D8,LOW);
    return true;
}

#endif