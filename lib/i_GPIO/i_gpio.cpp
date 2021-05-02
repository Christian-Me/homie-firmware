#include <Arduino.h>
#include "i_GPIO.h"
#include <homie.h>
#include "homieSyslog.h"
#include "c_homie.h"
/*!
   @brief    get the id string of the plugin
    @returns    id string (const char*)
*/
const char* i_GPIO::id() {
  return "iGPIO";
}

/*!
   @brief   get the GPIO plugin status (isInitialized)
    @param    index     index
*/
i_GPIO::i_GPIO() {
  _isInitialized = false;
}

/*!
   @brief    init GPIO plugin
    @param    homieProperty    pointer to property definition
    @returns  (boolean) - true=success
*/
bool i_GPIO::init(MyHomieProperty* homieProperty, uint8_t gpio) {
  _homieProperty = homieProperty;
  _gpio = gpio;
  myLog.setAppName(id());
  myLog.printf(LOG_INFO,F("GPIO input id: %s GPIO%d"),_homieProperty->getDef().id, _gpio);

  pinMode(_gpio, INPUT);
  _current = digitalRead(_gpio);

  _isInitialized = true;  
  return _isInitialized;
}
/*!
   @brief    check if device is initialized and/or if sensor is in operation
    @returns    true if so
*/
bool i_GPIO::checkStatus(void) {
  return _isInitialized;
}
/*!
   @brief    set a plugin option
    @param    option    (uint16_t) option Id
    @param    value     (int) option value
    @returns  (boolean) true if option set successfull
*/
bool i_GPIO::setOption(uint16_t option, int value) {
  bool success = false;
  myLog.setAppName(id());
  switch (option) {
    case DEBOUNCE_TIME: {
        _debounceDelay = value;
        myLog.printf(LOG_INFO,F("  debounce delay %dms"), value);
        success = true;
      }
      break;
    case MODE: {
        _mode = value;
        myLog.printf(LOG_INFO,F("  gpio input mode = %d"), value);
        success = true;
      }
      break;
  }
  return success;
};
/*!
   @brief    read/update current value
    @returns    (float) current value
*/
float i_GPIO::get() {
  _current = digitalRead(_gpio); 
  return _current;
}

/*!
   @class  i_GPIO
   @brief  loop function of the aGIO plugin
   @details The  input GPIO Plugin comes with it's own loop for continuos scan and switch debounce. 
*/
void i_GPIO::loop() {
  if (_timer == 0) {
    _lastRead = digitalRead(_gpio);
    if (_lastRead!=_current) {
      _current = _lastRead;
      myLog.setAppName(id());
      myLog.printf(LOG_INFO,F("  GPIO%d=%d"), _gpio, _current);
      _homieProperty->sendValue(_current);
      _timer = millis()+_debounceDelay;
    }
  } else if (_timer<millis()) _timer=0; // reset timer
}