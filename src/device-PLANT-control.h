#ifndef DEVICE_H801_H__
#define DEVICE_H801_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#define MAX_DATAPOINTS 3 // maximum of datapoints per node (ToDo: make this "dynamic")
#include <Arduino.h>
#include "../include/datatypes.h"

// basic configuration

/********************************************************************
controller for my indoor plant greenhouse

- two channel PWM fan speed control
- BME280 environmental sensor
- BH1750 light sensor
- ADS1115 dac for soil moisture sensor(s)

*********************************************************************/

#define FIRMWARE_NAME "PLANT-CONTROLL"
#define FIRMWARE_VERSION "0.0.1"

const HomieDeviceDef thisDevice PROGMEM = {"GREENHOUSE-01","Greenhouse 1"};

#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG

// modules to include

#define A_PWM
// #define S_BME280

#define BME280_FORCED_MODE false
const HomieNodeDef BME280sensorNode PROGMEM = {"BME280","Bosch BME280", "enviornment", 3, {
    {"temperature","Temperature", "°C", DATATYPE_FLOAT, RETAINED, "-40:85",NON_SETTABLE,0.1,30,6000,0,0}, // sample every 30seconds. Send if value change by 0.1 or 10 minutes pass. No oversampling
    {"humidity","Humidity","%", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,30,6000,0,0},
    {"pressure","Pressure","hPa", DATATYPE_FLOAT, RETAINED, "300:1100",NON_SETTABLE,1,30,6000,0,0},
}};

const HomieNodeDef PWMactuatorNode = {"FAN","2ch fan control", "PWM control",2, {
    {"fan1","lower fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,10,0,0,15}, // 0.1 fade step, delay per step, unused, GPIO
    {"fan2","upper fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,10,0,0,13} // 0.1 fade step, delay per step, unused, GPIO
}};

const HomieNodeDef virtualDevice = {"CONTROL","main Controls", "virtual device",1, {
    {"fans","Fan switch", "", DATATYPE_BOOLEAN, RETAINED, "0:1",SETTABLE,0.1,10,0,0,15}, // 0.1 fade step, delay per step, unused, GPIO
}};


// #include "plugins.h"

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
#define PIN_LED 5        // default on board signal LED
#define PIN_BUTTON 1     // no pushbutton

#include "c_homie.h"
#include "plugins.h"

MyHomieDevice myDevice;
bool normalOperation = false;

// callbacks
bool fan1InputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    float _value = value.toFloat();
    myLogf(LOG_INFO,F(" Fan1 received %s = %.2f"), value.c_str(), _value);
    homieNode->setValue("fan1",_value);
    return true;
};

bool fan2InputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    float _value = value.toFloat();
    myLogf(LOG_INFO,F(" Fan2 received %s = %.2f"), value.c_str(), _value);
    homieNode->setValue("fan2",_value);
    return true;
};

bool fansInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool _value = (value.equalsIgnoreCase("true")) ? true : false;
    float _factor = (value.equalsIgnoreCase("true")) ? 1 : 0;
    myLogf(LOG_INFO,F(" Fans received %s = %d"), value.c_str(), _value);

    myDevice.setFactor("FAN","fan1",_factor);
    myDevice.setFactor("FAN","fan2",_factor);
    homieNode->setValue("fans",_value); // confirm value
    return true;
};

bool deviceSetup(void) {
    myDevice.init(&thisDevice);
    myDevice.createHomieNode(BME280_ID, &BME280sensorNode);
    myDevice.createHomieNode(PWM_ID, &PWMactuatorNode);
    myDevice.getNode("FAN")->registerInputHandler("fan1",fan1InputHandler);
    // myDevice.getNode("FAN")->registerInputHandler("fan2",fan2InputHandler); // commented out to test default handler

    myDevice.createHomieNode(0, &virtualDevice)->registerInputHandler("fans",fansInputHandler);
    return true;
};

#endif