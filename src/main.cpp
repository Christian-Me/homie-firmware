#include <Arduino.h>
#include <homieSysLog.h>
#include <Homie.h>
#include "../include/datatypes.h"
#include "../include/globals.h"
#include <c_homie.h>
// #include <s_bme280.h>
#include "projects.h"
#include <SPI.h>
// #include "config.h"
#include <signalLED.h>


#ifdef M_NEOPIXEL
  #include "m_neopixel.h"
  m_neopixel neopixel;
#endif

bool resetHandler(const HomieRange& range, const String& value) {
  myLog.print(LOG_TRACE, F("resetting NOW!"));
  ESP.reset();
  return true;
}

bool globalInputHandler(const HomieNode& node, const HomieRange& range, const String& property, const String& value) {
  bool result = false;
  myLog.printf(LOG_INFO,F("deviceInputHandler %s/%s received = '%s'"),node.getId(),property.c_str(),value.c_str());
  MyHomieNode* homieNode = myDevice.getNode(node.getId());
  if (homieNode != NULL) {
    MyHomieProperty* homieProperty = homieNode->getProperty(property.c_str());
    if (homieProperty != NULL) {
      result = homieNode->inputHandler(range,value,homieNode, homieProperty); // ask node property handler first
      result = homieProperty->inputHandler(range,value,homieNode, homieProperty); // ask property input handler second 
    } else {
      myLog.printf(LOG_ERR,F(" property '%s/%s' not found"),node.getId(),property.c_str());
    }
  } else {
    myLog.printf(LOG_ERR,F(" node '%s' not found"),node.getId());
  }
  return result;
}

void onHomieEvent(const HomieEvent& event) {
  myLog.setAppName("CORE");
  switch(event.type) {
      case HomieEventType::MQTT_READY:
        myLog.print(LOG_INFO, F("MQTT ready!"));
        normalOperation = true;
      break;
      case HomieEventType::MQTT_DISCONNECTED:
        myLog.printf(LOG_INFO, "MQTT disconnected reason: %d", event.mqttReason);
        normalOperation = false;
      break;
      case HomieEventType::WIFI_CONNECTED:
        myLog.start();
        myLog.setDeviceName(Homie.getConfiguration().deviceId);
        myLog.printInfo(LOG_NETWORK);
      break;
      case HomieEventType::WIFI_DISCONNECTED:
        myLog.printInfo(LOG_NETWORK);
        normalOperation = false;
      break;    
      case HomieEventType::SENDING_STATISTICS:
        triggerLED();
        myLog.print(LOG_TRACE, F("Sending stats"));
      break;
      case HomieEventType::OTA_STARTED:
        myLog.print(LOG_INFO, F("OTA started"));
        normalOperation = false;
      break;
      case HomieEventType::OTA_PROGRESS:
      break;
      case HomieEventType::OTA_SUCCESSFUL:
        myLog.print(LOG_NOTICE, F("OTA success"));
      break;
      case HomieEventType::OTA_FAILED:
        myLog.print(LOG_ERR, F("OTA failed"));
        normalOperation = true;
      break;
      case HomieEventType::ABOUT_TO_RESET:
        myLog.print(LOG_WARNING, F("About to reset"));
        normalOperation = false;
      break;
      case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
      break;
      case HomieEventType::NORMAL_MODE:
        myLog.print(LOG_INFO, F("Normal Mode"));
        normalOperation = true;
      break;
      case HomieEventType::READY_TO_SLEEP:
        myLog.print(LOG_WARNING, F("Ready to Sleep"));
        Homie.doDeepSleep();
      break;
      case HomieEventType::STANDALONE_MODE:
        myLog.print(LOG_INFO, F("Standalaone Mode"));
        normalOperation = true;
      break;
      case HomieEventType::CONFIGURATION_MODE:
        myLog.print(LOG_NOTICE, F("Configuration Mode"));
        normalOperation = false;
      break;
  }
}

const unsigned long sampleDuration = 60000;
unsigned long deviceTime = 0;
unsigned long loopTimer = 0;

void loopHandler() {
  updateLED();
  if (normalOperation) {
    deviceTime += myDevice.loop();
  }
  deviceLoop();
  myLog.loop();
  if (loopTimer < millis()) {
    myLog.printf(LOG_INFO,F("Utilization %.1f%%"),(float) deviceTime/sampleDuration * 100);
    deviceTime = 0;
    loopTimer = sampleDuration + millis();
  }
}

struct bootflags
{
    unsigned char raw_rst_cause : 4;
    unsigned char raw_bootdevice : 4;
    unsigned char raw_bootmode : 4;

    unsigned char rst_normal_boot : 1;
    unsigned char rst_reset_pin : 1;
    unsigned char rst_watchdog : 1;

    unsigned char bootdevice_ram : 1;
    unsigned char bootdevice_flash : 1;
};

struct bootflags bootmode_detect(void) {
    int reset_reason, bootmode;
    asm (
        "movi %0, 0x60000600\n\t"
        "movi %1, 0x60000200\n\t"
        "l32i %0, %0, 0x114\n\t"
        "l32i %1, %1, 0x118\n\t"
        : "+r" (reset_reason), "+r" (bootmode) /* Outputs */
        : /* Inputs (none) */
        : "memory" /* Clobbered */
    );

    struct bootflags flags;

    flags.raw_rst_cause = (reset_reason&0xF);
    flags.raw_bootdevice = ((bootmode>>0x10)&0x7);
    flags.raw_bootmode = ((bootmode>>0x1D)&0x7);

    flags.rst_normal_boot = flags.raw_rst_cause == 0x1;
    flags.rst_reset_pin = flags.raw_rst_cause == 0x2;
    flags.rst_watchdog = flags.raw_rst_cause == 0x4;

    flags.bootdevice_ram = flags.raw_bootdevice == 0x1;
    flags.bootdevice_flash = flags.raw_bootdevice == 0x3;

    return flags;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  #ifndef USE_SERIAL1
    Serial.begin(115200);
    Serial << endl << endl;
    myLog.setup(&Serial);
  #else
    Serial1.begin(115200);
    Serial1 << endl << endl;
    Homie.setLoggingPrinter(&Serial1);
    myLog.setup(&Serial1,SYSLOG_PROTO_BSD);
  #endif
  myLog.printInfo(LOG_MEMORY);
  bootflags bootResult = bootmode_detect();
  String string = ESP.getResetInfo();
  myLog.printf(LOG_INFO,F("Reset cause: %d (%s)"),bootResult.raw_rst_cause,string.c_str());
  if (string.startsWith("Fatal exception")) {
    if (string.substring(string.indexOf(':')+1,string.indexOf(':')+3).toInt()!=28) {
      myLog.printf(LOG_EMERG,F(" Fatal Exception %s thrown! Going into save mode."),string.substring(string.indexOf(':')+1,string.indexOf(':')+3).c_str());
      Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
      Homie.setup();
      return;
    } else {
      myLog.print(LOG_EMERG,F(" reset after OTA."));
    }
  }

  myLog.printf(LOG_INFO,F("Firmware: %s Version: %s"),FIRMWARE_NAME,FIRMWARE_VERSION);
  
  Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
  Homie.setGlobalInputHandler(globalInputHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  Homie.setLedPin(LED_BUILTIN, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);
  // Homie.setLedPin(5 , LOW)

  if (deviceSetup()) {
    myLog.print(LOG_INFO,F("Device setup completed."));
  } else {
    myLog.print(LOG_ERR,F("Device setup unsuccessful!"));
  }

  #ifdef LOG_DEVICE
    myLog.device(LOG_DEVICE); // LOG2SERIAL|LOG2SYSLOG
  #else
    myLog.device(LOG2SERIAL|LOG2SYSLOG);
  #endif
  #ifdef HOMIE_DISABLE_LOGGING
    Homie.disableLogging();
  #endif
  myLog.printInfo(LOG_MEMORY);
  Homie.setup();
  myLog.print(LOG_DEBUG,F("Homie.setup started"));
}

void loop() {
  Homie.loop();
}