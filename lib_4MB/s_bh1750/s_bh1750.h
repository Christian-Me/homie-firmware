#ifndef S_BH1750_H__
#define S_BH1750_H__

#include <Arduino.h>
#include "../../include/config.h"
#include <Wire.h>

#include "BH1750.h"
const uint8_t BH1750_ADDR[] = {0x23,0x5C}; 

class s_BH1750{
    BH1750 *sensor = nullptr;
    HomieNode *homieNode = nullptr;
    bool isInitialized = false;
    bool initalSample = true;
    uint8_t sensorIndex = 0;
    uint16_t sensorSampleInterval = 1000; // Inteval for sensor readings in seconds (1sec)
    uint16_t sensorSendInterval = 0; // Inteval for sensor readings in seconds (10min)
    float lastReadings[1] = {0};
    float scaleFactor[1] = {1};
    //    float sensorSendThreshold =  0.5;     // if sample differ by this value the sensor will send the value
    //    float oversamplingSum[3] = {0,0,0};
    //    uint8_t sensorOversampling = 0;
    //    uint8_t sensorSamples = 0;
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
  if (index>0 && index<=4) sensorIndex = index-1;
  isInitialized = false;
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
  // if (thresholdValue>0) sensorSendThreshold=thresholdValue;
  // if (oversampling>0) sensorOversampling=oversampling;
  sampleTimeout=millis()+sensorSampleInterval;
  sendTimeout=millis()+(sensorSendInterval*1000);


  Wire.begin();
  Serial.println();
  Serial.println(F(" ROHM BH1750 Light Intensity Meter (LUX)"));
  Serial.println(F("--------------------------------------------------"));

  sensorAddress=scanI2C(BH1750_ADDR[sensorIndex]);

  if (sensorAddress!=0) {
    Serial.print(F("BH1750 found at 0x"));
    Serial.println(sensorAddress, HEX);
    sensor = new BH1750(sensorAddress);
    sensor->begin(BH1750::ONE_TIME_HIGH_RES_MODE);

    homieNode = new HomieNode("BH1750", "BH1750 Light Sensor", "enviornment sensor");
    homieNode->advertise("intensity").setName("Light Intensity").setDatatype("float").setUnit("LUX").setFormat("0:65535");    
 
    
    isInitialized = true;
    initalSample = true; 
  } else {
    Serial.println(F(" BH1750 not found. Please check wiring."));
    isInitialized = false;
  }
  return isInitialized;
}
/*!
   @brief    set scale factor in case of a voltage devider or calibration
    @param    sensor    number of ADC (1..4)
    @param    scale     scaleing Factor
*/
void s_BH1750::setScaler(uint8_t sensor, float scale) {
  if (sensor>0 && sensor<=3)
  scaleFactor[sensor-1]=scale;
}

/*!
   @brief    read the senor value(s)
    @param    force     if true the sensor is read without any filters
    @returns  true if minimum one value was sent successfully
*/
bool s_BH1750::read(bool force=false)
{
  if (sampleTimeout<millis() || initalSample || force) {
    sampleTimeout=millis()+sensorSampleInterval;
    if (isInitialized) {
      float readValue;
      bool dataSent = false;
      readValue=sensor->readLightLevel(true);

      Serial.printf("Temp %2.2f\n",readValue);
      if (fabs(lastReadings[0]-readValue)>BH1750_THRESHOLD || (sensorSendInterval>0 && sendTimeout<millis()) || initalSample || force) {
        homieNode->setProperty("intensity").send(toString(readValue,4));
        Serial.print("send intensity cause:");
        Serial.print((sendTimeout<millis()) ? "time" : (fabs(lastReadings[0]-readValue)>BH1750_THRESHOLD) ? "diff" : (initalSample) ? "init" : "none");
        Serial.printf(" last:%4.2f current:%4.2f diff:%.2f\n",lastReadings[0],readValue,fabs(lastReadings[0]-readValue));
        lastReadings[0]=readValue;
        dataSent=true;
      }

      if (dataSent) sendTimeout=millis()+(sensorSendInterval*1000); // always reset send timeout after successfull reading
      initalSample = false;
      return dataSent;
    }
  } 
  return false;
}


bool s_BH1750::checkSensorStatus(void) {
  if (sensor->status != BSEC_OK) {
    if (sensorBME680.status < BSEC_OK) {
      output = "BSEC error code : " + String(sensorBME680.status);
      Serial.println(output);
    } else {
      output = "BSEC warning code : " + String(sensorBME680.status);
      Serial.println(output);
    }
    return false;
  }

  if (sensorBME680.bme680Status != BME680_OK) {
    if (sensorBME680.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(sensorBME680.bme680Status);
      Serial.println(output);
    } else {
      output = "BME680 warning code : " + String(sensorBME680.bme680Status);
      Serial.println(output);
    }
  }
  return true;
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