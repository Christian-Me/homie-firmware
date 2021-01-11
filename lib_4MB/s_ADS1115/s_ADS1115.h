#ifndef S_ADS1115_H__
#define S_ADS1115_H__

#include <Arduino.h>
#include "../../include/config.h"
#include <Wire.h>
#include "Adafruit_ADS1015.h"
#include "sensor.h"

#define MAX_ADS1115_DATAPOINTS 4

const uint8_t ADS1115_ADDR[] = {0x48,0x49,0x4A,0x4B}; // ADR-> GND, ADR->VDD, ADR->SDA, ADR-SCL

/**
 * @brief ADS1115 enviornment sensor
 * 
 */
class s_ADS1115{
    Adafruit_ADS1115 *sensor = nullptr; /**< pointer to sensor driver */ 
    SensorCore core = {0,nullptr,false,true};
    SenstorData sensorData[MAX_ADS1115_DATAPOINTS] = {};
    String output;
    void loadState(void);
    void updateState(void);
  public:
    s_ADS1115(uint8_t);
    bool checkSensorStatus(void);
    bool available(void);
    bool read(bool force);
    bool init(uint8_t sensorAddress);
    void setScaler(uint8_t sensor, float scale);
    void loop();
};

/*!
   @brief    ADS1115 enviornment sensor
    @param    index     index of sensors ADS1115 connected (1..2)
    @returns  result String - empty string is index is out of range
*/
s_ADS1115::s_ADS1115(uint8_t index = 1) {
  if (index>0 && index<=2) core.sensorIndex = index-1;
  core.isInitialized = false;
}

/*!
  @brief    init ADS1115 enviornment sensor
    @param  sensorAddress   i2c address of that sensor (0=autodetect)
  @returns  result boolean - true=success
*/
bool s_ADS1115::init(uint8_t sensorAddress = 0) {
  
  Wire.begin();
  if (syslogStarted) syslog->appName("ADS1115");
  myLog(LOG_INFO,F("TI ADS1X15 (4ch 16bit Analog to Digital Converter)"));

  sensorAddress=scanI2C(ADS1115_ADDR[core.sensorIndex]);

  if (sensorAddress!=0) {
    myLogf(LOG_INFO,F("ADS1115 found at 0x%X"),sensorAddress);

    sensor = new Adafruit_ADS1115(sensorAddress);
    sensor->setGain(GAIN_TWOTHIRDS);

    core.homieNode = createHomieSensorNode(&ADS1115sensorNode);
    sensor_initDatapoints(&ADS1115sensorNode,sensorData);
    
    core.isInitialized = true;
  } else {
    myLog(LOG_INFO,F(" ADS1115 not found. Please check wiring."));
    core.isInitialized = false;
  }
  return core.isInitialized;
}
/*!
   @brief    set scale factor in case of a voltage devider or calibration
    @param    sensor    number of ADC (1..4)
    @param    scale     scaleing Factor
*/
void s_ADS1115::setScaler(uint8_t sensor, float scale) {
  if (sensor>0 && sensor<=3)
  sensorData[sensor-1].scale=scale;
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_ADS1115::read(bool force=false)
{
  if (!core.isInitialized) return false;
  if (!normalOperation) return false;

  bool dataSent = false;
  core.force=force;
  for (uint8_t i = 0; i<ADS1115sensorNode.datapoints; i++) {
    if (sensor_datapointTimeout(i, sensorData, &ADS1115sensorNode) || core.force) {
      mySysLog_setAppName("ADS1115");
      // myLogf(LOG_DEBUG,F("ADS1115 measurement (oversampling #1 %d/%d)"),sensorData[0].toSample,BME280sensorNode.dataPoint[0].oversampling);

      sensor_beforeRead(ADS1115sensorNode.dataPoint[i], sensorData[i]);

      // sensor specific task(s)
      sensorData[i].current = (float) sensor->readADC_SingleEnded(i) * 0.1875 * sensorData[i].scale + sensorData[i].shift;
      
      if (sensor_processReading(i, sensorData, &ADS1115sensorNode, &core)) {
        dataSent = true;
      }
      sensor_afterRead(ADS1115sensorNode.dataPoint[i], sensorData[i]);
    }
  }
  
  return dataSent;
}


bool s_ADS1115::checkSensorStatus(void) {
  return core.isInitialized;
}

// loading calibration data from non volatile memory
// not implemented jet 
void s_ADS1115::loadState(void)
{
}

// save calibration data to non volatile memory
// not implemented jet 
void s_ADS1115::updateState(void)
{
}


void s_ADS1115::loop() {
}

#endif