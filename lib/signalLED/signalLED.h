#ifndef SIGNAL_LED_H__
#define SIGNAL_LED_H__
#include <Arduino.h>

class SignalLED {
  private:
    int _ledPin = LED_BUILTIN;
    int ledValue = 0;
    unsigned long ledNextUpdate = 0;
  public:
    void setLedPin(int pin);
    void trigger(int value = 0);
    void loop();
};

extern SignalLED signalLED;
#endif