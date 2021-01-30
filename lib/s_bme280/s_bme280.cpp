#include <Arduino.h>
#include "s_bme280.h"
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
bool s_BME280::init(const MyHomieNode* homieNode) {
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
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_BME280::read(bool force=false)
{
  if (!_isInitialized || _homieNode==NULL) return false;
  myLog.setAppName(id());
  myLog.printf(LOG_TRACE,F("   %s read(%s)"), id(), force ? "true" : false);

  // sensor specific task(s)
  _sensor.takeForcedMeasurement();

  // write data to homie Property
  _homieNode->getProperty(0)->setValue(_sensor.readTemperature());
  _homieNode->getProperty(1)->setValue(_sensor.readHumidity());
  _homieNode->getProperty(2)->setValue(_sensor.readPressure() / 100.0F);

  myLog.printf(LOG_DEBUG,F("   %s measurement (%.1f,%.1f,%.1f)"), id(), _homieNode->getProperty(0)->getValue(),_homieNode->getProperty(1)->getValue(),_homieNode->getProperty(2)->getValue());
  return true;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_BME280::get(uint8_t channel) {
  return _homieNode->getProperty(channel)->getValue();
}

/*!
   @brief    check if device is initialized
    @returns    true if so
*/
bool s_BME280::checkStatus(void) {
  return _isInitialized;
}

