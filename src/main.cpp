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
  s_ADS1115 ADS1115(1,4); // index 1 / 4 sensors
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

void setup() {
  #ifndef USE_SERIAL1
    Serial.begin(115200);
    Serial << endl << endl;
  #else
    Serial1.begin(115200);
    Serial1 << endl << endl;
    Homie.setLoggingPrinter(&Serial1);
  #endif
  myLogf(LOG_INFO,F("Firmware: %s Version: %s"),FIRMWARE_NAME,FIRMWARE_VERSION);
  Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
  Homie.setGlobalInputHandler(globalInputHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);

  #ifdef A_PWM
    PWM.init();
  #endif

  #ifdef M_NEOPIXEL // using neopixel and setting the brightness value
    neopixel.init( NEOPIXEL_PIN , "", 1, NEO_RGBW, 5);
  #endif

  #ifdef S_ADS115
     ADS1115.init(ADS1115_SAMPLE_RATE,ADS1115_TIMEOUT,ADS1115_OVERSAMPLING,ADS1115_THRESHOLD);
  #endif

  #ifdef S_BME280
     BME280.init(BME280_SAMPLE_RATE,BME280_TIMEOUT);
  #endif

  #ifdef S_BME680
    BME680.init();
  #endif

  #ifdef S_BH1750
     BH1750.init(BME280_SAMPLE_RATE,BME280_TIMEOUT);
  #endif

  #ifdef LOG_DEVICE
    mySysLog_device(LOG_DEVICE); // LOG2SERIAL|LOG2SYSLOG
  #else
    mySysLog_device(LOG2SERIAL|LOG2SYSLOG);
  #endif
  mSyslog_setup(SYSLOG_PROTO_BSD);
  Homie.setup();
  myLog(LOG_DEBUG,F("Setup complete"));
}

void loop() {
  Homie.loop();
}