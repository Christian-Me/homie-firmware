#ifndef LED_6ch10W_H__
#define LED_6ch10W_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG
#include <Arduino.h>
#include "datatypes.h"
#include <c_homie.h>
#include <plugins.h>

/********************************************************************
controller for high power constant current LEDs
LED driver with PWM input like the PT4115 often used in build in drivers
you have to hack the DIM Pin or buy a driver module with broken out PWM pin
Be aware! The PT4115 has no reverse polarity protection! It burst in flames if you mess things up. A full bridge rectifier can prevent this ;)

- ESP01s (1MB Version)
- 1 PWM controlled high power CC driver

*********************************************************************/

//plugin used in this device (only needed for option enums)
#include <a_pwm_dimmer.h>

// basic configuration
#define FIRMWARE_NAME "6ch10W"
#define FIRMWARE_VERSION "0.0.1"

// Pin assignmens

// general
const int PIN_LED    = 5;      // default on board signal LED
const int PIN_BUTTON = -1;     // no pushbutton


// callbacks
int invertPWMValue(Plugin* plugin, int value) {
    return plugin->maxValue()-value;
};

bool dimmerHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool done = false; // set to true if value was processed and no further tasks necessary
    if (homieProperty->isTarget("switch")) {
        homieNode->setOption("ch*",a_PWMdimmer::SCALE, (value.startsWith(F("true")) ? 1 : 0));
        homieProperty->sendValue((value.startsWith(F("true")) ? true : false)); // acknowledge value as successfully processed
        done = true;
    }
    if (homieProperty->isTarget("dimmer")) {
        float scale = value.toFloat()/100.0f;
        if (scale>1) scale = 1.0f;
        if (scale<0) scale = 0.0f;
        homieNode->setOption("ch*",a_PWMdimmer::SCALE, scale);
        homieProperty->sendValue(scale*100);
        done = true;
    }
    return done; // unprocessed properties will be handled by the default handler
}

bool settingsHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    MyHomieNode *node = myDevice.getNode("DIMMERS");
    if (node==NULL) return false;
    myLog.setAppName("SETTINGS");
    myLog.printf(LOG_INFO,F(" Settings %s/%s = '%s'"), homieNode->getId(), homieProperty->getId(), value.c_str());
    if (homieProperty->isTarget("frequency")) {
        myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FREQUENCY, (int) value.toInt());
        homieProperty->sendValue(value.toFloat()); // confirm directly as sample rate = 0;
    } else
    if (homieProperty->isTarget("duration")) {
        myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FADE_DURATION_S, (int) value.toInt());
        homieProperty->sendValue(value.toFloat());
    }
}

// device setup
bool deviceSetup(void) {
    myDevice.init({"LED-1ch10W-01","LED Dimmer 02"});

    // node attached to the hardware by property plugins using the default input handler
    myDevice.addNode({"DIMMERS","1ch PWM LED Dimmer", "LED Dimmer"})
        ->addProperty({"ch1","Channel 1", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,1,600,0}, PWM_ID, 0, 1023)    // GPIO00
        ->addProperty({"switch","Switch","", DATATYPE_BOOLEAN, RETAINED, "",SETTABLE,1,0,0,0})
        ->addProperty({"dimmer","Dimmer","%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,0,0,0})
        ->registerInputHandler(&dimmerHandler); // register a single input handler for this node

    myDevice.addNode({"SETTINGS","device settings", "virtual device"})
        ->addProperty({"duration","Duration","s", DATATYPE_INTEGER, RETAINED, "0:3600",SETTABLE,1,0,0,0})
        ->addProperty({"frequency","Frequency","hz", DATATYPE_INTEGER, RETAINED, "100:40000",SETTABLE,1,0,0,0})
        ->registerInputHandler(&settingsHandler);

    // set default options
    myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FREQUENCY , 100); // 100hz PWM Frequency as PWM on a ESP8266 is software generated. 1Khz produces some random flicker (WiFi?)
    myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FADE_DURATION_MS , 100); // 100ms transition time for soft start/stop
    myDevice.setFilter("DIMMERS","ch*",invertPWMValue); // PWM-LED driver runs full power when pulled to ground
    return true;
}

void deviceLoop() {
};

#endif