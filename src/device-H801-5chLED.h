#ifndef DEVICE_H801_H__
#define DEVICE_H801_H__

#include <Arduino.h>
// #define HOMIE_DISABLE_LOGGING // homie internal log output disabled
#include "homieSyslog.h"
#define MAX_DATAPOINTS 5  // define the maximum datapoints per node here (of all your nodes)
#define MAX_ACTUATORS 10  // define the maximum actuators per node here (of all your nodes)
#include "../include/datatypes.h"

// basic configuration

#define FIRMWARE_NAME "H801"
#define FIRMWARE_VERSION "0.0.1"

#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG

// modules to include

#define A_PWM
const ActuatorNode PWMactuatorNode = {"H801","5ch PWM LED Dimmer", "LED Dimmer", 10, {
    {"ch1","Channel R", "%", "float", true, "0:100", 15},
    {"ch2","Channel G", "%", "float", true, "0:100", 13},
    {"ch3","Channel B", "%", "float", true, "0:100", 12},
    {"ch4","Channel W1", "%", "float", true, "0:100", 14},
    {"ch5","Channel W2", "%", "float", true, "0:100", 4},
    {"temp1","Temperature 1", "%", "float", true, "0:100", 0},
    {"dim1","Dimmer 1", "%", "float", true, "0:100", 0},
    {"temp2","Temperature 2", "%", "float", true, "0:100", 0},
    {"dim2","Dimmer 2", "%", "float", true, "0:100", 0},
    {"delay","transition delay", "us", "integer", true, "", 0}
}};

#include "sensors.h"
#include "actuators.h"

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
const int PIN_LED = 5;         // default on board signal LED
const int PIN_BUTTON = -1;     // no pushbutton

//callbacks

bool deviceSetup(void) {

}

#endif