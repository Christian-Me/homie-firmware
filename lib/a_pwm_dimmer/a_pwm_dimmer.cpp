#include <Arduino.h>
#include "a_pwm_dimmer.h"
#include <homie.h>
#include "homieSyslog.h"
#include "c_homie.h"

const char* a_PWMdimmer::id() {
  return "PWM";
}

/*!
   @brief    PWM PWM "analog" out actuator
    @param    index     index
*/
a_PWMdimmer::a_PWMdimmer() {
  _isInitialized = false;
}

/*!
   @brief    init PWM controller 
    @param    nodeDef           pointer to node definition
    @returns  result boolean - true=success
*/
bool a_PWMdimmer::init(MyHomieProperty* homieProperty, uint8_t gpio) {
  _homieProperty = homieProperty;
  _gpio = gpio;
  myLog.setAppName(id());
  myLog.printf(LOG_INFO,F("PWM Dimmer id: %s GPIO%d"),homieProperty->getDef().id, _gpio);

  pinMode(_gpio, OUTPUT);
  analogWrite(_gpio, 0);
  _increment=1;
  _target=0;
  _current=-1;
  _duration=0;
  _delay=1000;

  _isInitialized = true;  
  return _isInitialized;
}
/*!
   @brief    set a plugin option
    @param    option    (uint16_t) option ID: PWM_FREQUENCY to set the PWM frequency from 100 to 40kHz
    @param    value     (bool) option value
    @returns  true if option set successfull
*/
bool a_PWMdimmer::setOption(uint16_t option, int value) {
  bool success = false;
  myLog.setAppName(id());
  switch (option) {
    case PWM_FREQUENCY:
      if (value>=100 && value<=40000) {
        analogWriteFreq(value);
        myLog.printf(LOG_INFO,F("  PWM frequency %dHz"), value);
        success = true;
      } else success = false;
      break;
    case DIMM_SPEED:
      myLog.printf(LOG_INFO,F("  PWM sample delay = %d"), value);
      if (value>=0 && value<100000) {
        _delay = value;
        success = true;
      }
      break;
  }
  return success;
};
/*!
   @brief    set a channel option
    @param    option    (uint16_t) option ID: PWM_FREQUENCY to set the PWM frequency from 100 to 40kHz
    @param    channel   (uint8_t) channel number
    @param    value     (float) option value
    @returns  true if option set successfull
*/
bool a_PWMdimmer::setOption(uint16_t option, float value) {
  bool success = false;
  myLog.setAppName(id());
  switch (option) {
    case PWM_FADE_INCREMENT:
      myLog.printf(LOG_INFO,F("  PWM GPIO%d fade resolution %.2f"), _gpio, value);
      if (value>0 && value<1023) {
        _increment = value;
        success = true;
      }
      break;
    case PWM_FADE_DURATION:
      if (value>=0 && value<=3600) {
        _duration = (uint16_t) floor(value);
        myLog.printf(LOG_INFO,F("  PWM GPIO%d fade duration %ds"), _gpio, _duration);
        success = true;
      }
      break;
  }
  return success;
};
/*!
   @brief    read/update current value
    @param    channel   number of channel to read
    @returns    current value
*/
float a_PWMdimmer::get() {
  return _current;
}

/*!
   @class  a_PWMdimmer
   @brief  write a value to channel
    @param  channel   number of channel to write to
    @returns    true = achnkoleged new value
*/
bool a_PWMdimmer::set(PropertyData* data){
  myLog.setAppName(id());
  if (_isInitialized) {
    if (_current<0 || _duration==0) { // on initialisation or no fading = direct control
      _current=data->current;
      _target=data->current;
      analogWrite(_gpio, floor(_current * 10.23));
      myLog.printf(LOG_INFO,F("  PWM GPIO%d set to %.2f"), _gpio, _target);
      return true;
    } else {
      _target = data->current;
      _increment=(_target - _current) / (_duration * 1000 / _delay);
      myLog.printf(LOG_INFO,F("  PWM GPIO%d set to %.2f from %.2f in %ds (%.2f per %dms)"), _gpio, _target, _current, _duration, _increment, _delay);
      _timer = millis()+_delay;
      return false; // do not achnkoleged jet. Will be done by timer or threshold
    }
  }
  myLog.printf(LOG_ERR,F("PWM not initialized!"));
  return false;
}

bool a_PWMdimmer::checkStatus(void) {
  return _isInitialized;
}

bool a_PWMdimmer::available(void) {
  return true;
}

/*!
   @class  a_PWMdimmer
   @brief  loop function of the PWM dimmer plugin
   @details The PWM Plugin comes with it's own loop and timer for frequent updates during dimming events. 
    If the timer ist riggers it loops through all channels and in- or decrements the `_current[cannel]` value to reach the `_target[cannel]`
    The limits are checked and the current value is fine tuned if the target is reached within the `_increment` range
*/
void a_PWMdimmer::loop() {
  if (_timer > 0 && _timer<millis()) {
    _timer = millis()+_delay;
    if (_target!=_current) {
      myLog.setAppName(id());
      if (_duration>1) {
        _current += _increment;
        if (_current < 0) _current = 0; // too low
        if (_current > 100) _current = 100; // too big
        if (fabs(_target-_current)<=_increment) { // too close
          _current = _target;
        }
      } else {
        _current = _target;
      }
      myLog.printf(LOG_TRACE,F("  analogWrite GPIO%d %.2f=%d (Target %.2f Duration %ds)"), _gpio, _current, (int) floor(_current * 10.23), _target, _duration);
      analogWrite(_gpio,(int) floor(_current * 10.23));
    }
  }
}