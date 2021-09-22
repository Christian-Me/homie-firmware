#ifndef DEVICE_GREENHOUSE_H__
#define DEVICE_GREENHOUSE_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#include <Arduino.h>
#include "datatypes.h"
#include <plugins.h>
#include <a_gpio.h>
#include <s_BH1750.h>
#include <s_HDC1080.h>
#include <calc_functions.h>   // usefull measurement conversions and calculations
#include <utils.h>
#include <sequencer.h>


// basic configuration

/********************************************************************
controller for my indoor plant greenhouse

- two channel PWM fan speed control
- HDC1080 environmental sensor
- BH1750 light sensor
- humidifier control via power and single button (long and shot press two stages for fog and light)

*********************************************************************/

#define FIRMWARE_NAME "PLANT-CONTROL"
#define FIRMWARE_VERSION "0.0.1"

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
#define PIN_LED 5        // default on board signal LED
#define PIN_BUTTON 1     // no pushbutton

SequenceStep humidifierOn[] = {
                                {"CONTROL","humiPower",0,10},     // switch humidifier off (reset)
                                {"CONTROL","humiButton",1,2000},  // pull button line high 2sec cooldown
                                {"CONTROL","humiPower",1,2000},   // switch humidifier on and wait 2sec to boot
                                {"CONTROL","humiButton",0,100},   // press button 100ms
                                {"CONTROL","humiButton",1,0}      // release button
                              };
SequenceStep humidifierPulse[] = {
                                {"CONTROL","humiPower",0,10},
                                {"CONTROL","humiButton",1,2000},
                                {"CONTROL","humiPower",1,2000},
                                {"CONTROL","humiButton",0,100},   // first button press
                                {"CONTROL","humiButton",1,200},   // wait 200 ms
                                {"CONTROL","humiButton",0,100},   // second button press
                                {"CONTROL","humiButton",1,0}
                              };
SequenceStep singleLongPulse[] = {
                                {"CONTROL","humiPower",1,100},
                                {"CONTROL","humiButton",0,1000},
                                {"CONTROL","humiButton",1,0}
                              };
SequenceStep doubleLongPulse[] = {
                                {"CONTROL","humiPower",1,100},
                                {"CONTROL","humiButton",0,1000},
                                {"CONTROL","humiButton",1,50},
                                {"CONTROL","humiButton",0,1000},
                                {"CONTROL","humiButton",1,0}
                              };

Sequencer sequencer;

// callbacks

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

bool humidifierCallback(const char* node, const char* property, float value) {
  myLog.printf(LOG_INFO,F("  sequencer %s/%s = %.2f"), node, property, value);
  myDevice.getNode(node)->getPropertyPlugin(property)->set(value);
  return true; // continue sequence
};

uint8_t lightState = 0;
void doneCallback() {
  if (lightState==1) {
    sequencer.startSequence(singleLongPulse,&humidifierCallback);
  } else if (lightState==2) {
    sequencer.startSequence(doubleLongPulse,&humidifierCallback);
  }
};

bool humidifierInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
  bool success = false;
  int option = enumGetIndex(String(homieProperty->getDef().format),value);
  myLog.printf(LOG_INFO,F(" humidifierInputHandler received %s/%s = %d"), value.c_str(), homieProperty->getId(), option);
  bool switchLight = false;
  if (strcmp(homieProperty-> getId(),"humiLight")==0) {
    switchLight = true;
  }
  switch (option)
  {
    case 0: // off
      homieNode->getPropertyPlugin("humiPower")->set(LOW);
      success = true;
      break;
    case 1: // on
      if (!sequencer.sequenceRunning()) {
        if (!switchLight) {
          myLog.print(LOG_INFO,F("Starting humidifier on sequence"));
          sequencer.startSequence(humidifierOn,&humidifierCallback,&doneCallback);
        } else {
          if (lightState==0) {
            sequencer.startSequence(singleLongPulse,&humidifierCallback);
          } if (lightState==2) {
            sequencer.startSequence(doubleLongPulse,&humidifierCallback);
          }
          lightState = 1;
        }
        success = true;
      }
      break;
    case 2: // pulse
      if (!sequencer.sequenceRunning()) {
        if (!switchLight) {
          sequencer.startSequence(humidifierPulse,&humidifierCallback, &doneCallback);
        } else {
          if (lightState==0) {
            sequencer.startSequence(doubleLongPulse,&humidifierCallback);
          } if (lightState==1) {
            sequencer.startSequence(singleLongPulse,&humidifierCallback);
          }
          lightState = 2;
        }
        success = true;
      }
      break;
    default:
      break;
  }
  if (success) {
    homieProperty->sendValue((float) option);
  }
  return true;
};

bool readAbsoluteHumidity(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool continueTasks = true;
    switch (task) {
      case TASK_AFTER_READ: {
        homieProperty->setValue((float) absoluteHumidity(homieNode->getProperty("temperature")->getValue(),homieNode->getProperty("humidity")->getValue()));
        continueTasks=false; // don't continue task because all done!
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
    myDevice.init({"GREENHOUSE-02","Greenhouse 2"});

    // PWM Fan speed control
    myDevice.addNode({"FAN","2ch fan control", "PWM control"})
      ->addProperty({"fan1","lower fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,600,6000,0},PWM_ID,15)    // 0.1 fade step, delay per step, unused, GPIO
      ->addProperty({"fan2","upper fan", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,0.1,600,6000,0},PWM_ID,13);  // 0.1 fade step, delay per step, unused, GPIO&PWMactuatorNode);
    
    // myDevice.getNode("FAN")->registerInputHandler("fan1", fan1InputHandler); // alternative way to register handlers
    // myDevice.getNode("FAN")->registerInputHandler("fan2",fan2InputHandler); // not used because default handler does the basic task sending received values unaltered to plugin

    //BH1750 ambient light sensor
    myDevice.addNode({"BH1750","BH1750 Light Sensor", "enviornment"}, BH1750_ID , 0)
      ->addProperty({"illumination","Light Amount", "lx", DATATYPE_FLOAT, RETAINED, "0:65528", NON_SETTABLE,5,30,6000,0}, s_BH1750::CHANNEL_ILLUMINATION);
    
    //HT1080 temperature, humidity sensor
    myDevice.addNode({"HDC1080","TI HDC1080", "enviornment"}, HDC1080_ID, 0)                                                         // io 0 = autodetect I2C Address
      ->addProperty({"temperature","Temperature", "°C", DATATYPE_FLOAT, RETAINED, "-40:85",NON_SETTABLE,0.1,30,6000,0},s_HDC1080::CHANNEL_TEMPERATURE) // sample every 30seconds. Send if value change by 0.1 or 10 minutes pass. No oversampling
      ->addProperty({"humidity","Humidity","%", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,30,6000,0},s_HDC1080::CHANNEL_HUMIDITY)               // sample every 30seconds. Send if value change by 1 or 10 minutes pass. 5x oversampling
      ->addProperty({"absoluteHumidity","Absolute Humidity","g/m³", DATATYPE_FLOAT, RETAINED, "0:1500",NON_SETTABLE,10,30,6000,0})  // readHandler will calculate and update this value
      ->addProperty({"drewPoint","Drew Point","°C", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,0.5,30,6000,0})                  // readHandler will calculate and update this value&HDC1080sensorNode)
    
      ->registerReadHandler("absoluteHumidity", readAbsoluteHumidity)   // do absolute humidity calculation (Yes, they can be reused as long as the property ids are the same)
      ->registerReadHandler("drewPoint", readDrewPoint);                // do drew point calculation

    //device controls
    myDevice.addNode({"CONTROL","main Controls", "virtual device"})
      ->addProperty({"fans","Fan switch", "", DATATYPE_BOOLEAN, RETAINED, "0:1",SETTABLE,0.1,600,6000,0}) // 0.1 fade step, delay per step, unused, GPIO&virtualDevice)
      ->registerInputHandler("fans", fansInputHandler)
      ->addProperty({"humiPower","Humidifier Power", "", DATATYPE_INTEGER, RETAINED, "",SETTABLE,1,600,6000,0},aGPIO_ID,12)   // D6
      ->addProperty({"humiButton","Humidifier Button", "", DATATYPE_INTEGER, RETAINED, "",SETTABLE,1,600,6000,0},aGPIO_ID,14) // D5
      ->addProperty({"humiState","Humidifier", "", DATATYPE_ENUM, RETAINED, "off,on,pulse",SETTABLE,1,600,6000,0})
      ->registerInputHandler("humiState", humidifierInputHandler)
      ->addProperty({"humiLight","Humidifier Light", "", DATATYPE_ENUM, RETAINED, "off,on,fade",SETTABLE,1,600,6000,0})
      ->registerInputHandler("humiLight", humidifierInputHandler);


    return true;
};

void deviceLoop() {
  sequencer.loop();
};

#endif