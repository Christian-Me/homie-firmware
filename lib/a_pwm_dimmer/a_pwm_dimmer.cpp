#include <Arduino.h>
#include "a_pwm_dimmer.h"
#include <homie.h>
#include "homieSyslog.h"
#include "c_homie.h"
#include <utils.h>

int defaultFilter(Plugin* plugin, int value) {
  return value;
}
/*!
   @brief    get the id string of the plugin
    @returns    id string (const char*)
*/
const char* a_PWMdimmer::id() {
  return "PWM";
}

/*!
   @brief    PWM PWM "analog" out actuator
    @param    index     index
*/
a_PWMdimmer::a_PWMdimmer() {
  _isInitialized = false;
  _filter = defaultFilter;
}

/*!
   @brief    init PWM controller 
    @param    homieProperty    pointer to property definition
    @param    gpio             gpio port number
    @returns  (boolean) - true=success
*/
bool a_PWMdimmer::init(MyHomieProperty* homieProperty, uint8_t gpio) {
  _homieProperty = homieProperty;
  _gpio = gpio;
  myLog.setAppName(id());
  myLog.printf(LOG_INFO,F("    PWMdimmer::init id: %s GPIO%d="),_homieProperty->getDef().id, _gpio, _homieProperty->defaultValue());

  pinMode(_gpio, OUTPUT);
  analogWrite(_gpio, _filter(this, _homieProperty->defaultValue()));
  _increment=1;
  _target=0;
  _current=-1;
  _duration=0;
  _delay=1000;
  _shortestDelay=100;
  _factor = 100 / (float) _maximum;
  _activeCommand=NO_COMMAND;

  _isInitialized = true;  
  return _isInitialized;
}
/*!
   @brief    set a plugin option
    @param    option    (uint16_t) option Id
    @param    value     (int) option value
    @returns  (boolean) true if option set successfull
*/
bool a_PWMdimmer::setOption(uint16_t option, int value) {
  bool success = false;
  myLog.setAppName(id());
  switch (option) {
    case PWM_MINIMUM:
    case PWM_MAXIMUM:
      if (value>=UINT8_MAX && value<UINT16_MAX) {
        _minimum = (option == PWM_MINIMUM)? value : _minimum;
        _maximum = (option == PWM_MAXIMUM)? value : _maximum;
        _factor = 100 / (_maximum - _minimum);
        myLog.printf(LOG_INFO,F("  PWM bounds = %d-%d steps"), _minimum, _maximum);
        success = true;
      }
      break;
    case PWM_FREQUENCY:
      if (value>=100 && value<=40000) { // limits for the ESP8266
        analogWriteFreq(value);
        myLog.printf(LOG_INFO,F("  PWM frequency %dHz"), value);
        success = true;
      } else {
        myLog.printf(LOG_ERR,F("  PWM GPIO%d frequency %dhz exceeds limits 100hz-40khz"), _gpio, value);
      }
      break;
    case PWM_FADE_DELAY:
      if (value>=10 && value<1000) {
        myLog.printf(LOG_INFO,F("  PWM sample delay = %dms/step"), value);
        _shortestDelay = value;
        success = true;
      } else {
        myLog.printf(LOG_ERR,F("  PWM GPIO%d sample delay %dms/step exceeds limits 10-1000ms"), _gpio, value);
      }
      break;
    case PWM_FADE_DURATION_S:
      if (value<=UINT16_MAX/1000) {
        _duration = (uint16_t) value*1000;
        myLog.printf(LOG_INFO,F("  PWM GPIO%d fade duration %ds"), _gpio, _duration);
        success = true;
      } else {
        myLog.printf(LOG_ERR,F("  PWM GPIO%d fade duration %dms exceeds limit of %dms"), _gpio, value, UINT16_MAX);
      }
      break;
    case PWM_FADE_DURATION_MS:
      if (value<=UINT16_MAX) {
        _duration = (uint16_t) value;
        myLog.printf(LOG_INFO,F("  PWM GPIO%d fade duration %ds"), _gpio, _duration);
        success = true;
      } else {
        myLog.printf(LOG_ERR,F("  PWM GPIO%d fade duration %dms exceeds limit of %dms"), _gpio, _duration, UINT16_MAX);
      }
      break;
    case SCALE:
      _scale = value;
      myLog.printf(LOG_DEBUG,F("  PWM GPIO%d scale %.1f"), _gpio, _scale);
      pwmWrite();
      break;
    case SHIFT:
      _shift = value;
      myLog.printf(LOG_DEBUG,F("  PWM GPIO%d shift %.1f"), _gpio, _shift);
      pwmWrite();
      break;
  }
  return success;
};
/*!
   @brief    set an integer value filter
    @param filter (intValueFilter) filter callback function
    @returns    true on success;
*/
bool a_PWMdimmer::setFilter(intValueFilter filter) {
  _filter = filter;
  return true;
}
/*!
   @brief    read/update current value
    @returns    (float) current value
*/
float a_PWMdimmer::get() {
  return (float) _current * _factor;
}
/*!
   @class  a_PWMdimmer
   @brief  write a value to channel
    @param  data   pointer to PropertyData
    @returns    (boolean) true = achnkoleged new value
*/
bool a_PWMdimmer::set(PropertyData* data){
  myLog.setAppName(id());
  if (_isInitialized) {
    _scale =  data->scale;
    if (_current<0 || _duration==0) { // on initialisation or no fading = direct control
      _current=data->current / _factor;
      _target=data->current / _factor;
      pwmWrite();
      _homieProperty->sendValue();
      myLog.printf(LOG_INFO,F("  PWM GPIO%d set to %d"), _gpio, _target);
      return true;
    } else {
      _target = data->current / _factor;
      if (_target != _current) {
        float delta = _target - _current;
        if (delta < (_duration * _shortestDelay)) { // _increment = 1 with one step per X ms}
          _increment = (_target > _current) ? 1 : -1;
          _delay = _duration / fabs(delta);
        } else { // _delay = 10ms  steps per X increments
          _delay = _shortestDelay;
          _increment= delta / (_duration * _shortestDelay);
          if (_increment==0) _increment = (_target > _current) ? 1 : -1;
        }
        _timer = millis()+_delay;
        _homieProperty->sendValue((float) _current * _factor); // send current value as start point
        myLog.printf(LOG_INFO,F("  PWM GPIO%d set to %d from %d in %dms (%d per %dms)"), _gpio, _target, _current, _duration, _increment, _delay);
        _activeCommand=NO_COMMAND;
      }
      return false; // do not achnkoleged jet. Will be done by timer or threshold
    }
  }
  myLog.printf(LOG_ERR,F("PWM not initialized!"));
  return false;
}
/*!
   @brief    set a command string
    @returns    (boolean) true if everthing is OK
*/
bool a_PWMdimmer::set(const String& value) {
  String command = enumGetString(value,0);
  myLog.printf(LOG_DEBUG,F("  PWM GPIO%d received command '%s'"), _gpio, command.c_str());
  if (command.equalsIgnoreCase("tone")) {
    int frequency = enumGetString(value,1).toInt();
    if (frequency>=100 && frequency<=40000) {
        int duration = enumGetString(value,2).toInt();
        if (duration>0) {
          analogWriteFreq(frequency);
          _activeCommand = TONE;
          _timer=millis()+duration;
          analogWrite(_gpio, _filter(this, floor(_maximum/2)));
          myLog.printf(LOG_INFO,F("  PWM GPIO%d set to %dhz for %dms)"), _gpio, frequency, duration);
          return true;
        }
        myLog.printf(LOG_ERR,F("  duration %dms out of range!"),duration);
    } else {
      myLog.printf(LOG_ERR,F("  frequency %dhz out of range (100-40kHz)!"),frequency);
    }
  }
  myLog.printf(LOG_ERR,F("  command %s unknown!"),value.c_str());
  return false;
}
/*!
   @brief    check the status of the plugin / device
    @returns    (boolean) true if everthing is OK
*/
bool a_PWMdimmer::checkStatus(void) {
  // myLog.printf(LOG_DEBUG,F("    PWMdimmer::checkStatus %s"),(_isInitialized) ? "true" : "false");
  return _isInitialized;
}
/*!
   @brief    check if data is available
    @returns    true if (new) data is available
*/
bool a_PWMdimmer::available() {
  return true;
}
/*!
   @brief    get the minimum setable generic value
    @returns    (uint16_t) minimum Value
*/
uint16_t a_PWMdimmer::minValue() {
  return _minimum;
}
/*!
   @brief    get the maximum setable generic value
    @returns    (uint16_t) maximum Value
*/
uint16_t a_PWMdimmer::maxValue() {
  return _maximum;
}

/*!
   @brief    writes to analog out applient filter, scale and shift)
    @returns    (bool) true on success
*/
bool a_PWMdimmer::pwmWrite() {
  analogWrite(_gpio, _filter(this, (float)_current * _scale + _shift));
  return true;
}

/*!
   @class  a_PWMdimmer
   @brief  loop function of the PWM dimmer plugin
   @details The PWM Plugin comes with it's own loop and timer for frequent updates during dimming events. 
    If the timer is triggered it in- or decrements the `_current` value to reach the `_target`
    The limits are checked and the current value is fine tuned if the target is reached within the `_increment` range
*/
void a_PWMdimmer::loop() {
  // myLog.printf(LOG_TRACE,F(" %s loop"), id());
  if (_timer > 0 && _timer<millis()) {
    _timer = millis()+_delay;
    if (_target!=_current) {
      myLog.setAppName(id());
      if (_duration>1) {
        _current += _increment;
        if (_current < 0) _current = 0; // too low
        if (_current > _maximum) _current = _maximum; // too big
        if (fabs(_target-_current)<=_increment) { // too close
          _current = _target;
          _homieProperty->sendValue(roundTo((float) _current * _factor,1));
          myLog.printf(LOG_INFO,F("  reached target %s GPIO%d = %d"), _homieProperty->getId(), _gpio, _target);
        }
      } else {
        _homieProperty->sendValue(roundTo((float) _current * _factor,1));
        _current = _target;
      }
      myLog.printf(LOG_TRACE,F("  analogWrite %s GPIO%d=%d (%.1f%% Target %d%% Delay %dms)"), _homieProperty->getId(), _gpio, _current, (float) _current * _factor, _target, _delay);
      pwmWrite();
      _homieProperty->updateValue(roundTo((float) _current * _factor,1));
    } 
    if (_activeCommand==TONE) {
      _timer = 0;
      _activeCommand = 0;
      analogWrite(_gpio, _filter(this, 0));
    }
  }
}