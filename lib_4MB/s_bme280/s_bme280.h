#ifndef S_BME280_H__
#define S_BME280_H__

#include <Arduino.h>
#include "../../include/config.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include "sensor.h"

#define MAX_BME280_DATAPOINTS 3
const uint8_t BME280_ADDR[] = {0x76,0x77}; 

/**
 * @brief BME280 enviornment sensor
 * 
 */
class s_BME280{
    Adafruit_BME280 *sensor = nullptr; /**< pointer to sensor driver */ 
    SensorCore core = {0,nullptr,false,true};
    SenstorData sensorData[MAX_BME280_DATAPOINTS] = {};
    uint16_t sensorSampleInterval = 1000; // Inteval for sensor readings in seconds (1sec)
    uint16_t sensorSendInterval = 0; // Inteval for sensor readings in seconds (10min)
    unsigned long sampleTimeout = 0;
    unsigned long sendTimeout = 0;
    String output;
    void loadState(void);
    void updateState(void);
  public:
    s_BME280(uint8_t);
    bool checkSensorStatus(void);
    bool available(void);
    bool read(bool force);
    bool init(uint16_t minSampleRate, uint16_t maxSampleRate);
    void setScaler(uint8_t sensor, float scale);
    void loop();
};

/*!
   @brief    BME280 enviornment sensor
    @param    index     index of sensors BME280 connected (1..2)
    @returns  result String - empty string is index is out of range
*/
s_BME280::s_BME280(uint8_t index = 1) {
  if (index>0 && index<=2) core.sensorIndex = index-1;
  core.isInitialized = false;
}

/*!
   @brief    init BME280 enviornment sensor
    @param    minSampleRate     sample Rate milli seconds to read data (0=default)
    @param    maxSampleRate     sample Rate seconds to update Data 0=only send updates (0=default)
    @returns  result boolean - true=success
*/
bool s_BME280::init(uint16_t minSampleRate=0, uint16_t maxSampleRate=0) {
  uint8_t sensorAddress = 0;
  if (minSampleRate>0) sensorSampleInterval=minSampleRate;
  if (maxSampleRate>0) sensorSendInterval=maxSampleRate;
  sampleTimeout=millis()+sensorSampleInterval;
  sendTimeout=millis()+(sensorSendInterval*1000);


  Wire.begin();
  if (syslogStarted) syslog->appName("BME280");
  myLog(LOG_INFO,F("BOSCH BME280 enviornment sensor"));

  sensorAddress=scanI2C(BME280_ADDR[core.sensorIndex]);

  if (sensorAddress!=0) {
    myLogf(LOG_INFO,F("BME280 found at 0x%X"),sensorAddress);

    sensor = new Adafruit_BME280();
    sensor->begin(sensorAddress, &Wire);

    core.homieNode = createHomieSensorNode(&BME280sensorNode);
    sensor_initDatapoints(&BME280sensorNode,sensorData);
   
    core.isInitialized = true;
  } else {
    myLog(LOG_INFO,F(" BME280 not found. Please check wiring."));
    core.isInitialized = false;
  }
  return core.isInitialized;
}
/*!
   @brief    set scale factor in case of calibration
    @param    sensor    number of sensor (1..4)
    @param    scale     scaleing Factor
*/
void s_BME280::setScaler(uint8_t sensor, float scale) {
  if (sensor>0 && sensor<=3)
  sensorData[sensor-1].scale=scale;
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_BME280::read(bool force=false)
{
  if (!core.isInitialized) return false;
  if (!normalOperation) return false;
  bool dataSent = false;

  if (sensor_sensorTimeout(sensorData, &BME280sensorNode) || force) {
    mySysLog_setAppName("BME280");
    core.force=force;

    // sensor specific task(s)
    sensor->takeForcedMeasurement();
    sensorData[0].current=sensor->readTemperature();
    sensorData[1].current=sensor->readHumidity();
    sensorData[2].current=sensor->readPressure() / 100.0F;

    // myLogf(LOG_DEBUG,F("BME280 measurement (%.1f,%.1f,%.1f)"),sensorData[0].current,sensorData[1].current,sensorData[2].current);
    for (uint8_t i = 0; i<BME280sensorNode.datapoints; i++) {
      if (sensor_datapointTimeout(i, sensorData, &BME280sensorNode) || force) {
        sensor_beforeRead(BME280sensorNode.dataPoint[i], sensorData[i]);
        if (sensor_processReading(i, sensorData, &BME280sensorNode, &core)) {
          dataSent = true;
        };
        sensor_afterRead(BME280sensorNode.dataPoint[i], sensorData[i]);
      }
    }
  }
  return dataSent;
}


bool s_BME280::checkSensorStatus(void) {
  return core.isInitialized;
}

// loading calibration data from non volatile memory
// not implemented jet 
void s_BME280::loadState(void)
{
}

// save calibration data to non volatile memory
// not implemented jet 
void s_BME280::updateState(void)
{
}


void s_BME280::loop() {
}

#endif