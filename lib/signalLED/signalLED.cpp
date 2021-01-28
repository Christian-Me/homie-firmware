#include <Arduino.h>
#include "signalLED.h"

#define LED_PERM_VALUE 1020   // standby dimmed value
#define LED_DIM_STEP 750      // micros pro step > 0-1000 * 50us total 50ms

int ledValue = 0;
unsigned long ledNextUpdate = 0;

void triggerLED(int value){
  ledValue = value;
  ledNextUpdate = micros()+LED_DIM_STEP;
  analogWrite(LED_BUILTIN,ledValue);
}

void updateLED() {
  if (ledValue<LED_PERM_VALUE) {
    if (micros()>ledNextUpdate) {
      ++ledValue;
      ledNextUpdate = micros()+LED_DIM_STEP;
      analogWrite(LED_BUILTIN,ledValue);  
    }
  }
}