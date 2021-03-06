#include <Arduino.h>
#include "s_DUMMY.h"
#include <Wire.h>
#include <DUMMY.h>
#include <c_homie.h>
#include "homieSyslog.h"
#include "utils.h"

/*!
   @brief    DUMMY get id string
    @returns  result id string
*/
const char* s_DUMMY::id() {
  return "DUMMY";
}

/*!
   @brief    DUMMY enviornment sensor constructor
    @param    port     constructor port= I2C address or port=0 autodetect
*/
s_DUMMY::s_DUMMY(int port) {
  _address = port;
  _isInitialized = false;
}

/*!
   @brief     init DUMMY enviornment sensor
    @param    homieNode     pointer to homie Node
    @returns  result boolean - true=success
*/
bool s_DUMMY::init(const MyHomieNode* homieNode) {
  uint8_t sensorAddress = 0;
  _homieNode = homieNode;
  Wire.begin();
  myLog.setAppName(id());
  myLog.print(LOG_INFO,F(" DUMMY Sensor device"));

  // sensorAddress=scanI2C(DUMMY_ADDR, _address);

  if (sensorAddress!=0) {
    myLog.printf(LOG_INFO,F("DUMMY found at 0x%X"),sensorAddress);
//    _sensor = new SENSOR_DRIVER(sensorAddress);
//    _isInitialized = _sensor->begin(sensorAddress,&Wire);
    if (!_isInitialized) myLog.print(LOG_ERR,F(" DUMMY error!"));
  } else {
    myLog.print(LOG_ERR,F(" DUMMY not found. Please check wiring."));
    _isInitialized = false;
  }
  return _isInitialized;
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_DUMMY::read(bool force=false)
{
  if (!_isInitialized || _homieNode==NULL) return false;
  myLog.setAppName(id());
  myLog.printf(LOG_TRACE,F("   %s read(%s)"), id(), force ? "true" : false);

  // sensor specific task(s)
  // if (_sensor->measurementReady()) {
  //   _homieNode->getProperty(0)->setValue(_sensor->readLightLevel());
  //   myLog.printf(LOG_DEBUG,F("   %s measurement (%.1f)"), id(), _homieNode->getProperty(0)->getValue());
  //   return true;
  // }

  return false;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_DUMMY::get(uint8_t channel) {
  return _homieNode->getProperty(channel)->getValue();
}

/*!
   @brief    check if device is initialized
    @returns    true if so
*/
bool s_DUMMY::checkStatus(void) {
  return _isInitialized;
}

