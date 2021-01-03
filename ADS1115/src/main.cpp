#include "modules.h"
#include <Homie.h>
#include "../../include/config.h"
#include "utils.h"

#define FIRMWARE_NAME "ADS1115"
#define FIRMWARE_VERSION "0.0.1"

#ifdef S_ADS115
  #include "s_ADS1115.h"
  s_ADS1115 ADS1115(1,4); // index 1 / 4 sensors
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

void homieParameterSet(const __FlashStringHelper *nodeId, const __FlashStringHelper *parameterId, bool state) {
  String topic = "";
  topic = Homie.getConfiguration().mqtt.baseTopic;
  topic+=Homie.getConfiguration().deviceId;
  topic+="/";
  topic+=nodeId;
  topic+="/";
  topic+=parameterId;
  topic+="/set";
  Homie.getMqttClient().publish(topic.c_str(),2,true,(state ? "true" : "false"));
}

bool resetHandler(const HomieRange& range, const String& value) {
  ESP.reset();
  return true;
}

void loopHandler() {

    updateLED();
    
    #ifdef S_ADS115
      if (ADS1115.read()) triggerLED();
    #endif
}

void setup() {

  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
  Homie.setLoopFunction(loopHandler);
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);

  #ifdef S_ADS115
     ADS1115.init(500,0,5,10);
    // ADS1115.init(1000,600,10,30);
    // ADS1115.setScaler(1,5.0/3.292);
  #endif

  Homie.setup();
}

void loop() {
  Homie.loop();
}