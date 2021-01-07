#ifndef S_BME280_H__
#define S_BME280_H__

#include <Arduino.h>
#include "../../include/config.h"
#include <Wire.h>

#include "Adafruit_BME280.h"
const uint8_t BME280_ADDR[] = {0x76,0x77}; 

class s_BME280{
    Adafruit_BME280 *sensor = nullptr;
    HomieNode *homieNode = nullptr;
    bool isInitialized = false;
    bool initalSample = true;
    uint8_t sensorIndex = 0;
    uint16_t sensorSampleInterval = 1000; // Inteval for sensor readings in seconds (1sec)
    uint16_t sensorSendInterval = 0; // Inteval for sensor readings in seconds (10min)
    float lastReadings[3] = {0,0,0};
    float scaleFactor[3] = {1,1,1};
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
    s_BME280(uint8_t);
    bool checkSensorStatus(void);
    bool available(void);
    bool read();
    bool init(uint16_t minSampleRate, uint16_t maxSampleRate);
    void setScaler(uint8_t sensor, float scale);
    void loop();
};

/*!
   @brief    ADS1115 16bit 4ch ADC
    @param    index     index of sensors ADS1115 connected (1..2)
    @returns  result String - empty string is index is out of range
*/
s_BME280::s_BME280(uint8_t index = 1) {
  if (index>0 && index<=4) sensorIndex = index-1;
  isInitialized = false;
}

/*!
   @brief    init ADS1115 16bit 4ch ADC
    @param    minSampleRate     sample Rate milli seconds to read data (0=default)
    @param    maxSampleRate     sample Rate seconds to update Data 0=only send updates (0=default)
    @returns  result boolean - true=success
*/
bool s_BME280::init(uint16_t minSampleRate=0, uint16_t maxSampleRate=0) {
  uint8_t sensorAddress = 0;
  if (minSampleRate>0) sensorSampleInterval=minSampleRate;
  if (maxSampleRate>0) sensorSendInterval=maxSampleRate;
  // if (thresholdValue>0) sensorSendThreshold=thresholdValue;
  // if (oversampling>0) sensorOversampling=oversampling;
  sampleTimeout=millis()+sensorSampleInterval;
  sendTimeout=millis()+(sensorSendInterval*1000);


  Wire.begin();
  Serial.println();
  Serial.println(F("BOSCH BME280 Temperature, Humidity and Pressure"));
  Serial.println(F("--------------------------------------------------"));

  sensorAddress=scanI2C(BME280_ADDR[sensorIndex]);

  if (sensorAddress!=0) {
    Serial.print(F("BME280 found at 0x"));
    Serial.println(sensorAddress, HEX);
    sensor = new Adafruit_BME280(sensorAddress);
    sensor->setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );

    homieNode = new HomieNode("BME280", "Bosch BME280", "enviornment sensor");
    homieNode->advertise("temperature").setName("Temperature").setDatatype("float").setUnit("C").setFormat("-40:85");    
    homieNode->advertise("humidity").setName("Humidity").setDatatype("float").setUnit("%").setFormat("0:100");    
    homieNode->advertise("pressure").setName("Pressure").setDatatype("float").setUnit("hPa").setFormat("300:1100");    
    
    isInitialized = true;
    initalSample = true; 
  } else {
    Serial.println(F(" BME280 not found. Please check wiring."));
    isInitialized = false;
  }
  return isInitialized;
}
/*!
   @brief    set scale factor in case of a voltage devider or calibration
    @param    sensor    number of ADC (1..4)
    @param    scale     scaleing Factor
*/
void s_BME280::setScaler(uint8_t sensor, float scale) {
  if (sensor>0 && sensor<=3)
  scaleFactor[sensor-1]=scale;
}

bool s_BME280::read()
{
  if (sampleTimeout<millis() || initalSample) {
    sampleTimeout=millis()+sensorSampleInterval;
    if (isInitialized) {
      float readValue;
      bool dataSent = false;
      sensor->takeForcedMeasurement();

      readValue=sensor->readTemperature();
      Serial.printf("Temp %2.2f\n",readValue);
      if (fabs(lastReadings[0]-readValue)>BME280_THRESHOLD_TEMP || (sensorSendInterval>0 && sendTimeout<millis()) || initalSample) {
        homieNode->setProperty("temperature").send(toString(readValue,4));
        Serial.print("send temperature cause:");
        Serial.print((sendTimeout<millis()) ? "time" : (fabs(lastReadings[0]-readValue)>BME280_THRESHOLD_TEMP) ? "diff" : (initalSample) ? "init" : "none");
        Serial.printf(" last:%4.2f current:%4.2f diff:%.2f\n",lastReadings[0],readValue,fabs(lastReadings[0]-readValue));
        lastReadings[0]=readValue;
        dataSent=true;
      }
   
      readValue=sensor->readHumidity()  / 100.0F;
      if (fabs(lastReadings[1]-readValue)>BME280_THRESHOLD_HUMI || (sensorSendInterval>0 && sendTimeout<millis()) || initalSample) {
        homieNode->setProperty("humidity").send(toString(readValue,4));
        Serial.print("send humidity cause:");
        Serial.print((sendTimeout<millis()) ? "time" : (fabs(lastReadings[1]-readValue)>BME280_THRESHOLD_HUMI) ? "diff" : (initalSample) ? "init" : "none");
        Serial.printf(" last:%4.2f current:%4.2f diff:%.2f\n",lastReadings[1],readValue,fabs(lastReadings[1]-readValue));
        lastReadings[1]=readValue;
        dataSent=true;
      }

      readValue=sensor->readPressure();
      if (fabs(lastReadings[2]-readValue)>BME280_THRESHOLD_PRES || (sensorSendInterval>0 && sendTimeout<millis()) || initalSample) {
        homieNode->setProperty("pressure").send(toString(readValue,4));
        Serial.print("send pressure cause:");
        Serial.print((sendTimeout<millis()) ? "time" : (fabs(lastReadings[2]-readValue)>BME280_THRESHOLD_PRES) ? "diff" : (initalSample) ? "init" : "none");
        Serial.printf(" last:%4.2f current:%4.2f diff:%.2f\n",lastReadings[2],readValue,fabs(lastReadings[2]-readValue));
        lastReadings[2]=readValue;
        dataSent=true;
      }

      if (dataSent) sendTimeout=millis()+(sensorSendInterval*1000); // always reset send timeout
      initalSample = false;
      return dataSent;
    }
  } 
  return false;
}


bool s_BME280::checkSensorStatus(void) {
  return isInitialized;
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