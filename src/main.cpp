#include <Arduino.h>
#include <Homie.h>
#include "homieSyslog.h"
#include "projects.h"
#include <SPI.h>
#include "c_homie.h"
#include "config.h"

#ifdef M_NEOPIXEL
  #include "m_neopixel.h"
  m_neopixel neopixel;
#endif

#ifdef S_ADS115
  #include "s_ADS1115.h"
  s_ADS1115 ADS1115(1); // index 1
#endif

#ifdef S_BME280
  #include "s_BME280.h"
  s_BME280 BME280(1); // index 1
#endif

#ifdef S_BME680
  #include "s_BME680.h"
  s_BME680 BME680(2); // index 2 (secondary address)
#endif

bool resetHandler(const HomieRange& range, const String& value) {
  ESP.reset();
  return true;
}

bool globalInputHandler(const HomieNode& node, const HomieRange& range, const String& property, const String& value) {
  myLogf(LOG_INFO,F("[Global] Received for node '%s': %s=%s"),node.getId(),property.c_str(),value.c_str());
  MyHomieNode* homieNode = myDevice.getNode(node.getId());
  if (homieNode != NULL) {
    MyHomieProperty* homieProperty = homieNode->getProperty(property.c_str());
    if (homieProperty != NULL) {
      return homieProperty->inputHandler(range,value,homieNode, homieProperty);
    } else {
      myLogf(LOG_ERR,F(" Property '%s' not found"),property.c_str());
    }
  } else {
    myLogf(LOG_ERR,F(" node '%s' not found"),node.getId());
  }
  return false;
}

void onHomieEvent(const HomieEvent& event) {
  mySysLog_setAppName("CORE");
  switch(event.type) {
      case HomieEventType::MQTT_READY:
        myLog(LOG_INFO, F("MQTT ready!"));
        normalOperation = true;
      break;
      case HomieEventType::MQTT_DISCONNECTED:
        myLogf(LOG_INFO, "MQTT disconnected reason: %d", event.mqttReason);
        normalOperation = false;
      break;
      case HomieEventType::WIFI_CONNECTED:
        mSysLog_start();
        mySysLog_setDeviceName(Homie.getConfiguration().deviceId);
        myLogf(LOG_INFO, "WiFi ready ip: %s", event.ip.toString().c_str());
      break;
      case HomieEventType::WIFI_DISCONNECTED:
        normalOperation = false;
      break;    
      case HomieEventType::SENDING_STATISTICS:
        myLog(LOG_TRACE, F("Sending stats"));
      break;
      case HomieEventType::OTA_STARTED:
        myLog(LOG_INFO, F("OTA started"));
        normalOperation = false;
      break;
      case HomieEventType::OTA_PROGRESS:
      break;
      case HomieEventType::OTA_SUCCESSFUL:
        myLog(LOG_INFO, F("OTA success"));
      break;
      case HomieEventType::OTA_FAILED:
        myLog(LOG_ERR, F("OTA failed"));
        normalOperation = true;
      break;
      case HomieEventType::ABOUT_TO_RESET:
        myLog(LOG_WARNING, F("OTA success"));
        normalOperation = false;
      break;
      case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
      break;
      case HomieEventType::NORMAL_MODE:
        myLog(LOG_INFO, F("Normal Mode"));
        normalOperation = true;
      break;
      case HomieEventType::READY_TO_SLEEP:
        myLog(LOG_WARNING, F("Ready to Sleep"));
      break;
      case HomieEventType::STANDALONE_MODE:
        myLog(LOG_INFO, F("Standalaone Mode"));
        normalOperation = true;
      break;
      case HomieEventType::CONFIGURATION_MODE:
        myLog(LOG_INFO, F("Configuration Mode"));
        normalOperation = false;
      break;
  }
}

void loopHandler() {
  updateLED();
  if (normalOperation) {
    /*
    MyHomieNode* node = NULL;
    MyHomieProperty* property = NULL;
    for (int i=0; i<myDevice.length(); i++) {
      myLogf(LOG_DEBUG,F("loopHandler node #%d"),i);
      node = myDevice.getNode(i);
      myLogf(LOG_DEBUG,F(" loopHandler for '%s'"),node->getDef()->id);
      if (node!=NULL) {
        myLogf(LOG_TRACE,F(" Node %s %d properties"),node->getDef()->id, node->length());
        for (int j=0; j<node->length(); j++) {
          property = node->getProperty(j);
          if (!property->getDef()->settable){
            myLogf(LOG_TRACE,F("  Property %s"),property->getDef()->id);
            if (property->readyToSample()) {
              myLogf(LOG_DEBUG,F("  Sample %s"),property->getDef()->id);
            }
            if (property->readyToSend()) {
              myLogf(LOG_DEBUG,F("  Send %s"),property->getDef()->id);
            }
          }
        }
        if (node->plugin != NULL) {
          myLogf(LOG_DEBUG,F("  Plugin loop for '%s'"),property->getDef()->id);
          node->plugin->loop();
        }
      } else {
        myLogf(LOG_ERR,F("getNode results NULL! %d"),node);
      }
    }
   
    #ifdef S_ADS115
      if (ADS1115.read()) triggerLED();
    #endif

    #ifdef S_BME280
      if (BME280.read()) triggerLED();
    #endif
    
    #ifdef S_BME680
      if (BME680.read()) {
        triggerLED();
        #ifdef M_NEOPIXEL
          neopixel.disable();
        #endif
      }
    #endif
    
    #ifdef S_BH1750
      // if (BH1750.read()) triggerLED();
    #endif

    #ifdef M_NEOPIXEL // neopixel loop
      neopixel.loop();
    #endif
    */
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
  #else
    Serial1.begin(115200);
    Serial1 << endl << endl;
    Homie.setLoggingPrinter(&Serial1);
  #endif
  
  bootflags bootResult = bootmode_detect();
  String string = ESP.getResetInfo();
  myLogf(LOG_INFO,F("Reset cause: %d (%s)"),bootResult.raw_rst_cause,string.c_str());
  if (string.startsWith("Fatal exception")) {
    if (string.substring(string.indexOf(':')+1,string.indexOf(':')+3).toInt()!=28) {
      myLogf(LOG_EMERG,F(" Fatal Exception %s thrown! Going into save mode."),string.substring(string.indexOf(':')+1,string.indexOf(':')+3).c_str());
      Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
      Homie.setup();
      return;
    } else {
      myLog(LOG_EMERG,F(" reset after OTA."));
    }
  }

  myLogf(LOG_INFO,F("Firmware: %s Version: %s"),FIRMWARE_NAME,FIRMWARE_VERSION);
  
  Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
  Homie.setGlobalInputHandler(globalInputHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  Homie.setLedPin(LED_BUILTIN, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);

  string=F("Modules: ");

  if (deviceSetup()) {

  } else {
    
  }

  #ifdef A_PWM
    string += F("PWM ");
  #endif

  #ifdef M_NEOPIXEL // using neopixel and setting the brightness value
    neopixel.init( NEOPIXEL_PIN , "", 1, NEO_RGBW, 5);
    string += F("NEOPIXEL ");
  #endif

  #ifdef S_BME280
     BME280.init();
     string += F("BME280 ");
  #endif

  #ifdef S_ADS115
     ADS1115.init();
     string += F("ADS1115 ");
  #endif

  #ifdef S_BME680
    BME680.init();
    string += F("BME680 ");
  #endif

  #ifdef S_BH1750
    string += F("BH1750 ");
  #endif

  #ifdef LOG_DEVICE
    mySysLog_device(LOG_DEVICE); // LOG2SERIAL|LOG2SYSLOG
  #else
    mySysLog_device(LOG2SERIAL|LOG2SYSLOG);
  #endif
  mSyslog_setup(SYSLOG_PROTO_BSD);
  #ifdef HOMIE_DISABLE_LOGGING
    Homie.disableLogging();
  #endif
  Homie.setup();
  string += F("- Setup complete");
  myLog(LOG_DEBUG,string.c_str());
}

void loop() {
  Homie.loop();
}