#ifdef S_BH1750 // don't compile if not defined in platformio.ini

#define BH1750_DEBUG
#include <Arduino.h>
#include "s_BH1750.h"
#include <Wire.h>
#include <BH1750.h>
#include <c_homie.h>
#include "homieSyslog.h"
#include "utils.h"

/*!
   @brief    BH1750 get id string
    @returns  result id string
*/
const char* s_BH1750::id() {
  return "BH1750";
}

/*!
   @brief    BH1750 enviornment sensor constructor
    @param    port     constructor port= I2C address or port=0 autodetect
*/
s_BH1750::s_BH1750(int port) {
  _address = port;
  _isInitialized = false;
}

/*!
   @brief    init BH1750 enviornment sensor
    @param    homieNode     pointer to homie Node
    @returns  result boolean - true=success
*/
bool s_BH1750::init(MyHomieNode* homieNode) {
  uint8_t sensorAddress = 0;
  _homieNode = homieNode;
  Wire.begin();
  myLog.setAppName(id());
  myLog.print(LOG_INFO,F(" ROHM BH1750 Light Intensity Meter"));

  sensorAddress=scanI2C(BH1750_ADDR, _address);

  if (sensorAddress!=0) {
    myLog.printf(LOG_INFO,F("BH1750 found at 0x%X"),sensorAddress);
    _sensor = new BH1750(sensorAddress);
    _isInitialized = _sensor->begin(BH1750::CONTINUOUS_HIGH_RES_MODE,sensorAddress,&Wire);
    if (!_isInitialized) myLog.print(LOG_ERR,F(" BH1750 error!"));
  } else {
    myLog.print(LOG_ERR,F(" BH1750 not found. Please check wiring."));
    _isInitialized = false;
  }
  return _isInitialized;
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_BH1750::read()
{
  if (!_isInitialized || _homieNode==NULL) return false;
  return true;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float s_BH1750::get(uint8_t channel) {
  if (!_isInitialized) return 0;
  myLog.setAppName(id());
  float result = 0;
  switch (channel) {
    case CHANNEL_ILLUMINATION: 
      if (_sensor->measurementReady()) {
        result = _sensor->readLightLevel();
      } else {
        myLog.printf(LOG_NOTICE,F("   %s channel %d measurement NOT ready!"), id(), channel);
      };
      break;
    default:
      myLog.printf(LOG_ERR,F("   %s channel %d unknown!"), id(), channel);
      return result;
  }
  myLog.printf(LOG_INFO,F("   %s measurement #%d=%.2f"), id(), channel, result);
  return result;
}

/*!
   @brief    check if device is initialized
    @returns    true if so
*/
bool s_BH1750::checkStatus(void) {
  return _isInitialized;
}

#endif
