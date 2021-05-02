#ifdef S_BME280 // don't compile if not defined in platformio.ini

#include <Arduino.h>
#include "s_BME280.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <c_homie.h>
#include "homieSyslog.h"
#include "utils.h"

/*!
   @brief    BME280 get id string
    @returns  result id string
*/
const char* s_BME280::id() {
  return "BME280";
}

/*!
   @brief    BME280 enviornment sensor constructor
    @param    port     constructor port= I2C address or port=0 autodetect
*/
s_BME280::s_BME280(int port) {
  _address = port;
  _isInitialized = false;
}

/*!
   @brief    init BME280 enviornment sensor
    @param    homieNode     pointer to homie Node
    @returns  result boolean - true=success
*/
bool s_BME280::init(MyHomieNode* homieNode) {
  uint8_t sensorAddress = 0;
  _homieNode = homieNode;
  Wire.begin();
  myLog.setAppName(id());
  myLog.print(LOG_INFO,F("BOSCH BME280 enviornment sensor"));

  sensorAddress=scanI2C(BME280_ADDR, _address);

  if (sensorAddress!=0) {
    myLog.printf(LOG_INFO,F("BME280 found at 0x%X"),sensorAddress);
    _sensor = Adafruit_BME280();
    _sensor.begin(sensorAddress, &Wire);
    _isInitialized = true;
  } else {
    myLog.print(LOG_INFO,F(" BME280 not found. Please check wiring."));
    _isInitialized = false;
  }
  return _isInitialized;
}

/*!
   @brief    set BME280 plugin option
    @param    option    (uint16_t) option ID
    @param    value     (int) option value
    @returns  true if option set successfull
*/
bool s_BME280::setOption(uint16_t option, int value) {
  if (!_isInitialized) return false;
  myLog.printf(LOG_INFO,F(" BME280::setOption(%d,(int)%d)"),option,value);
  bool success = true;
  switch (option) {
    case MODE: 
      _mode = (Adafruit_BME280::sensor_mode)(byte) value;
      break;
    case TEMP_SAMPLING: 
      _tempSampling = (Adafruit_BME280::sensor_sampling) value;
      break;
    case HUMIDITY_SAMPLING: 
      _humiditySampling = (Adafruit_BME280::sensor_sampling) value;
      break;
    case PRESSURE_SAMPLING: 
      _pressureSampling = (Adafruit_BME280::sensor_sampling) value;
      break;
    case FILTER: 
      _filter = (Adafruit_BME280::sensor_filter) value;
      break;
    case DURATION: 
      _duration = (Adafruit_BME280::standby_duration) value;
      break;
    default:
      success = false;
  }
  if (success) {
    _sensor.setSampling(_mode,_tempSampling,_pressureSampling, _humiditySampling, _filter, _duration);
  }
  return success;
};

/*!
   @brief    set BME280 plugin option
    @param    option    (uint16_t) option ID
    @param    value     (float) option value
    @returns  true if option set successfull
*/
bool s_BME280::setOption(uint16_t option, float value) {
  if (!_isInitialized) return false;
  myLog.printf(LOG_INFO,F(" BME280::setOption(%d,(float)%.2f)"),option,value);
  bool success = true;
  switch (option) {
    case TEMP_CALIBRATION:
      _sensor.setTemperatureCompensation(value);
      break;
    default:
      success = false;
  }
  return success;
};

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_BME280::read()
{
  if (!_isInitialized || _homieNode==NULL) return false;
  myLog.setAppName(id());
  myLog.printf(LOG_TRACE,F("   %s sample"), id());

  _sensor.takeForcedMeasurement();

  return true;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_BME280::get(uint8_t channel) {
  if (!_isInitialized) return 0;
  float result = 0;
  switch (channel) {
    case CHANNEL_TEMPERATURE: 
      result = _sensor.readTemperature();
      break;
    case CHANNEL_HUMIDITY:
      result = _sensor.readHumidity();
      break;
    case CHANNEL_PRESSURE:
      result = _sensor.readPressure() / 100.0F;
      break;
    default:
      myLog.printf(LOG_TRACE,F("   %s channel %d unknown"), id(), channel);
      return false;
  }
  myLog.printf(LOG_DEBUG,F("   %s measurement #%d=%.2f"), id(), channel, result);
  return result;
}

/*!
   @brief    check if device is initialized and/or if sensor is in operation
    @returns    true if so
*/
bool s_BME280::checkStatus() {
  return _isInitialized;
}

#endif