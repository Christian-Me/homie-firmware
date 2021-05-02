#include <Arduino.h>
#include "signalLED.h"

#define LED_PERM_VALUE 1020   // standby dimmed value
#define LED_DIM_STEP 750      // micros pro step > 0-1000 * 50us total 50ms

void SignalLED::setLedPin(int pin){
  _ledPin = pin;
}

void SignalLED::trigger(int value){
  ledValue = value;
  ledNextUpdate = micros()+LED_DIM_STEP;
  analogWrite(_ledPin,ledValue);
}

void SignalLED::loop() {
  if (ledValue<LED_PERM_VALUE) {
    if (micros()>ledNextUpdate) {
      ++ledValue;
      ledNextUpdate = micros()+LED_DIM_STEP;
      analogWrite(_ledPin,ledValue);  
    }
  }
}

SignalLED signalLED;