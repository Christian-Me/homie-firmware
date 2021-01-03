#ifndef s_ADS1115_H__
#define s_ADS1115_H__

#include <Arduino.h>
#include "../../include/config.h"
#include <Wire.h>
#include <Homie.h>

#include "Adafruit_ADS1015.h"

const uint8_t ADS1X15_ADDR[] = {0x48,0x49,0x4A,0x4B}; // ADR-> GND, ADR->VDD, ADR->SDA, ADR-SCL

class s_ADS1115{
    Adafruit_ADS1115 *sensor = nullptr;
    HomieNode *homieNode = nullptr;
    bool isInitialized = false;
    bool initalSample = true;
    uint8_t sensorIndex = 0;
    uint8_t sensors = 0;
    uint16_t sensorSampleInterval = 1000; // Inteval for sensor readings in seconds (1sec)
    uint16_t sensorSendInterval = 0; // Inteval for sensor readings in seconds (10min)
    float sensorSendThreshold =  0.5;     // if sample differ by this value the sensor will send the value
    float lastReadings[4] = {0,0,0,0};
    float oversamplingSum[4] = {0,0,0,0};
    float scaleFactor[4] = {1,1,1,1};
    uint8_t sensorOversampling = 0;
    uint8_t sensorSamples = 0;
    unsigned long sampleTimeout = 0;
    unsigned long sendTimeout = 0;
    String output;
    void loadState(void);
    void updateState(void);
  public:
    s_ADS1115(uint8_t,uint8_t);
    bool checkSensorStatus(void);
    bool available(void);
    bool read();
    bool init(uint16_t minSampleRate, uint16_t maxSampleRate, float thresholdValue, uint8_t oversampling);
    void setScaler(uint8_t sensor, float scale);
    void loop();
};

/*!
   @brief    ADS1115 16bit 4ch ADC
    @param    index     index of sensors ADS1115 connected (1..4)
    @param    sensors   number of ADCS to use
    @returns  result String - empty string is index is out of range
*/
s_ADS1115::s_ADS1115(uint8_t index = 1, uint8_t adcs = 4) {
  if (index>0 && index<=4) sensorIndex = index-1;
  if (adcs>0 && adcs<=4) sensors = adcs;
  isInitialized = false;
}

/*!
   @brief    init ADS1115 16bit 4ch ADC
    @param    minSampleRate     sample Rate milli seconds to read data (0=default)
    @param    maxSampleRate     sample Rate seconds to update Data 0=only send updates (0=default)
    @param    thresholdValue    threshold value. If this is exceeded data will be sent (0=default)
    @param    oversampling      number of samples to average (0=default)
    @returns  result boolean - true=success
*/
bool s_ADS1115::init(uint16_t minSampleRate=0, uint16_t maxSampleRate=0, float thresholdValue=0, uint8_t oversampling=0) {
  uint8_t sensorAddress = 0;
  if (minSampleRate>0) sensorSampleInterval=minSampleRate;
  if (maxSampleRate>0) sensorSendInterval=maxSampleRate;
  if (thresholdValue>0) sensorSendThreshold=thresholdValue;
  if (oversampling>0) sensorOversampling=oversampling;
  sampleTimeout=millis()+sensorSampleInterval;
  sendTimeout=millis()+(sensorSendInterval*1000);


  Wire.begin();
  Serial.println();
  Serial.println(F("TI ADS1X15 (4ch 16bit Analog to Digital Converter)"));
  Serial.println(F("--------------------------------------------------"));

  sensorAddress=scanI2C(ADS1X15_ADDR[sensorIndex]);

  if (sensorAddress!=0) {
    Serial.print(F("ADS1X15 found at 0x"));
    Serial.println(sensorAddress, HEX);
    sensor = new Adafruit_ADS1115(sensorAddress);
    sensor->setGain(GAIN_TWOTHIRDS);
    homieNode = new HomieNode("ADS1115", "4ch 16bit adc", "ad converter");
    for (uint8_t i = 0; i<sensors; i++) {
      string="ADC-";
      string+=i+1;
      homieNode->advertise(string.c_str()).setName("Analog Value").setDatatype("float").setUnit("mV");    
    }
    isInitialized = true;
    initalSample = true; 
  } else {
    Serial.println(F(" ADS1X15 not found. Please check wiring."));
    isInitialized = false;
  }
  return isInitialized;
}
/*!
   @brief    set scale factor in case of a voltage devider or calibration
    @param    sensor    number of ADC (1..4)
    @param    scale     scaleing Factor
*/
void s_ADS1115::setScaler(uint8_t sensor, float scale) {
  if (sensor>0 && sensor<=sensors)
  scaleFactor[sensor-1]=scale;
}

bool s_ADS1115::read()
{
  if (sampleTimeout<millis()) {
    sampleTimeout=millis()+sensorSampleInterval;
    if (isInitialized) {
      float readValue;
      bool sendValue = true;
      bool dataSent = false;
      for (uint8_t i = 0; i<sensors; i++) {
        string="ADC-";
        string+=i+1;
        readValue = (float) sensor->readADC_SingleEnded(i) * 0.1875 * scaleFactor[i];
        if (sensorOversampling>0) {
          if (sensorSamples<sensorOversampling) { // to perserve memory this is not a rolling average
            oversamplingSum[i]+=readValue;
            if (i==sensors-1) sensorSamples++;
            sendValue=false; // do not send value jet because still oversampling
          } else {
            readValue=oversamplingSum[i] / (sensorSamples-1);
            if (i==sensors-1) sensorSamples=0;
            oversamplingSum[i]=0;
          }
        }
        if (sendValue) {
          if (fabs(lastReadings[i]-readValue)>sensorSendThreshold || (sensorSendInterval>0 && sendTimeout<millis()) || initalSample) {
            homieNode->setProperty(string).send(toString(readValue,4));
            Serial.printf("send#%d cause:",i);
            Serial.print((sendTimeout<millis()) ? "time" : (fabs(lastReadings[i]-readValue)>sensorSendThreshold) ? "diff" : (initalSample) ? "init" : "none");
            Serial.printf(" last:%4.2f current:%4.2f diff:%.2f",lastReadings[i],readValue,fabs(lastReadings[i]-readValue));
            Serial.println();
            lastReadings[i]=readValue;
            dataSent=true;
          }
        }
      }   
      if (dataSent) sendTimeout=millis()+(sensorSendInterval*1000); // always reset send timeout
      initalSample = false;
      return true;
    }
  } 
  return false;
}


bool s_ADS1115::checkSensorStatus(void) {
  return isInitialized;
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