#ifndef DEVICE_H801_H__
#define DEVICE_H801_H__

#define HOMIE_DISABLE_LOGGING // homie internal log output disabled. Comment this to see homie.h logs
#define LOG_DEVICE LOG2SERIAL|LOG2SYSLOG
// for now define max Datapoints in platformio.ini
// #define MAX_DATAPOINTS 5 // maximum of datapoints per node (ToDo: make this "dynamic")
#include <Arduino.h>
#include "datatypes.h"
#include <plugins.h>

/********************************************************************
controller for LED lighting in my indoor plant greenhouse

- 5 PWM controlled mosfets driving 4 different LED strips
- W1/W2 tunable white
- R 6000K COB strips
- G 6000K high Power LED strip
- B red LED strip

*********************************************************************/

//plugin used in this device
#include <a_pwm_dimmer.h>

// basic configuration
#define FIRMWARE_NAME "H801"
#define FIRMWARE_VERSION "0.0.1"
// #define  TUNEABLE_WHITE

// Pin assignmens

// I2C
#define I2C_PIN_SDA  D2
#define I2C_PIN_SCL  D1

// general
const int PIN_LED    = 5;      // default on board signal LED
const int PIN_BUTTON = -1;     // no pushbutton

// device code
bool pwmSettingsHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    MyHomieNode *nodeH801 = myDevice.getNode("H801");
    myLog.setAppName("H801");
    if (nodeH801==NULL) return false;
    myLog.printf(LOG_INFO,F(" H801 pwmSettings %s/%s = '%s'"), homieNode->getId(), homieProperty->getId(), value.c_str());
    if (homieProperty->isTarget("frequency")) {
        for (uint8_t i=0; i < nodeH801->length(); i++) { // use the same settings for all channels
            nodeH801->getProperty(i)->getPlugin()->setOption(a_PWMdimmer::PWM_FREQUENCY, (int) value.toInt());
        }
        homieProperty->sendValue(value.toFloat()); // confirm directly as sample rate = 0;
    } else
    if (homieProperty->isTarget("duration")) {
        for (uint8_t i=0; i < nodeH801->length(); i++) {
            nodeH801->getProperty(i)->getPlugin()->setOption(a_PWMdimmer::PWM_FADE_DURATION , (int) value.toInt());
        }
        homieProperty->sendValue(value.toFloat());
    } else 
    if (homieProperty->isTarget("shortestDelay")) {
        for (uint8_t i=0; i < nodeH801->length(); i++) {
            nodeH801->getProperty(i)->getPlugin()->setOption(a_PWMdimmer::PWM_FADE_DELAY , (int) value.toInt());
        }
        homieProperty->sendValue(value.toFloat());
    } else 
    {
        return false;
    }
    return true;
};

#ifdef H801_2
float dimmer[2] = {1,1};
float factor[2] = {1,1};
float temperature[2] = {100,100};

bool tuneableHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    MyHomieNode *dimmerNode = myDevice.getNode("H801");
    Plugin *plugin = dimmerNode->plugin;
    myLog.printf(LOG_INFO,F(" tuneableHandler %s/%s Plugin:%s '%s'"), homieNode->getId(), homieProperty->getId(), plugin->id(), value.c_str());
    float ww = 0;
    float cw = 0;
    if (homieProperty->isTarget("temp")) {
        temperature[0] = (value.toFloat()-3000) * 0.0666; //scale 3000-6000 to 0-200
    } else 
    if (homieProperty->isTarget("dimmer")) {
        dimmer[0] = value.toFloat()/100;
    } else 
    if (homieProperty->isTarget("switch")) {
        factor[0] = (value.equalsIgnoreCase("true")) ? 1 : 0;
    } else 
    {   // no property handled here
        return false;
    }

    if (temperature[0]<100) {
        ww=100;
        cw=temperature[0];
    } else {
        ww=200-temperature[0];
        cw=100;
    }
    dimmerNode->setValue("ch4", ww*dimmer[0]*factor[0]);
    dimmerNode->setValue("ch5", cw*dimmer[0]*factor[0]);
    homieProperty->sendValue(value.c_str());
    return true;
};
#endif

// device setup
bool deviceSetup(void) {
    // DIMMER PWM Node
    myDevice.init({"H801-01","Greenhouse Light-1"});

    // node attached to the hardware by property plugins using the default input handler
    myDevice.addNode(VIRTUAL_ID, {"H801","5ch PWM LED Dimmer", "LED Dimmer", 0})
    
    #ifdef H801_1
        ->addProperty({"ch1","Red", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 15)        // R =GPIO15
        ->addProperty({"ch2","Level 1", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 14)    // G =GPIO14
        ->addProperty({"ch3","Level 2", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 13)    // B =GPIO13
        ->addProperty({"ch4","Level 3", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 12)    // W1=GPIO12
        ->addProperty({"ch5","Level 4", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 4);    // W2=GPIO4
    #endif
    
    #ifdef H801_2
        ->addProperty({"ch1","CW", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 15)             // R =GPIO15
        ->addProperty({"ch2","Extra CW", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 13)       // G =GPIO14
        ->addProperty({"ch3","RED", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 12)            // B =GPIO13
        ->addProperty({"ch4","Tuneable1 WW", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 14)   // W1=GPIO12
        ->addProperty({"ch5","Tuneable1 CW", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, 4);   // W2=GPIO4
    #endif

    // virtual node to remotely set pwm and dimmer parameters for all 5 channels
    myDevice.addNode({"pwmSettings","dimmer Controls", "virtual device"})
        ->addProperty({"duration","Duration","s", DATATYPE_INTEGER, RETAINED, "0:3600",SETTABLE,1,0,0,0})
        ->addProperty({"shortestDelay","Shortest delay","ms", DATATYPE_INTEGER, RETAINED, "10:1000",SETTABLE,1,0,0,0})
        ->addProperty({"frequency","Frequency","hz", DATATYPE_INTEGER, RETAINED, "100:40000",SETTABLE,1,0,0,0})
        ->registerInputHandler(&pwmSettingsHandler);

#ifdef H801_2
    // virtual node for mixing W1:W2 for tunable white with boolean switch keeping dimmer values and dimmer keeping the color temperature (intentionally not the intensity)
    myDevice.addNode(VIRTUAL_ID,{"tuneable1","Tunable White W1:W2", "virtual device", 0})
        ->addProperty({"switch","Switch", "", DATATYPE_BOOLEAN, RETAINED, "0:1",SETTABLE,0.1,0,0,0})
        ->addProperty({"dimmer","Dimmer", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,0,0,0})
        ->addProperty({"temp","Color temperature", "K", DATATYPE_FLOAT, RETAINED, "3000:6000",SETTABLE,10,0,0,0})
        ->addProperty({"duration","Dimmer duration","s", DATATYPE_INTEGER, RETAINED, "0:3600",SETTABLE,1,0,0,0})
        ->registerInputHandler(&tuneableHandler);
#endif

    return true;
}

void deviceLoop() {
};

#endif