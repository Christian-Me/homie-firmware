#include <Arduino.h>
#include "s_DUMMY.h"
#include <Wire.h>
// #include <your_lib.h>
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
bool s_DUMMY::init(MyHomieNode* homieNode) {
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
   @brief     set an option of the blugin individually
    @param    option     option ID defined in 
    @param    value      (bool) option value
    @returns  result boolean - true=success
*/
bool s_DUMMY::setOption(uint16_t option, bool value) {
  return true;
}  
/*!
   @brief     set an option of the blugin individually
    @param    option     option ID defined in 
    @param    value      (integer) option value
    @returns  result boolean - true=success
*/
bool s_DUMMY::setOption(uint16_t option, int value) {
  return true;
}  
/*!
   @brief    check if device is initialized and/or if sensor is in operation
    @returns    true if so
*/
bool s_DUMMY::checkStatus(void) {
  return _isInitialized;
}
/*!
   @brief    check if new data is available
    @returns    true if so
*/
bool s_DUMMY::available(void) {
  return true;
}
/*!
   @brief    link into the big loop
*/
void s_DUMMY::loop() {
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_DUMMY::read()
{
  if (!_isInitialized || _homieNode==NULL) return false;
  myLog.setAppName(id());
  myLog.printf(LOG_TRACE,F("   %s read"), id());

  // sensor specific task(s)
  // initiate measurement and / or read values and store them in _lastSample[]

  return false;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_DUMMY::get(uint8_t channel) {
  float result = 0;
  if (channel < _maxDatapoints){  
    switch (channel) {
      case 0: 
        result = 1; // _sensor.read(); or _lastSample[channel];
        break;
    }
    myLog.printf(LOG_DEBUG,F("   %s measurement %s=%.1f%s"), id(), _homieNode->getProperty(channel)->getDef()->id, result, _homieNode->getProperty(channel)->getDef()->unit);
  } else {
    myLog.printf(LOG_ERR,F("   %s channel %d exceeds 0-%d"), id(), channel, _maxDatapoints-1);
  }
  return result;
}
