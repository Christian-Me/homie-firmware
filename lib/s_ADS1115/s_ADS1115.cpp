#include <Arduino.h>
#include "s_ADS1115.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <c_homie.h>
#include "homieSyslog.h"
#include "utils.h"

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
bool s_ADS1115::init(const MyHomieNode* homieNode) {
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

  // sensor specific task(s)
  float sensorValue = 0;
  for (uint8_t i = 0; i<MAX_ADS1115_DATAPOINTS; i++) {
    sensorValue = (float) _sensor.readADC_SingleEnded(i) * 0.1875;
    // write data to homie Property
    _homieNode->getProperty(i)->setValue(sensorValue);
  }
  myLog.printf(LOG_DEBUG,F("   %s measurement (%.1f,%.1f,%.1f,%.1f)"), id(), _homieNode->getProperty(0)->getValue(),_homieNode->getProperty(1)->getValue(),_homieNode->getProperty(2)->getValue(),_homieNode->getProperty(3)->getValue());
  return true;
}

/*!
   @brief    ADS1115 read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_ADS1115::get(uint8_t channel) {
  return _homieNode->getProperty(channel)->getValue();
}

/*!
   @brief    check if device is initialized
    @returns    true if so
*/
bool s_ADS1115::checkStatus(void) {
  return _isInitialized;
}

