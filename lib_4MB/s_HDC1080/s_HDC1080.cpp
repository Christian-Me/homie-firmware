#include <Arduino.h>
#include "s_HDC1080.h"
#include <Wire.h>
#include <ClosedCube_HDC1080.h>
#include <c_homie.h>
#include <homieSyslog.h>
#include <utils.h>

/*!
   @brief    HDC1080 get id string
    @returns  result id string
*/
const char* s_HDC1080::id() {
  return "HDC1080";
}

/*!
   @brief    HDC1080 enviornment sensor constructor
    @param    port     constructor port= I2C address or port=0 autodetect
*/
s_HDC1080::s_HDC1080(int port) {
  _address = port;
  _isInitialized = false;
  for (uint8_t i=0; i<MAX_HDC1080_DATAPOINTS; i++) {
    _lastSample[i]=0;
  }
}

/*!
   @brief     init HDC1080 enviornment sensor
    @param    homieNode     pointer to homie Node
    @returns  result boolean - true=success
*/
bool s_HDC1080::init(MyHomieNode* homieNode) {
  uint8_t sensorAddress = 0;
  _homieNode = homieNode;
  Wire.begin();
  myLog.setAppName(id());
  myLog.print(LOG_INFO,F(" TI HDC1080 14bit ±2% humidity and temperature sensor"));

  sensorAddress=scanI2C(HDC1080_ADDR, _address);

  if (sensorAddress!=0) {
    myLog.printf(LOG_INFO,F(" HDC1080 found at 0x%X"),sensorAddress);
    _sensor = new ClosedCube_HDC1080();
    _sensor->begin(sensorAddress);
    uint16_t manufacturerID = _sensor->readManufacturerId();
    if (manufacturerID==0x5449) {
      myLog.printf(LOG_INFO,F("  Manufacturer ID=0x%04x (0x5449)"),manufacturerID);
    } else {
      myLog.printf(LOG_ERR,F(" HDC1080 error. Manufacturer ID missmatch! (0x%04x!=0x5449)"),manufacturerID);
      return _isInitialized;
    }
    uint16_t deviceId = _sensor->readDeviceId();
    if (deviceId==0x1050) {
      myLog.printf(LOG_INFO,F("  Device ID=0x%04x (0x1050)"),deviceId);
    } else {
      myLog.printf(LOG_ERR,F(" HDC1080 error. Device ID missmatch! (0x%04x!=0x1050)"),deviceId);
      return _isInitialized;
    }
	  HDC1080_SerialNumber serialNumber = _sensor->readSerialNumber();
	  myLog.printf(LOG_ERR,F("  Serial Number=%02X-%04X-%04X"), serialNumber.serialFirst, serialNumber.serialMid, serialNumber.serialLast);
    _isInitialized = true;
  } else {
    myLog.print(LOG_ERR,F(" HDC1080 not found. Please check wiring."));
  }
  return _isInitialized;
}
/*!
   @brief     set an option of the plugin individually
    @param    option     option ID defined in 
    @param    value      (bool) option value
    @returns  result boolean - true=success
*/
bool s_HDC1080::setOption(uint16_t option, bool value) {
  return true;
}  
/*!
   @brief     set an option of the blugin individually
    @param    option     option ID defined in 
    @param    value      (integer) option value
    @returns  result boolean - true=success
*/
bool s_HDC1080::setOption(uint16_t option, int value) {
  if (!_isInitialized) return false;
  bool success = true;
  switch (option) {
    case HUMIDITY_RESOLUTION: 
      _humidityResolution = (HDC1080_MeasurementResolution) value;
      break;
    case TEMPERATURE_RESOLUTION: 
      _temperatureResolution = (HDC1080_MeasurementResolution) value;
      break;
    case HEATER: 
      _sensor->heatUp(value);
      break;
    default:
      success = false;
  }
  if (success) {
    _sensor->setResolution(_humidityResolution,_temperatureResolution);
  }
  return success;
}  
/*!
   @brief    check if device is initialized and/or if sensor is in operation
    @returns    true if so
*/
bool s_HDC1080::checkStatus() {
  return _isInitialized;
}
/*!
   @brief    check if new data is available
    @returns    true if so
*/
bool s_HDC1080::available() {
  return true;
}
/*!
   @brief    link into the big loop
*/
void s_HDC1080::loop() {
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_HDC1080::read()
{
  if (!_isInitialized || _homieNode==NULL) return false;
  myLog.setAppName(id());
  _lastSample[0]=_sensor->readTemperature();
  _lastSample[1]=_sensor->readHumidity();
  myLog.printf(LOG_TRACE,F("   %s read %.1f°C %.1f%"), id(), _lastSample[0], _lastSample[1]);
  return true;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_HDC1080::get(uint8_t channel) {
  if (!_isInitialized) return 0;
  float result = 0;
  if (channel < _maxDatapoints){  
    result = (float) _lastSample[channel];
    myLog.printf(LOG_DEBUG,F("   %s measurement %s=%.1f%s"), id(), _homieNode->getProperty(channel)->getDef().id, _lastSample[channel], _homieNode->getProperty(channel)->getDef().unit);
  } else {
    myLog.printf(LOG_TRACE,F("   %s channel %d exceeds 0-%d"), id(), channel, _maxDatapoints-1);
  }
  return result;
}
