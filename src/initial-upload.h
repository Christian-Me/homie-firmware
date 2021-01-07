#ifndef INITIAL_UPLOAD_H__
#define INITIAL_UPLOAD_H__

#include <Arduino.h>
#include "homieSyslog.h"
#define MAX_DATAPOINTS 5 // define the maximum datapoints per nodehere (of all your nodes)
#define MAX_ACTUATORS 5  // define the maximum actuators per node here (of all your nodes)
#include "../include/datatypes.h"

// basic configuration

#define FIRMWARE_NAME "H801"
#define FIRMWARE_VERSION "0.0.1"

#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG

// modules to include

// Pin assignmens

// general
const int PIN_LED = 5;         // default on board signal LED
const int PIN_BUTTON = -1;      // no pushbutton

#endif