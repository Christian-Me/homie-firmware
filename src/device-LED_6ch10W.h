#ifndef LED_6ch10W_H__
#define LED_6ch10W_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG
#include <Arduino.h>
#include "datatypes.h"
#include <plugins.h>

/********************************************************************
controller for high power constant current LEDs
LED driver with PWM input like the PT4115 often used in build in drivers
you have to hack the DIM Pin or buy a driver module with broken out PWM pin
Be aware! The PT4115 has no reverse polarity protection! It burst in flames if you mess things up. A full bridge rectifier can prevent this

- 6 PWM controlled high power CC drivers

*********************************************************************/

//plugin used in this device (only needed for option enums)
#include <a_pwm_dimmer.h>

// basic configuration
#define FIRMWARE_NAME "6ch10W"
#define FIRMWARE_VERSION "0.0.1"
// #define  TUNEABLE_WHITE

// Pin assignmens

// general
const int PIN_LED    = 5;      // default on board signal LED
const int PIN_BUTTON = -1;     // no pushbutton


// device setup
bool deviceSetup(void) {
    // DIMMER PWM Node
    myDevice.init({"LED-6ch10W-01","LED Dimmer 02"});

    // node attached to the hardware by property plugins using the default input handler
    myDevice.addNode({"DIMMERS","6ch PWM LED Dimmer", "LED Dimmer"})
        ->addProperty({"ch1","Channel 1", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D3)    // GPIO00
        ->addProperty({"ch2","Channel 2", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D4)    // GPIO02
        ->addProperty({"ch3","Channel 3", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D5)    // GPIO14
        ->addProperty({"ch4","Channel 4", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D6)    // GPIO12
        ->addProperty({"ch5","Channel 5", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D7)    // GPIO13
        ->addProperty({"ch6","Channel 6", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D8);   // GPIO15

    myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FREQUENCY , 100); // 100hz PWM Frequency as PWM on a ESP8266 is software generated. 1Khz produces some random flicker (WiFi?)
    myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FADE_DURATION_MS , 100); // 100ms transition time for soft start/stop
    return true;
}

void deviceLoop() {
};

#endif