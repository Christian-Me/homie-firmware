#ifdef S_HLW8012 
#include <Arduino.h>
#include "s_HLW8012.h"
#include <Wire.h>
#include <HLW8012.h>
#include <c_homie.h>
#include "homieSyslog.h"
#include "utils.h"

uint8_t _instanceNumber = 0;
/*!
   @brief    HLW8012 get id string
    @returns  result id string
*/
const char* s_HLW8012::id() {
  return "HLW8012";
}

/*!
   @brief    HLW8012 enviornment sensor constructor
    @param    port     constructor port= I2C address or port=0 autodetect
*/
s_HLW8012::s_HLW8012(int port) {
  interuptInstance = this;
  _instanceNumber++;
  _isInitialized = false;
}

/*!
   @brief     init HLW8012 enviornment sensor
    @param    homieNode     pointer to homie Node
    @returns  result boolean - true=success
*/
bool s_HLW8012::init(MyHomieNode* homieNode) {
  _homieNode = homieNode;
  myLog.setAppName(id());
  myLog.print(LOG_INFO,F(" HLW8012 Sensor device"));
  if (_instanceNumber>1) {
    myLog.print(LOG_WARNING,F("  HLW8012 can currently used only once!"));
    return false;
  }

  _sensor = new HLW8012();
  myLog.printf(LOG_NOTICE,F("  use setOption(SET_SEL_PIN / SET_SF_PIN / SET_SF1_PIN / _selLevel, PIN / HIGH or LOW)"));
  _isInitialized = false;
  return _isInitialized;
}
/*!
   @brief     set an option of the blugin individually
    @param    option     option ID defined in 
    @param    value      (bool) option value
    @returns  result boolean - true=success
*/
bool s_HLW8012::setOption(uint16_t option, bool value) {
  return true;
} 

/*!
   @brief     set an option of the blugin individually
    @param    option     option ID defined in 
    @param    value      (integer) option value
    @returns  result boolean - true=success
*/
bool s_HLW8012::setOption(uint16_t option, int value) {
  if (_instanceNumber>1) {
    myLog.print(LOG_WARNING,F("  HLW8012 can currently used only once!"));
    return false;
  }

  myLog.printf(LOG_INFO,F(" HLW8012::setOption(%d,(int)%d)"),option,value);
  bool success = true;
  switch (option) {
    case SET_SEL_PIN:
      _selPin = value;
      break;
    case SET_CF_PIN:
      _cfPin = value;
      break;
    case SET_CF1_PIN:
      _cf1Pin = value;
      break;
    case SET_CURRENT_RESISTOR:
      _currentResistor = value;
      _sensor->setResistors(_currentResistor/1000, _voltageResistorUpstream, _voltageResistorDownstream);
      break;
    case SET_VOLTAGE_RESISTOR_UPSTREAM:
      _voltageResistorUpstream = value;
      _sensor->setResistors(_currentResistor/1000, _voltageResistorUpstream, _voltageResistorDownstream);
      break;
    case SET_VOLTAGE_RESISTOR_DOWNSTREAM:
      _voltageResistorUpstream = value;
      _sensor->setResistors(_currentResistor/1000, _voltageResistorUpstream, _voltageResistorDownstream);
    default:
      success = false;
      myLog.print(LOG_ERR,F("  Option unknown!"));
  }
  if (!_isInitialized && _selPin>=0 && _cfPin>=0 && _cf1Pin>=0 && _selLevel>=0) {
    myLog.printf(LOG_DEBUG,F(" start library begin(%d,%d,%d,HIGH,true)"),_cfPin, _cf1Pin, _selPin);
    _sensor->begin(_cfPin, _cf1Pin, _selPin, HIGH, true);
    myLog.printf(LOG_DEBUG,F("   setResistors(%.1f,%.1f,%.1f)"),_currentResistor, _voltageResistorUpstream, _voltageResistorUpstream);
    _sensor->setResistors(_currentResistor, _voltageResistorUpstream, _voltageResistorUpstream);

    myLog.printf(LOG_DEBUG,F("   Default current multiplier : %.2f"), _sensor->getCurrentMultiplier());
    myLog.printf(LOG_DEBUG,F("   Default voltage multiplier : %.2f"), _sensor->getVoltageMultiplier());
    myLog.printf(LOG_DEBUG,F("   Default power multiplier   : %.2f"), _sensor->getPowerMultiplier());
    myLog.printf(LOG_DEBUG,F("  attach interupt CF"));
    attachInterrupt(_cfPin, ISRFunction1, CHANGE);
    myLog.print(LOG_DEBUG,F("  attach interupt CF1"));
    attachInterrupt(_cf1Pin, ISRFunction2, CHANGE);
    myLog.print(LOG_INFO,F(" HLW8012 sensor initialized"));
    _isInitialized = true;
  }
  return success;
}  
/*!
   @brief    check if device is initialized and/or if sensor is in operation
    @returns    true if so
*/
bool s_HLW8012::checkStatus() {
  return _isInitialized;
}
/*!
   @brief    check if new data is available
    @returns    true if so
*/
bool s_HLW8012::available() {
  return true;
}
/*!
   @brief    link into the big loop
*/
void s_HLW8012::loop() {
}

/*!
   @brief    read the senor value(s)
    @returns  true if read was successfull
*/
bool s_HLW8012::read()
{
  if (!_isInitialized || _homieNode==NULL) return false;
  return true;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_HLW8012::get(uint8_t channel) {
  float result = 0;
  myLog.printf(LOG_DEBUG,F("  %s reading channel %d"),id(),channel);
  switch (channel) {
    case CHANNEL_POWER: 
      result = _sensor->getActivePower();
      break;
    case CHANNEL_VOLTAGE: 
      result = _sensor->getVoltage();
      break;
    case CHANNEL_CURRENT: 
      result = _sensor->getCurrent();
      break;
    case CHANNEL_APPARENT_POWER: 
      result = _sensor->getApparentPower();
      break;
    case CHANNEL_POWER_FACTOR: 
      result = 100 * _sensor->getPowerFactor();
      break;
    case CHANNEL_ENERGY: 
      result = _sensor->getEnergy();
      break;
    default:
      myLog.printf(LOG_TRACE,F("   %s channel %d exceeds 1-%d"), id(), channel, _maxDatapoints);
      break;
  }
  myLog.printf(LOG_DEBUG,F("   %s measurement #%d=%.2f"), id(), channel, result);
  return result;
}
#endif