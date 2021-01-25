#ifndef DEVICE_TESTBENCH_H__
#define DEVICE_TESTBENCH_H__

#include <Arduino.h>
// #define HOMIE_DISABLE_LOGGING // homie internal log output disabled
#include "homieSyslog.h"
#define MAX_DATAPOINTS 6 // define the maximum datapoints per nodehere (of all your nodes)
#define MAX_ACTUATORS 5  // define the maximum actuators per node here (of all your nodes)
#include "../include/datatypes.h"
// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
const int PIN_LED = LED_BUILTIN;     // default on board signal LED
const int PIN_BUTTON = 0;            // default pushbutton

// modules to include

#define S_BH1750

#include "c_homie.h"

// basic configuration

#define FIRMWARE_NAME "TESTBENCH"
#define FIRMWARE_VERSION "0.0.3"
#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG


#define BME280_FORCED_MODE false
const HomieNodeDef BME280sensorNode PROGMEM = {"BME280","Bosch BME280", "enviornment", 3, {
    {"temperature","Temperature", "°C", "float", true, "-40:85",0.1,30,6000,0}, // sample every 30seconds. Send if value change by 0.1 or 10 minutes pass. No oversampling
    {"humidity","Humidity","%", "float", true, "0:100",1,30,6000,0},
    {"pressure","Pressure","hPa", "float", true, "300:1100",1,30,6000,0},
}};

const HomieNodeDef BH1750sensorNode PROGMEM = {"BH1750","BH1750 Light Sensor", "enviornment", 1, {
    {"illumination","Light Amount", "lx", "float", true, "0:65528",5,30,600,5},
}};

#include "sensors.h"

// calbacks

void soilBeforeRead(char* id, PropertyData sensorData) {
    digitalWrite(D8,HIGH);
    delay(500);
}

void soilAfterRead(char* id, PropertyData sensorData) {
    digitalWrite(D8,LOW);
}

void soilProcessValue(char* id, PropertyData sensorData) {
    
}

void soilAProcessValue(char* id, PropertyData sensorData) {
    
}

HomieNodeDef ADS1115sensorNode PROGMEM = {"ADS1115","TI ADS1115 a/d converter", "ad converter", 4, {
    {"SOIL-C","Capacitive Soil", "mV", "float", true, "-0.3:3.6",10,10,3600,5},
    {"POT-1","Potentiometer", "mV", "float", true, "-0.3:3.6",5,1,6000,0},
    {"SOIL-R","Resistive Soil", "mV", "float", true, "-0.3:3.6",10,600,3600,0,soilBeforeRead,soilAfterRead,soilProcessValue},  // sample minute. Send if value change by 2 or a hour had passed. No oversampling. Switch on sensor before reding
    {"VCC","VCC", "mV", "float", true, "-0.3:3.6",2,10,600,0},
    {"SOIL-A","calculated humidity", "%", "float", true, "0:100",5,600,3600,0,nullptr,nullptr,soilAProcessValue} // calchulated value
  }
};


//callbacks

bool deviceSetup(void) {
    static const HomieDeviceDef thisDevice PROGMEM = {"TESTBENCH","Sensor Testbench"};
    myDevice.init(&thisDevice);
    createHomieNode(BH1750_ID, &BH1750sensorNode);
    /*
    static const HomieNodeDef BH1750sensorNode PROGMEM = {"BH1750","BH1750 Light Sensor", "enviornment", 1, {}};
    
    
    static const HomiePropertyDef dataPoint1 PROGMEM = {"illumination","Light Amount", "lx", "float", true, "0:65528",5,30,600,5};
    node->addProperty(&dataPoint1);
    static const HomiePropertyDef dataPoint2 PROGMEM = {"illumination2","Light Amount", "lx", "float", true, "0:65528",5,30,600,5};
    node->addProperty(&dataPoint2);
    */

    // switch off resistive soil sensor to avoid corrosion
    pinMode(D8,OUTPUT);
    digitalWrite(D8,LOW);
    return true;
}

#endif