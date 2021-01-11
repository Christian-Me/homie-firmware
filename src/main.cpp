#include <Arduino.h>
#include "projects.h"
#include <SPI.h>
#include <Homie.h>
#include "homieSyslog.h"
#include "c_homie.h"
#include "config.h"

#ifdef A_PWM
  #include "a_pwm_dimmer.h"
  a_PWMdimmer PWM;
#endif

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

#ifdef S_BH1750
  #include "s_BH1750.h"
  s_BH1750 BH1750(1); // index 1
#endif

#define LED_PERM_VALUE 1000   // standby dimmed value
#define LED_DIM_STEP 500      // micros pro step > 0-1000 * 50us total 50ms

int ledValue = 0;
unsigned long ledNextUpdate = 0;

void triggerLED(int value = 0){
  ledValue = value;
  ledNextUpdate = micros()+LED_DIM_STEP;
  analogWrite(PIN_LED,ledValue);
}

void updateLED() {
  if (ledValue<LED_PERM_VALUE) {
    if (micros()>ledNextUpdate) {
      ++ledValue;
      ledNextUpdate = micros()+LED_DIM_STEP;
      analogWrite(PIN_LED,ledValue);  
    }
  }
}

bool resetHandler(const HomieRange& range, const String& value) {
  ESP.reset();
  return true;
}

void loopHandler() {
  if (normalOperation) {
    updateLED();
    
    #ifdef A_PWM
      PWM.loop();
    #endif
    
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
      if (BH1750.read()) triggerLED();
    #endif

    #ifdef M_NEOPIXEL // neopixel loop
      neopixel.loop();
    #endif
  }
}

bool globalInputHandler(const HomieNode& node, const HomieRange& range, const String& property, const String& value) {
  myLogf(LOG_INFO,F("[Global] Received on node %s: %s = %s"),node.getId(),property.c_str(),value.c_str());
  #ifdef A_PWM
    return PWM.inputHandler(property,value);
  #endif
  return true;
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
  #ifndef USE_SERIAL1
    Serial.begin(115200);
    Serial << endl << endl;
  #else
    Serial1.begin(115200);
    Serial1 << endl << endl;
    Homie.setLoggingPrinter(&Serial1);
  #endif
  
  bootflags bootResult = bootmode_detect();
  
  myLogf(LOG_INFO,F("Reset cause: %d (%s)"),bootResult.raw_rst_cause,ESP.getResetInfo().c_str());

  myLogf(LOG_INFO,F("Firmware: %s Version: %s"),FIRMWARE_NAME,FIRMWARE_VERSION);
  
  Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
  Homie.setGlobalInputHandler(globalInputHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);

  string=F("Modules: ");

  if (deviceSetup()) {

  } else {
    
  }

  #ifdef A_PWM
    PWM.init();
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
    BH1750.init();
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