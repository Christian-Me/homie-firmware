#ifdef HOMIE_FIRMWARE_HLW8012
#ifndef DEVICE_H801_H__
#define DEVICE_H801_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG
#include <Arduino.h>
#include "datatypes.h"
#include <plugins.h>
#include <signalLED.h>

#include <s_HLW8012.h>

/********************************************************************
controller for LED lighting in my indoor plant greenhouse

- 2 Relays
- 2 GPIO inputs
- HLW8012 power measurement

*********************************************************************/

// basic configuration
#define FIRMWARE_NAME "HLW8012"
#define FIRMWARE_VERSION "0.0.1"

// Pin assignmens

// general
const int PIN_LED    = 0;      // default on board signal LED
const int PIN_BUTTON = -1;     // no pushbutton

// device setup
bool deviceSetup(void) {
    // DIMMER PWM Node
    signalLED.setLedPin(0);
    myDevice.init({"HLW8012-01","Milfra 2 Relay POW"});

    // node attached to the hardware by property plugins using the default input handler
    myDevice.addNode({"HLW8012","Mains Power", "power meter"},HLW8012_ID, 0)
        ->addProperty({"power","Active Power", "W", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,10,600,5}, s_HLW8012::CHANNEL_POWER)
        ->addProperty({"voltage","Voltage", "V", DATATYPE_FLOAT, RETAINED, "0:250",NON_SETTABLE,1,10,600,5}, s_HLW8012::CHANNEL_VOLTAGE)
        ->addProperty({"current","Current", "A", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,10,600,5}, s_HLW8012::CHANNEL_CURRENT)
        ->addProperty({"powerFactor","Power Factor", "%", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,10,600,5}, s_HLW8012::CHANNEL_POWER_FACTOR)
        ->addProperty({"energy","Energy", "Ws", DATATYPE_FLOAT, RETAINED, "0:100",NON_SETTABLE,1,10,600,0}, s_HLW8012::CHANNEL_ENERGY);

    myDevice.setOption("HLW8012","",s_HLW8012::SET_SEL_PIN,12);
    myDevice.setOption("HLW8012","",s_HLW8012::SET_CF_PIN,5);
    myDevice.setOption("HLW8012","",s_HLW8012::SET_CF1_PIN,4);
 
    myDevice.addNode({"RELAYS","2ch relays", "GPIO OUT"})
      ->addProperty({"relay1","Relay 1", "", DATATYPE_BOOLEAN, RETAINED, "",SETTABLE,0.1, 0, 600,0},aGPIO_ID,15)   // 0.1 report threshold, no sampling, report every 10min
      ->addProperty({"relay2","Relay 2", "", DATATYPE_BOOLEAN, RETAINED, "",SETTABLE,0.1, 0, 600,0},aGPIO_ID,3);  // 0.1 fade step, delay per step, unused, GPIO&PWMactuatorNode);

    myDevice.addNode({"SWITCHES","2ch mains gpio in", "GPIO IN"})
      ->addProperty({"switch1","Switch 1", "", DATATYPE_BOOLEAN, RETAINED, "",NON_SETTABLE,0.1, 0, 600,0},iGPIO_ID,16)   // 0.1 report threshold, continuos sampling, report every 10min
      ->addProperty({"switch2","Switch 2", "", DATATYPE_BOOLEAN, RETAINED, "",NON_SETTABLE,0.1, 0, 600,0},iGPIO_ID,13);  
    
    myDevice.addNode({"BUZZER","on board buzzer", "PWM BUZZER"})
      ->addProperty({"command","Command String", "", DATATYPE_STRING, NOT_RETAINED, "",SETTABLE, 0, 0, 0, 0}, PWM_ID, 14);

    return true;
}

void deviceLoop() {
};

#endif
#endif