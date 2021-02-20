#include <Arduino.h>
#include "a_pwm_dimmer.h"
#include "a_pwm_dimmer.hpp"
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
bool a_PWMdimmer::init(MyHomieNode* homieNode) {
  _homieNode = homieNode;
  myLog.setAppName(id());
  myLog.printf(LOG_INFO,F("PWM Dimmer id: %s name: %s"),homieNode->getDef()->id,homieNode->getDef()->name);

  for (int i=0; i<homieNode->getDef()->datapoints; i++) {
    myLog.printf(LOG_DEBUG,F(" id:%s GPIO%u"),homieNode->getDef()->dataPoint[i].id, homieNode->getDef()->dataPoint[i].gpio);
    pinMode(homieNode->getDef()->dataPoint[i].gpio, OUTPUT);
    analogWrite(homieNode->getDef()->dataPoint[i].gpio,0);
    _increment[i]=1;
    _target[i]=0;
    _current[i]=-1;
    _duration[i]=0;
  }
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
        _homieNode->setCustomSampleRate(value);
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
bool a_PWMdimmer::setOption(uint16_t option, uint8_t channel, float value) {
  bool success = false;
  myLog.setAppName(id());
  switch (option) {
    case PWM_FADE_INCREMENT:
      myLog.printf(LOG_INFO,F("  PWM fade resolution #%d = %.2f"), channel, value);
      if (value>0 && value<1023) {
        _increment[channel] = value;
        success = true;
      }
      break;
    case PWM_FADE_DURATION:
      if (value>=0 && value<=3600) {
        _duration[channel] = (uint16_t) floor(value);
        myLog.printf(LOG_INFO,F("  PWM fade duration #%d = %ds"), channel, _duration[channel]);
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
float a_PWMdimmer::get(uint8_t channel) {
  return _current[channel];
}

/*!
   @class  a_PWMdimmer
   @brief  write a value to channel
    @param  channel   number of channel to write to
    @returns    true = achnkoleged new value
*/
bool a_PWMdimmer::set(uint8_t channel, PropertyData* data){
  myLog.setAppName(id());
  if (_isInitialized) {
    if (_current[channel]<0 || _duration[channel]==0) { // on initialisation or no fading = direct control
      _current[channel]=data->current;
      _target[channel]=data->current;
      analogWrite(_homieNode->getDef()->dataPoint[channel].gpio,floor(_current[channel] * 10.23));
      myLog.printf(LOG_INFO,F("  PWM #%d set to %.2f"), channel, _target[channel]);
      return true;
    } else {
      _target[channel] = data->current;
      _increment[channel]=(_target[channel] - _current[channel]) / (_duration[channel] * 1000 / _delay);
      myLog.printf(LOG_INFO,F("  PWM #%d set to %.2f from %.2f in %ds (%.2f per %dms)"), channel, _target[channel], _current[channel], _duration[channel], _increment[channel], _delay);
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
    for (uint8_t channel=0; channel<MAX_DATAPOINTS; channel++) {
      if (_target[channel]!=_current[channel]) {
        myLog.setAppName(id());
        if (_duration[channel]>1) {
          _current[channel] += _increment[channel];
          if (_current[channel] < 0) _current[channel] = 0; // too low
          if (_current[channel] > 100) _current[channel] = 100; // too big
          if (fabs(_target[channel]-_current[channel])<=_increment[channel]) { // too close
            _current[channel] = _target[channel];
          }
        } else {
          _current[channel] = _target[channel];
        }
        myLog.printf(LOG_TRACE,F("  analogWrite #%d %.2f=%d (Target %.2f Duration %ds)"), channel, _current[channel], (int) floor(_current[channel] * 10.23), _target[channel], _duration[channel]);
        analogWrite(_homieNode->getDef()->dataPoint[channel].gpio,(int) floor(_current[channel] * 10.23));
      }
    }
  }
}