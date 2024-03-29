#ifdef HOMIE_FIRMWARE_PLANT_CONTROL

#ifndef DEVICE_PLANT_CONTROL_H__
#define DEVICE_PLANT_CONTROL_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#include <Arduino.h>
#include "datatypes.h"
#include <plugins.h>
#include <s_BME280.h>         // needed to get options enums 
#include <calc_functions.h>   // usefull measurement conversions and calculations
#include <utils.h>

#include <s_BME280.h>
#include <s_BH1750.h>
#include <s_HDC1080.h>
#include <s_ADS1115.h>

// basic configuration

/********************************************************************
controller for my indoor plant greenhouse

- two channel PWM fan speed control
- BME280 environmental sensor
- BH1750 light sensor
- ADS1115 4x dac for soil moisture sensor(s)

*********************************************************************/

#define FIRMWARE_NAME "PLANT-CONTROL"
#define FIRMWARE_VERSION "0.0.2"

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

bool tempCalibration(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    float _value = value.toFloat();
    myLog.printf(LOG_INFO,F(" tempCalibration received %s = %.2f"), value.c_str(), _value);
    homieNode->plugin->setOption(s_BME280::TEMP_CALIBRATION,_value);
    return true;
};

// use the build in calibration factor to implement a switch without altering the set values
bool fansInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool _value = (value.equalsIgnoreCase("true")) ? true : false;
    float _factor = (value.equalsIgnoreCase("true")) ? 1 : 0;
    myLog.printf(LOG_INFO,F(" Fans received %s = %d"), value.c_str(), _value);
    myDevice.setFactor("FAN","fan1",_factor);
    myDevice.setFactor("FAN","fan2",_factor);
    if (homieNode!=NULL) {
      homieNode->setValue("fans",_value); // confirm value
    } else {
      myLog.print(LOG_ERR,F(" homieNode undefined!"));
    }
    return true;
};


bool ADS1115readHandler(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    static unsigned long timer = 0;
    static uint8_t spikeCounter = 0;
    static bool firstReading = true;
    static bool soilR = false; // flag if resistive soil sensor is on
    bool proceed = true;
    switch (task) {
      case TASK_BEFORE_SAMPLE: {
        // switch on resistive soil sensor for measurement
        if (!soilR) {
          timer = millis();
          myLog.printf(LOG_TRACE,F("  before sample %s/%s (%d)"),homieNode->getId(),homieProperty->getId(),timer);
          digitalWrite(D8,HIGH);
          homieNode->setCustomSampleRate(5000); // read sample in 5 sec
          proceed = false;
          soilR = true;
        } 
        break;
      };
      case TASK_AFTER_READ: {
        // switch off resistive soil sensor to avoid corrosion
        myLog.printf(LOG_TRACE,F("  after read %s/%s on for %dms (%d)"),homieNode->getId(),homieProperty->getId(),millis()-timer,timer);
        //drop first reading
        if (firstReading) {
          firstReading = false;
          proceed = false;
        }
        // check for spikes > 500mV but not on boot where value==0;
        if (homieProperty->getValue()!=0 && fabs(homieProperty->getReadValue()-homieProperty->getValue()) > 500) {
          myLog.printf(LOG_INFO,F("   %s/%s spike detected %.2fmv"),homieNode->getId(),homieProperty->getId(),homieProperty->getReadValue()-homieProperty->getValue());
          proceed = false;
          spikeCounter++;
          if (spikeCounter>2) {
            proceed=true;
            spikeCounter=0;
          }
        }
        homieNode->setCustomSampleRate(0);
        digitalWrite(D8,LOW);
        soilR = false;
        break;
      };
    };
   
    return proceed;
};

bool readAbsoluteHumidity(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool continueTasks = true;
    switch (task) {
      case TASK_AFTER_READ: {
        homieProperty->setValue((float) absoluteHumidity(homieNode->getProperty("temperature")->getValue(),homieNode->getProperty("humidity")->getValue()));
        continueTasks=false;
        break;
      }
    }
    return continueTasks;
};

bool readDrewPoint(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool continueTasks = true;
    switch (task) {
      case TASK_AFTER_READ: {
        homieProperty->setValue((float) drewPoint(homieNode->getProperty("temperature")->getValue(),homieNode->getProperty("humidity")->getValue()));
        continueTasks=false;
        break;
      }
    }
    return continueTasks;
};

bool deviceSetup(void) {
    // switch off resistive soil sensor to avoid corrosion
    pinMode(D8,OUTPUT);
    digitalWrite(D8,LOW);
    myDevice.init({"GREENHOUSE-01","Greenhouse 1"});

    //BME280 temperature, humidity and pressure sensor
    myDevice.addNode({"BME280","Bosch BME280", "enviornment"}, BME280_ID, 0)
    // first add the properies
      ->addProperty({"temperature","Temperature", "°C", DATATYPE_FLOAT, RETAINED, "-40:85",NON_SETTABLE,0.1,30,6000,0}, s_BME280::CHANNEL_TEMPERATURE)   // sample every 30seconds. Send if value change by 0.1 or 10 minutes pass. No oversampling
      ->addProperty({"humidity","Humidity","%", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,30,6000,5}, s_BME280::CHANNEL_HUMIDITY)              // sample every 30seconds. Send if value change by 1 or 10 minutes pass. 5x oversampling
      ->addProperty({"pressure","Pressure","hPa", DATATYPE_FLOAT, RETAINED, "300:1100",NON_SETTABLE,10,30,6000,0}, s_BME280::CHANNEL_PRESSURE)
      ->addProperty({"tempCalibration","Temperature calibration","°C", DATATYPE_FLOAT, RETAINED, "-5:5", SETTABLE, 0.1, 600,6000,0})
      ->addProperty({"absoluteHumidity","Absolute Humidity","g/m³", DATATYPE_FLOAT, RETAINED, "0:1500",NON_SETTABLE,1,30,6000,0})  // readHandler will calculate and update this value
      ->addProperty({"drewPoint","Drew Point","°C", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,30,6000,0})                   // readHandler will calculate and update this value
    // then options and handlers
      ->setPluginOption(s_BME280::MODE, Adafruit_BME280::MODE_FORCED)   // enable forced mode
      ->registerInputHandler("tempCalibration", tempCalibration)        // set temperature calculation
      ->registerReadHandler("absoluteHumidity", readAbsoluteHumidity)   // do absolute humidity calculation
      ->registerReadHandler("drewPoint", readDrewPoint);                // do drew point calculation

    // PWM Fan speed control
    myDevice.addNode({"FAN","2ch fan control", "PWM control"})
      ->addProperty({"fan1","lower fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,600,6000,0},PWM_ID,15)    // 0.1 fade step, delay per step, unused, GPIO
      ->addProperty({"fan2","upper fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,600,6000,0},PWM_ID,13);  // 0.1 fade step, delay per step, unused, GPIO&PWMactuatorNode);
    
    myDevice.getNode("FAN")->registerInputHandler("fan1", fan1InputHandler); // alternative way to register handlers
    // myDevice.getNode("FAN")->registerInputHandler("fan2",fan2InputHandler); // not used because default handler does the basic task sending received values unaltered to plugin

    //BH1750 ambient light sensor
    myDevice.addNode({"BH1750","BH1750 Light Sensor", "enviornment"},BH1750_ID, 0)
      ->addProperty({"illumination","Light Amount", "lx", DATATYPE_FLOAT, RETAINED, "0:65528", NON_SETTABLE,20,30,6000,5},s_BH1750::CHANNEL_ILLUMINATION);
    
    //HT1080 temperature, humidity sensor
    myDevice.addNode({"HDC1080","TI HDC1080", "enviornment"},HDC1080_ID, 0)                                                         // io 0 = autodetect I2C Address
      ->addProperty({"temperature","Temperature", "°C", DATATYPE_FLOAT, RETAINED, "-40:85",NON_SETTABLE,0.1,30,6000,0},s_HDC1080::CHANNEL_TEMPERATURE)            // sample every 30seconds. Send if value change by 0.1 or 10 minutes pass. No oversampling
      ->addProperty({"humidity","Humidity","%", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,30,6000,5},s_HDC1080::CHANNEL_HUMIDITY)                       // sample every 30seconds. Send if value change by 1 or 10 minutes pass. 5x oversampling
      ->addProperty({"absoluteHumidity","Absolute Humidity","g/m³", DATATYPE_FLOAT, RETAINED, "0:1500",NON_SETTABLE,1,30,6000,0})  // readHandler will calculate and update this value
      ->addProperty({"drewPoint","Drew Point","°C", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,30,6000,0})                   // readHandler will calculate and update this value&HDC1080sensorNode)
    
      ->registerReadHandler("absoluteHumidity", readAbsoluteHumidity)   // do absolute humidity calculation (Yes, they can be reused as long as the property ids are the same)
      ->registerReadHandler("drewPoint", readDrewPoint);                // do drew point calculation

    //ADS1115 ADC for soil moisture monitoring
    myDevice.addNode({"ADS1115","TI ADS1115 a/d converter", "ad converter"}, ADS1115_ID, 0)
      ->addProperty({"SOIL-C","Capacitive Soil", "mV", DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,50,600,3600,5}, s_ADS1115::CHANNEL_ADC1)
      ->addProperty({"POT-1","Potentiometer", "mV",  DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,5,100,6000,0}, s_ADS1115::CHANNEL_ADC2)
      ->addProperty({"SOIL-R","Resistive Soil", "mV",  DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,50,3600,3600,0}, s_ADS1115::CHANNEL_ADC3)  // sample minute. Send if value change by 2 or a hour had passed. No oversampling. Switch on sensor before reding
      ->addProperty({"VCC","VCC", "mV",  DATATYPE_FLOAT, RETAINED, "-0.3:3.6",NON_SETTABLE,2,100,600,0}, s_ADS1115::CHANNEL_ADC4)
      ->registerReadHandler("SOIL-R", ADS1115readHandler)
      ->hideLogLevel("POT-1",0b11111101); // do not display debug (avoid spamming messages for fast sample rates)

    //device controls
    myDevice.addNode({"CONTROL","main Controls", "virtual device"})
      ->addProperty({"fans","Fan switch", "", DATATYPE_BOOLEAN, RETAINED, "0:1",SETTABLE,0.1,600,6000,0}) // 0.1 fade step, delay per step, unused, GPIO&virtualDevice)
      ->registerInputHandler("fans", fansInputHandler);

    return true;
};

void deviceLoop() {
};

#endif
#endif