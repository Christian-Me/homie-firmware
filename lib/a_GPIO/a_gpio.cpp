#include <Arduino.h>
#include "a_gpio.h"
#include <homie.h>
#include "homieSyslog.h"
#include "c_homie.h"
/*!
   @brief    get the id string of the plugin
    @returns    id string (const char*)
*/
const char* a_GPIO::id() {
  return "aGPIO";
}

/*!
   @brief   get the GPIO plugin status (isInitialized)
    @param    index     index
*/
a_GPIO::a_GPIO() {
  _isInitialized = false;
}

/*!
   @brief    init GPIO plugin
    @param    homieProperty    pointer to property definition
    @returns  (boolean) - true=success
*/
bool a_GPIO::init(MyHomieProperty* homieProperty, uint8_t gpio) {
  _homieProperty = homieProperty;
  _gpio = gpio;
  myLog.setAppName(id());
  myLog.printf(LOG_INFO,F("GPIO actuator id: %s GPIO%d"),_homieProperty->getDef().id, _gpio);

  pinMode(_gpio, OUTPUT);
  digitalWrite(_gpio, HIGH);
  _duration=0;

  _isInitialized = true;  
  return _isInitialized;
}
/*!
   @brief    check if device is initialized and/or if sensor is in operation
    @returns    true if so
*/
bool a_GPIO::checkStatus(void) {
  return _isInitialized;
}
/*!
   @brief    set a plugin option
    @param    option    (uint16_t) option Id
    @param    value     (int) option value
    @returns  (boolean) true if option set successfull
*/
bool a_GPIO::setOption(uint16_t option, int value) {
  bool success = false;
  myLog.setAppName(id());
  switch (option) {
    case GPIO_DURATION: {
        _duration = value;
        myLog.printf(LOG_INFO,F("  Duration %dms"), value);
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
float a_GPIO::get() {
  return _current;
}
/*!
   @class  a_GPIO
   @brief  write a value to channel
    @param  data   integer value
    @returns    (boolean) true = achnkoleged new value
*/
bool a_GPIO::set(int data) {
  PropertyData propertyData;
  propertyData.current=floor(data);
  return set(&propertyData);
};
/*!
   @class  a_GPIO
   @brief  write a value to channel
    @param  data   pointer to PropertyData
    @returns    (boolean) true = achnkoleged new value
*/
bool a_GPIO::set(PropertyData* data) {
  myLog.setAppName(id());
  if (_isInitialized) {
    _current=(data->current<1) ? false : true;
     digitalWrite(_gpio, _current);
    if (_duration==0) { // on initialisation or no fading = direct control
      myLog.printf(LOG_INFO,F("  GPIO%d set to %d"), _gpio, _current);
      return true;
    } else {
      _timer = millis() + _duration;
      myLog.printf(LOG_INFO,F("  GPIO%d set to %d for %dms"), _gpio, _current, _duration);
      return true;
    }
  }
  myLog.printf(LOG_ERR,F("%s not initialized!"), id());
  return false;
}

/*!
   @class  a_GPIO
   @brief  loop function of the aGIO plugin
   @details The aGPIO Plugin comes with it's own loop and timer to handle pulse state changes. 
*/
void a_GPIO::loop() {
  // myLog.printf(LOG_TRACE,F(" %s loop"), id());
  if (_timer > 0 && _timer<millis()) {
    _current = !_current;
    digitalWrite(_gpio,_current);
    myLog.setAppName(id());
    myLog.printf(LOG_INFO,F("  GPIO%d=%d after %dms"), _gpio, _current, _duration);
    _timer = 0;
  }
}