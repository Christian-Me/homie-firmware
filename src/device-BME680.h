#ifndef DEVICE_BME680_H__
#define DEVICE_BME680_H__
#include <Arduino.h>
#include "homieSyslog.h"
#define MAX_DATAPOINTS 7 // define the maximum datapoints per nodehere (of all your nodes)
#define MAX_ACTUATORS 5  // define the maximum actuators per node here (of all your nodes)
#include "../include/datatypes.h"

// basic configuration

#define FIRMWARE_NAME "BME680"
#define FIRMWARE_VERSION "0.0.1"

#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG

// modules to include

#define S_BME680
#define MAX_BME680_DATAPOINTS 7
const HomieNodeDef BME680sensorNode = {"BME680","Bosch BME680", "enviornment sensor", MAX_BME680_DATAPOINTS, {
    {"temperature","Temperature", "°C", "float", true, "-40:85",0.5,30000,600,0},
    {"humidity","Humidity","%", "float", true, "0:100",1,30000,600,0},
    {"pressure","Pressure","hPa", "float", true, "300:1100",5,30000,600,0},
    {"IAQ","Indoor Air Quality","", "float", true, "0:500", 5,30000,600,0},
    {"accuracy","IAQ Accuracy","", "enum", true, "started,uncertain,calibrating,calibrated",0,30000,0,0},
    {"sIAQ","Static Indoor Air Quality","", "float", true, "0:500", 5,30000,600,0},
    {"saccuracy","SIAQ Accuracy","", "enum", true, "started,uncertain,calibrating,calibrated",0,30000,0,0}
}};

#define M_NEOPIXEL
#define NEOPIXEL_PIN D4

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
const int PIN_LED = LED_BUILTIN;     // default on board signal LED
const int PIN_BUTTON = 0;            // default pushbutton

#endif