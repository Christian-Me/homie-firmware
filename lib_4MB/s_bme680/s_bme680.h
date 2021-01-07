#ifndef S_BME680_H__
#define S_BME680_H__

#include <Arduino.h>
#include "../include/config.h"
#include "../include/datatypes.h"
#include <Wire.h>
#include <homie.h>
#include "homieSyslog.h"

#ifdef USE_RTC
  #include "rtcmem.h"
#endif
#include "bsec.h"

const uint8_t BME680_ADDR[] = {BME680_I2C_ADDR_PRIMARY,BME680_I2C_ADDR_SECONDARY}; 
const uint8_t bsec_config_iaq[] = {
  #include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

class s_BME680{
    Bsec *sensor = nullptr;
    HomieNode *homieNode = nullptr;
    bool isInitialized = false;
    bool initalSample = true;
    uint8_t sensorIndex = 0;
    uint16_t sensorSampleInterval = 3000; // Inteval for sensor readings in miliseconds (1sec)
    float currentReadings[MAX_BME680_DATAPOINTS] = {0,0,0,0,0};
    float lastReadings[MAX_BME680_DATAPOINTS] = {0,0,0,0,0};
    float scaleFactor[MAX_BME680_DATAPOINTS] = {1,1,1,1,1};
    unsigned long sampleTimeout = 0;
    unsigned long sensorSendTimeout[MAX_BME680_DATAPOINTS] = {0,0,0,0,0};
    String output;
    void loadState(void);
    void updateState(void);
  public:
    s_BME680(uint8_t);
    bool checkSensorStatus(void);
    bool available(void);
    bool read(bool force);
    bool init();
    void setScaler(uint8_t sensor, float scale);
    void loop();
};

int64_t GetTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

/*!
   @brief    BME680 environmental sensor 
    @param    index     index of sensors ADS1115 connected (1..2)
    @returns  result String - empty string is index is out of range
*/
s_BME680::s_BME680(uint8_t index = 1) {
  if (index>0 && index<=4) sensorIndex = index-1;
  isInitialized = false;
}

/*!
   @brief    init BME680 environmental sensor 
    @param    minSampleRate     sample Rate milli seconds to read data (0=default)
    @param    maxSampleRate     sample Rate seconds to update Data 0=only send updates (0=default)
    @returns  result boolean - true=success
*/
bool s_BME680::init() {
  uint8_t sensorAddress = 0;
  sampleTimeout=millis()+sensorSampleInterval;

  Wire.begin();
  if (syslogStarted) syslog->appName("BME680");
  myLog(LOG_INFO,F("BOSCH BME680 Air Quality, Temperature, Humidity and Pressure"));

  sensorAddress=scanI2C(BME680_ADDR[sensorIndex]);

  if (sensorAddress!=0) {
    myLogf(LOG_INFO,"BME680 found at 0x%X",sensorAddress);

    sensor = new Bsec();
    sensor->begin(sensorAddress, Wire);
    output = "BSEC library version " + String(sensor->version.major) + "." + String(sensor->version.minor) + "." + String(sensor->version.major_bugfix) + "." + String(sensor->version.minor_bugfix);
    myLog(LOG_DEBUG,output.c_str());
    checkSensorStatus();
    sensor->setConfig(bsec_config_iaq);
    checkSensorStatus();
    loadState();

    bsec_virtual_sensor_t sensorList1[3] = {
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
    };

    sensor->updateSubscription(sensorList1, 3, BSEC_SAMPLE_RATE_ULP);
    checkSensorStatus();
    
    bsec_virtual_sensor_t sensorList2[5] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    sensor->updateSubscription(sensorList2, 5, BSEC_SAMPLE_RATE_LP);
    checkSensorStatus();

    homieNode = createHomieSensorNode(&BME680sensorNode);

    for (uint8_t i=0; i<BME680sensorNode.datapoints; i++) {
      lastReadings[i]=0;
      currentReadings[i]=0;
      scaleFactor[i]=1;
      sensorSendTimeout[i]=millis()+BME680sensorNode.dataPoint[i].timeout*1000;
    }

    isInitialized = true;
    initalSample = true;
    myLog(LOG_INFO,F("BME680 initialized sucsessfull"));
  } else {
    myLog(LOG_ERR,F("BME680 not found. Please check wiring."));
    isInitialized = false;
  }
  if (syslogStarted) syslog->appName(Homie.getConfiguration().name);
  return isInitialized;
}
/*!
   @brief    set scale factor in case of a voltage devider or calibration
    @param    sensor    number of ADC (1..4)
    @param    scale     scaleing Factor
*/
void s_BME680::setScaler(uint8_t sensor, float scale) {
  if (sensor>0 && sensor<=3)
  scaleFactor[sensor-1]=scale;
}

bool s_BME680::read(bool force=false)
{
  if (sampleTimeout<millis() || initalSample || force) {
    sampleTimeout=millis()+sensorSampleInterval;
    if (isInitialized) {

      mySysLog_setAppName("BME680");
      bool dataSent = false;
      if (sensor->run()) { // If new data is available
        currentReadings[0]=sensor->temperature;
        currentReadings[1]=sensor->humidity;
        currentReadings[2]=sensor->pressure;
        currentReadings[3]=sensor->iaq;
        currentReadings[4]=sensor->iaqAccuracy;
        currentReadings[5]=sensor->staticIaq;
        currentReadings[6]=sensor->staticIaqAccuracy;

        string="";
        bool timeTrigger=false;
        bool diffTrigger=false;
        for (uint8_t i = 0; i<BME680sensorNode.datapoints; i++) {
          string += BME680sensorNode.dataPoint[i].id;
          string += ":";
          string += currentReadings[i];
          string += BME680sensorNode.dataPoint[i].unit;
          
          diffTrigger=(fabs(lastReadings[i]-currentReadings[i])>BME680sensorNode.dataPoint[i].threshold);
          timeTrigger=(BME680sensorNode.dataPoint[i].timeout>0 && sensorSendTimeout[i]<millis());

          if  ( diffTrigger || timeTrigger || initalSample || force) {
            homieNode->setProperty(BME680sensorNode.dataPoint[i].id).send(toString(currentReadings[i],4));
            string += (timeTrigger) ? "[time] " : (diffTrigger) ? "[diff] " : (initalSample) ? "[init] " : " ";
            lastReadings[i]=currentReadings[i];
            sensorSendTimeout[i]=millis()+BME680sensorNode.dataPoint[i].timeout*1000;
            dataSent=true;
          } else {
            string += " ";
          }
        }
        myLog(LOG_DEBUG,string.c_str());
      } else {
        checkSensorStatus();
      }
      initalSample = false;
      mySyslog_resetAppName();
      return dataSent;
    }
  }
  return false;
}


bool s_BME680::checkSensorStatus(void) {
  if (!isInitialized) return isInitialized;
  if (sensor->status != BSEC_OK) {
    if (sensor->status < BSEC_OK) {
      myLogf(LOG_ERR,F("BSEC error code : %d"),sensor->status);
    } else {
      myLogf(LOG_ERR,F("BSEC warning code : %d"),sensor->status);
    }
    return false;
  }

  if (sensor->bme680Status != BME680_OK) {
    if (sensor->bme680Status < BME680_OK) {
      myLogf(LOG_ERR,F("BME680 error code : %d"),sensor->bme680Status);
    } else {
      myLogf(LOG_ERR,F("BME680 warning code : %d"),sensor->bme680Status);
    }
  }
  return true;
}

// loading calibration data from non volatile memory
// not implemented jet 
void s_BME680::loadState(void)
{
}

// save calibration data to non volatile memory
// not implemented jet 
void s_BME680::updateState(void)
{
}


void s_BME680::loop() {
  
}

#endif