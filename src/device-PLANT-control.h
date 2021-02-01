#ifndef DEVICE_H801_H__
#define DEVICE_H801_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#define MAX_DATAPOINTS 4 // maximum of datapoints per node (ToDo: make this "dynamic")
#include <Arduino.h>
#include "datatypes.h"

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

const HomieDeviceDef thisDevice PROGMEM = {"GREENHOUSE-02","Greenhouse 2"};

#define BME280_FORCED_MODE false
const HomieNodeDef BME280sensorNode = {"BME280","Bosch BME280", "enviornment", 3, 0, { // io 0 = autodetect I2C Address
    {"temperature","Temperature", "°C", DATATYPE_FLOAT, RETAINED, "-40:85",NON_SETTABLE,0.1,30,6000,0,0},   // sample every 30seconds. Send if value change by 0.1 or 10 minutes pass. No oversampling
    {"humidity","Humidity","%", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,30,6000,5,0},              // sample every 30seconds. Send if value change by 1 or 10 minutes pass. 5x oversampling
    {"pressure","Pressure","hPa", DATATYPE_FLOAT, RETAINED, "300:1100",NON_SETTABLE,10,30,6000,0,0}
}};

const HomieNodeDef PWMactuatorNode = {"FAN","2ch fan control", "PWM control",2, 0, {
    {"fan1","lower fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,0,6000,0,15}, // 0.1 fade step, delay per step, unused, GPIO
    {"fan2","upper fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,0,6000,0,13} // 0.1 fade step, delay per step, unused, GPIO
}};

const HomieNodeDef BH1750sensorNode = {"BH1750","BH1750 Light Sensor", "enviornment", 1, 0,{
    {"illumination","Light Amount", "lx", DATATYPE_FLOAT, RETAINED, "0:65528", NON_SETTABLE,20,30,6000,5,0},
}};

const HomieNodeDef ADS1115sensorNode = {"ADS1115","TI ADS1115 a/d converter", "ad converter", 4, 0, {
    {"SOIL-C","Capacitive Soil", "mV", DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,50,100,3600,5,0},
    {"POT-1","Potentiometer", "mV",  DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,5,1,6000,0,0},
    {"SOIL-R","Resistive Soil", "mV",  DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,50,600,3600,0,0},  // sample minute. Send if value change by 2 or a hour had passed. No oversampling. Switch on sensor before reding
    {"VCC","VCC", "mV",  DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,2,10,600,0,0}
  }
};
const HomieNodeDef virtualDevice = {"CONTROL","main Controls", "virtual device",1, 0, {
    {"fans","Fan switch", "", DATATYPE_BOOLEAN, RETAINED, "0:1",SETTABLE,0.1,0,6000,0,0} // 0.1 fade step, delay per step, unused, GPIO
}};


// #include "plugins.h"

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
#define PIN_LED 5        // default on board signal LED
#define PIN_BUTTON 1     // no pushbutton

// callbacks
bool fan1InputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    float _value = value.toFloat();
    myLog.printf(LOG_INFO,F(" Fan1 received %s = %.2f"), value.c_str(), _value);
    homieNode->setValue("fan1",_value);
    return true;
};

bool fan2InputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    float _value = value.toFloat();
    myLog.printf(LOG_INFO,F(" Fan2 received %s = %.2f"), value.c_str(), _value);
    homieNode->setValue("fan2",_value);
    return true;
};

bool fansInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool _value = (value.equalsIgnoreCase("true")) ? true : false;
    float _factor = (value.equalsIgnoreCase("true")) ? 1 : 0;
    myLog.printf(LOG_INFO,F(" Fans received %s = %d"), value.c_str(), _value);

    myDevice.setFactor("FAN","fan1",_factor);
    myDevice.setFactor("FAN","fan2",_factor);
    homieNode->setValue("fans",_value); // confirm value
    return true;
};

bool ADS1115readHandler(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool _result = true;
    myLog.printf(LOG_INFO,F(" Node %s Property %s Task %d"),homieNode->getDef()->id, homieProperty->getDef()->id);
    switch (task) {
      case TASK_BEFORE_SAMPLE: {
        // switch on resistive soil sensor for measurement
        digitalWrite(D8,HIGH);
        break;
      };
      case TASK_AFTER_READ: {
        // switch off resistive soil sensor to avoid corrosion
        digitalWrite(D8,LOW);
        break;
      };
    };
    
    return _result;
};
bool deviceSetup(void) {
    // switch off resistive soil sensor to avoid corrosion
    pinMode(D8,OUTPUT);
    digitalWrite(D8,LOW);
    myDevice.init(&thisDevice);

    //BME280 temperature, humidity and pressure sensor
    myDevice.createHomieNode(BME280_ID, &BME280sensorNode);

    // PWM Fan speed control
    myDevice.createHomieNode(PWM_ID, &PWMactuatorNode);
    myDevice.getNode("FAN")->registerInputHandler("fan1",fan1InputHandler);
    // myDevice.getNode("FAN")->registerInputHandler("fan2",fan2InputHandler); // commented out to test default handler

    //BH1750 ambient light sensor
    myDevice.createHomieNode(BH1750_ID, &BH1750sensorNode);

    //ADS1115 ADC for soil moisture monitoring
    myDevice.createHomieNode(ADS1115_ID, &ADS1115sensorNode);
    myDevice.getNode("ADS1115")->registerReadHandler("SOIL-R",&ADS1115readHandler);

    //special controls
    myDevice.createHomieNode(0, &virtualDevice)->registerInputHandler("fans",fansInputHandler);
    return true;
};

#endif