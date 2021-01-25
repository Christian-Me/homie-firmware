#ifndef S_BH1750_H__
#define S_BH1750_H__

#include <Arduino.h>
#include "../../include/config.h"
#include <Wire.h>
#include "sensor.h"

#include "BH1750.h"

#define MAX_BH1750_DATAPOINTS 1

const uint8_t BH1750_ADDR[] = {0x23,0x5C}; 

/**
 * @brief BH1750 light sensor
 * 
 */
class s_BH1750{
    BH1750 *sensor = nullptr; /**< pointer to sensor driver */ 
    SensorCore core = {0,nullptr,false,true};
    PropertyData sensorData[MAX_BH1750_DATAPOINTS] = {};
    uint16_t sensorSampleInterval = 1000; // Inteval for sensor readings in seconds (1sec)
    uint16_t sensorSendInterval = 0; // Inteval for sensor readings in seconds (10min)
    unsigned long sampleTimeout = 0;
    unsigned long sendTimeout = 0;
    String output;
    void loadState(void);
    void updateState(void);
  public:
    s_BH1750(uint8_t);
    bool checkSensorStatus(void);
    bool available(void);
    bool read(bool force);
    bool init(uint16_t minSampleRate, uint16_t maxSampleRate);
    void setScaler(uint8_t sensor, float scale);
    void loop();
};

/*!
   @brief    BH1750 16bit 4ch ADC
    @param    index     index of sensors BH1750 connected (1..2)
    @returns  result String - empty string is index is out of range
*/
s_BH1750::s_BH1750(uint8_t index = 1) {
  if (index>0 && index<=4) core.sensorIndex = index-1;
  core.isInitialized = false;
}

/*!
   @brief    init BH1750 LUX Meter
    @param    minSampleRate     sample Rate milli seconds to read data (0=default)
    @param    maxSampleRate     sample Rate seconds to update Data 0=only send updates (0=default)
    @returns  result boolean - true=success
*/
bool s_BH1750::init(uint16_t minSampleRate=0, uint16_t maxSampleRate=0) {
  uint8_t sensorAddress = 0;
  if (minSampleRate>0) sensorSampleInterval=minSampleRate;
  if (maxSampleRate>0) sensorSendInterval=maxSampleRate;
  sampleTimeout=millis()+sensorSampleInterval;
  sendTimeout=millis()+(sensorSendInterval*1000);


  Wire.begin();
  if (syslogStarted) syslog->appName("BME680");
  myLog(LOG_INFO,F(" ROHM BH1750 Light Intensity Meter (LUX)"));

  sensorAddress=scanI2C(BH1750_ADDR[core.sensorIndex]);

  if (sensorAddress!=0) {
    myLogf(LOG_INFO,F("BH1750 found at 0x%X"),sensorAddress);

    sensor = new BH1750(sensorAddress);
    sensor->begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

    // core.homieNode = createHomieNode(&BH1750sensorNode);
    // sensor_initDatapoints(&BH1750sensorNode,sensorData);    

    core.isInitialized = true;
  } else {
    myLog(LOG_INFO,F(" BH1750 not found. Please check wiring."));
    core.isInitialized = false;
  }
  return core.isInitialized;
}
/*!
   @brief    set scale factor in case of a voltage devider or calibration
    @param    sensor    number of ADC (1..4)
    @param    scale     scaleing Factor
*/
void s_BH1750::setScaler(uint8_t sensor, float scale) {
  if (sensor>0 && sensor<=3)
  sensorData[sensor-1].scale=scale;
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_BH1750::read(bool force=false)
{
  if (!core.isInitialized) return false;
  if (!normalOperation) return false;
  bool dataSent = false;
  core.force=force;
  if (sensor_sensorTimeout(sensorData, &BH1750sensorNode) || force) {
    for (uint8_t i = 0; i<BH1750sensorNode.datapoints; i++) {
      if (sensor_datapointTimeout(i, sensorData, &BH1750sensorNode) || force) {
        mySysLog_setAppName("BH1750");
        sensor_beforeRead(BH1750sensorNode.dataPoint[i], sensorData[i]);
        if (sensor->measurementReady()) {
          sensorData[i].current=sensor->readLightLevel();
          if (sensor_processReading(i, sensorData, &BH1750sensorNode, &core)) {
            dataSent = true;
          };
        }
        sensor_afterRead(BH1750sensorNode.dataPoint[i], sensorData[i]);
      }
      return dataSent;
    } 
  }
  return false;
}


bool s_BH1750::checkSensorStatus(void) {
  return core.isInitialized && sensor->measurementReady();
}

// loading calibration data from non volatile memory
// not implemented jet 
void s_BH1750::loadState(void)
{
}

// save calibration data to non volatile memory
// not implemented jet 
void s_BH1750::updateState(void)
{
}


void s_BH1750::loop() {
}

#endif