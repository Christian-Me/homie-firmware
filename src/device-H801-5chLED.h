#ifndef DEVICE_H801_H__
#define DEVICE_H801_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG
// for now define max Datapoints in platformio.ini
// #define MAX_DATAPOINTS 5 // maximum of datapoints per node (ToDo: make this "dynamic")
#include <Arduino.h>
#include "datatypes.h"
#include <plugins.h>

#include <a_pwm_dimmer.hpp>

// basic configuration

#define FIRMWARE_NAME "H801"
#define FIRMWARE_VERSION "0.0.1"

// definitions of nodes and properties

const HomieDeviceDef thisDevice PROGMEM = {"H801-02","Greenhouse Light-2"};

const HomieNodeDef PWMactuatorNode = {"H801","5ch PWM LED Dimmer", "LED Dimmer", 5, 0, {
    {"ch1","Channel R", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,1,600,0, 15},  // 0.1 fade step, delay per step, unused, GPIO
    {"ch2","Channel G", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,1,600,0, 13},
    {"ch3","Channel B", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,1,600,0, 12},
    {"ch4","Channel W1", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,1,600,0, 14},
    {"ch5","Channel W2", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,1,600,0, 4}
}};

const HomieNodeDef pwmSettings = {"pwmSettings","main Controls", "virtual device",4, 0, {
    {"speed","Speed","ms", DATATYPE_FLOAT, RETAINED, "0:60000",SETTABLE,1,0,0,0,0},
    {"increment","Increment","", DATATYPE_FLOAT, RETAINED, "0:1023",SETTABLE,1,0,0,0,0},
    {"duration","Duration","", DATATYPE_FLOAT, RETAINED, "0:3600",SETTABLE,1,0,0,0,0},
    {"frequency","Frequency","ms", DATATYPE_FLOAT, RETAINED, "100:40000",SETTABLE,1,0,0,0,0}
}};

const HomieNodeDef tuneable1 = {"tuneable1","Tunable White W1:W2", "virtual device",4, 0, {
    {"switch","Switch", "", DATATYPE_BOOLEAN, RETAINED, "0:1",SETTABLE,0.1,0,0,0,0},
    {"dimmer","Dimmer", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,0,0,0,0},
    {"temp","Color temperature", "K", DATATYPE_FLOAT, RETAINED, "3000:6000",SETTABLE,10,0,0,0,0},
    {"duration","Dimmer duration","s", DATATYPE_FLOAT, RETAINED, "0:3600",SETTABLE,1,0,0,0,0},
}};

const HomieNodeDef tuneable2 = {"tuneable2","Tunable White G:B", "virtual device",4, 0, {
    {"switch","Switch", "", DATATYPE_BOOLEAN, RETAINED, "0:1",SETTABLE,0.1,0,0,0,0},
    {"dimmer","Dimmer", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,0,0,0,0},
    {"temp","Color temperature", "K", DATATYPE_FLOAT, RETAINED, "3000:6000",SETTABLE,10,0,0,0,0},
    {"duration","Dimmer duration","s", DATATYPE_FLOAT, RETAINED, "0:3600",SETTABLE,1,0,0,0,0},
}};

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
const int PIN_LED    = 5;      // default on board signal LED
const int PIN_BUTTON = -1;     // no pushbutton

// device code
bool pwmSettingsHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    Plugin *plugin = myDevice.getNode("H801")->plugin;
    myLog.printf(LOG_INFO,F(" pwmSettings %s/%s Plugin:%s '%s'"), homieNode->getId(), homieProperty->getId(), plugin->id(), value.c_str());
    if (homieProperty->isTarget("frequency")) {
        plugin->setOption(a_PWMdimmer::PWM_FREQUENCY, (int) value.toInt());
    } else
    if (homieProperty->isTarget("speed")) {
        plugin->setOption(a_PWMdimmer::DIMM_SPEED, (int) value.toInt());
    } else
    if (homieProperty->isTarget("increment")) {
        for (uint8_t i=0; i < myDevice.getNode("H801")->length(); i++) {
            plugin->setOption(a_PWMdimmer::PWM_FADE_INCREMENT ,i , value.toFloat());
        }
    } else
    if (homieProperty->isTarget("duration")) {
        for (uint8_t i=0; i < myDevice.getNode("H801")->length(); i++) {
            plugin->setOption(a_PWMdimmer::PWM_FADE_DURATION ,i , value.toFloat());
        }
    } else 
    {
        return false;
    }
    return true;
};

float dimmer[2] = {1,1};
float factor[2] = {1,1};
float temperature[2] = {6000,6000};

bool tuneableHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    MyHomieNode *dimmerNode = myDevice.getNode("H801");
    Plugin *plugin = dimmerNode->plugin;
    myLog.printf(LOG_INFO,F(" tuneableHandler %s/%s Plugin:%s '%s'"), homieNode->getId(), homieProperty->getId(), plugin->id(), value.c_str());
    float ww = 0;
    float cw = 0;
    if (homieProperty->isTarget("temp")) {
        if (homieNode->isTarget("tuneable1")) {
            temperature[0] = (value.toFloat()-3000) * 0.0666; //scale 3000-6000 to 0-200
        } else {
            temperature[1] = (value.toFloat()-3000) * 0.0666; //scale 3000-6000 to 0-200
        }
    } else 
    if (homieProperty->isTarget("dimmer")) {
        if (homieNode->isTarget("tuneable1")) {
            dimmer[0] = value.toFloat()/100;
        } else {
            dimmer[1] = value.toFloat()/100;
        }
    } else 
    if (homieProperty->isTarget("switch")) {
        if (homieNode->isTarget("tuneable1")) {
            factor[0] = (value.equalsIgnoreCase("true")) ? 1 : 0;
        } else {
            factor[1] = (value.equalsIgnoreCase("true")) ? 1 : 0;
        }
    } else 
    {   // no property handled here
        return false;
    }

    if (homieNode->isTarget("tuneable1")) {
        if (temperature[0]<100) {
            ww=100;
            cw=temperature[0];
        } else {
            ww=200-temperature[0];
            cw=100;
        }
        dimmerNode->setValue("ch4", ww*dimmer[0]*factor[0]);
        dimmerNode->setValue("ch5", cw*dimmer[0]*factor[0]);
    } 
    else {
        if (temperature[1]<100) {
            ww=100;
            cw=temperature[1];
        } else {
            ww=200-temperature[1];
            cw=100;
        }
        dimmerNode->setValue("ch2", ww*dimmer[1]*factor[1]);
        dimmerNode->setValue("ch3", cw*dimmer[1]*factor[1]);
    }
    return true;
};

// device setup
bool deviceSetup(void) {
    // DIMMER PWM Node
    myDevice.init(&thisDevice);
    myDevice.createHomieNode(PWM_ID, &PWMactuatorNode);
    myDevice.createHomieNode(VIRTUAL_ID, &pwmSettings)
        ->registerInputHandler(&pwmSettingsHandler);
    myDevice.createHomieNode(VIRTUAL_ID, &tuneable1)
        ->registerInputHandler(&tuneableHandler);
    myDevice.createHomieNode(VIRTUAL_ID, &tuneable2)
        ->registerInputHandler(&tuneableHandler);
}

#endif