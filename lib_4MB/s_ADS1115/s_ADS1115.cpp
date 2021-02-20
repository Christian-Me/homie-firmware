#include <Arduino.h>
#include "s_ADS1115.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <c_homie.h>
#include "homieSyslog.h"
#include <utils.h>

/*!
   @brief    ADS1115 get id string
    @returns  result id string
*/
const char* s_ADS1115::id() {
  return "ADS1115";
}

/*!
   @brief    ADS1115 analog to digital converter constructor
    @param    port     constructor port= I2C address or port=0 autodetect
*/
s_ADS1115::s_ADS1115(int port) {
  myLog.print(LOG_INFO,F("ADS1115 init"));
  _address = port;
  _isInitialized = false;
}

/*!
   @brief    init ADS1115 enviornment sensor
    @param    homieNode     pointer to homie Node
    @returns  result boolean - true=success
*/
bool s_ADS1115::init(MyHomieNode* homieNode) {
  uint8_t sensorAddress = 0;
  _homieNode = homieNode;
  Wire.begin();
  myLog.setAppName(id());
  myLog.print(LOG_INFO,F("TI ADS1X15 (4ch 16bit Analog to Digital Converter)"));

  sensorAddress=scanI2C(ADS1115_ADDR, _address);

  if (sensorAddress!=0) {
    myLog.printf(LOG_INFO,F("ADS1115 found at 0x%X"),sensorAddress);
    _sensor = Adafruit_ADS1115(sensorAddress);
    _sensor.begin();
    _sensor.setGain(GAIN_TWOTHIRDS);
    for (uint8_t i=0; i<_maxDatapoints; i++) { // read all datapoints once and discart to get stable state
      _sensor.readADC_SingleEnded(i);
    }
    _isInitialized = true;
  } else {
    myLog.print(LOG_INFO,F(" ADS1115 not found. Please check wiring."));
    _isInitialized = false;
  }
  return _isInitialized;
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_ADS1115::read(bool force=false)
{
  if (!_isInitialized || _homieNode==NULL) return false;
  myLog.setAppName(id());
  myLog.printf(LOG_TRACE,F("   %s read(%s)"), id(), force ? "true" : false);

  return true;
}

/*!
   @brief    ADS1115 read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_ADS1115::get(uint8_t channel) {
  float result = 0;
  if (channel < _maxDatapoints){
    result = (float) _sensor.readADC_SingleEnded(channel) * 0.1875,
    myLog.printf(LOG_DEBUG,F("   %s measurement %s=%.1f%s"), id(), _homieNode->getProperty(channel)->getDef()->id, result, _homieNode->getProperty(channel)->getDef()->unit);
  } else {
    myLog.printf(LOG_ERR,F("   %s channel %d exceeds 0-%d"), id(), channel, _maxDatapoints-1);
  }
  return result;
}

/*!
   @brief    check if device is initialized
    @returns    true if so
*/
bool s_ADS1115::checkStatus(void) {
  return _isInitialized;
}

