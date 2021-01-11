#ifndef SENSOR_H__
#define SENSOR_H__

#include <Arduino.h>
#include "../include/datatypes.h"
#include "utils.h"

/*!
  @brief    calback function called before sensor will be read (i.e. switch on sensor)
    @param    dataPoint   datapoint definition
    @param    sensorData  pointer to sensor data
  @returns  true = success
*/
bool sensor_beforeRead(DataPoint dataPoint, SenstorData sensorData) {
  if (dataPoint.beforeRead!=nullptr) {
    dataPoint.beforeRead((char*) dataPoint.id, sensorData);
  }
  return true;
}

/*!
  @brief    calback function called after sensor was read (i.e. switch off sensor)
    @param    dataPoint   datapoint definition
    @param    sensorData  pointer to sensor data
  @returns  true = success
*/
bool sensor_afterRead(DataPoint dataPoint, SenstorData sensorData) {
  if (dataPoint.afterRead!=nullptr) {
    dataPoint.afterRead((char*) dataPoint.id, sensorData);
  }
  return true;
}

/*!
  @brief    calback function called after sensor was read and before data will be processed (oversampled / send)
    @param    dataPoint   datapoint definition
    @param    sensorData  pointer to sensor data
  @returns  true = success
*/
bool sensor_processValue(DataPoint dataPoint, SenstorData sensorData) {
  if (dataPoint.processValue!=nullptr) {
    dataPoint.processValue((char*) dataPoint.id, sensorData);
  }
  return true;
}

/*!
  @brief    initialize all datapints
    @param    sensorNode  pointer to sensor definition
    @param    sensorData  pointer to sensor data
  @returns  true if success
*/
bool sensor_initDatapoints(const SensorNode *sensorNode, SenstorData *sensorData) {
  for (uint8_t i=0; i<sensorNode->datapoints; i++) {
    sensorData[i].last=0;
    sensorData[i].current=0;
    sensorData[i].scale=1;
    sensorData[i].shift=0;
    sensorData[i].oversamplingSum=0;
    sensorData[i].toSample=sensorNode->dataPoint[i].oversampling;
    sensorData[i].sendTimeout= millis()+sensorNode->dataPoint[i].timeout*1000;
    sensorData[i].sampleTimeout= millis() + (sensorNode->dataPoint[i].oversampling==0) ? sensorNode->dataPoint[i].sampleRate * 1000 : sensorNode->dataPoint[i].sampleRate/sensorNode->dataPoint[i].oversampling * 1000;
    sensorData[i].initialSample= false;
  }
  return true;
}

/*!
  @brief    reset datapoint sample Timer
    @param    i           index of datapoint
    @param    sensorData  pointer to sensor data
    @param    sensorNode  pointer to sensor definition
  @returns  true if timeout reached
*/
void sensor_resetSampleTimer(uint8_t i,  SenstorData *sensorData, const SensorNode *sensorNode) {
  
  unsigned long pauseTime = (sensorNode->dataPoint[i].oversampling==0) ? 
      sensorNode->dataPoint[i].sampleRate * 1000 : 
      sensorNode->dataPoint[i].sampleRate/sensorNode->dataPoint[i].oversampling * 1000;
//  myLogf(LOG_DEBUG,F("datapoint sample timer %s in : %.1f sec"),sensorNode->dataPoint[i].id,(float) pauseTime / 1000);
  sensorData[i].sampleTimeout= millis() + pauseTime;
}

/*!
  @brief    reset datapoint send Timer
    @param    i           index of datapoint
    @param    sensorData  pointer to sensor data
    @param    sensorNode  pointer to sensor definition
  @returns  true if timeout reached
*/
void sensor_resetSendTimer(uint8_t i,  SenstorData *sensorData, const SensorNode *sensorNode) {
  
  unsigned long pauseTime = sensorNode->dataPoint[i].timeout * 1000;
  myLogf(LOG_DEBUG,F("datapoint send timer %s in : %.1f sec"),sensorNode->dataPoint[i].id,(float) pauseTime / 1000);
  sensorData[i].sendTimeout= millis() + pauseTime;
}

/*!
  @brief    check if datapoint timeout is reached
    @param    i           index of datapoint
    @param    sensorData  pointer to sensor data
    @param    sensorNode  pointer to sensor definition
  @returns  true if timeout reached
*/
bool sensor_datapointTimeout(uint8_t i,  SenstorData *sensorData, const SensorNode *sensorNode) {
  bool timeout =  false;
  if ((!sensorData[i].initialSample) || ((sensorNode->dataPoint[i].timeout>0) && (sensorData[i].sendTimeout<millis()))) {
    timeout=true;
  };
  if ((!sensorData[i].initialSample) || ((sensorNode->dataPoint[i].sampleRate>0) && (sensorData[i].sampleTimeout<millis()))) {
    timeout=true;
  }
  return timeout;
}

/*!
  @brief    check if sensor (minimum one datapoint) timeout is reached
    @param    sensorData  pointer to sensor data
    @param    sensorNode  pointer to sensor definition
  @returns  true if timeout reached
*/
bool sensor_sensorTimeout(SenstorData *sensorData, const SensorNode *sensorNode) {
  bool timeout =  false;
  for (uint8_t i=0; i<sensorNode->datapoints; i++) {
    if (sensor_datapointTimeout(i, sensorData, sensorNode)) {
      timeout = true;
    }
  }
  return timeout;
}

/*!
  @brief    process one datapoint (i) value
    @param    i           index of datapoint
    @param    sensorData  pointer to sensor data
    @param    sensorNode  pointer to sensor definition
    @param    core        pointer to sensor core
  @returns  true value was sent successfully
*/
bool sensor_processReading(uint8_t i,  SenstorData *sensorData, const SensorNode *sensorNode, SensorCore *core) {
  bool timeTrigger=false;
  bool diffTrigger=false;
  bool dataSent=false;

  sensor_processValue(sensorNode->dataPoint[i], sensorData[i]);

  // perform oversampling
  if (sensorNode->dataPoint[i].oversampling>0) {
    // myLogf(LOG_DEBUG,F("Oversampling: %d/%d %.1f-%.1f = %.1f"),sensorData[i].toSample,BME280sensorNode.dataPoint[i].oversampling, sensorData[i].last,  sensorData[i].current, fabs(sensorData[i].last-sensorData[i].current));
    if (sensorData[i].toSample>0) {
      sensor_resetSampleTimer(i, sensorData, sensorNode);
      sensorData[i].oversamplingSum += sensorData[i].current;
      --sensorData[i].toSample;
      return false;
    }
    sensorData[i].current=sensorData[i].oversamplingSum / sensorNode->dataPoint[i].oversampling;
    sensorData[i].oversamplingSum = 0;
  }

  diffTrigger=(fabs(sensorData[i].last-sensorData[i].current) > sensorNode->dataPoint[i].threshold);
  timeTrigger=(sensorNode->dataPoint[i].timeout>0 && sensorData[i].sendTimeout<millis());

  if  ( diffTrigger || timeTrigger || !sensorData[i].initialSample || core->force) {
    
    core->homieNode->setProperty(sensorNode->dataPoint[i].id).send(toString(sensorData[i].current,4));

    myLogf(LOG_INFO,F("sending: %s = %s%s (%.4f) %s"), 
      sensorNode->dataPoint[i].id, 
      toString(sensorData[i].current,4).c_str(), 
      sensorNode->dataPoint[i].unit, 
      sensorData[i].last-sensorData[i].current,
      (!sensorData[i].initialSample) ? "[init] " : (core->force) ? "[force] " : (timeTrigger) ? "[time] " : (diffTrigger) ? "[diff] " :  " "
    );

    core->force = false;
    sensorData[i].initialSample = true;
    sensorData[i].last=sensorData[i].current;
    sensor_resetSendTimer(i, sensorData, sensorNode);
    sensorData[i].sendTimeout=millis()+sensorNode->dataPoint[i].timeout*1000;
    dataSent=true;
  }
  sensor_resetSampleTimer(i, sensorData, sensorNode);
  // reset oversampling counter
  if (sensorNode->dataPoint[i].oversampling>0 && sensorData[i].toSample==0) {
    sensorData[i].toSample=sensorNode->dataPoint[i].oversampling;
  }
  mySyslog_resetAppName();

  return dataSent;
}

/*!
  @brief    read all senor datapoints value(s)
    @param    sensorData  pointer to sensor data
    @param    sensorNode  pointer to sensor definition
    @param    core        pointer to sensor core
  @returns  true if minimum one value was sent successfully
*/
bool sensor_processReadings(SenstorData *sensorData, const SensorNode *sensorNode, SensorCore *core) {
  bool dataSent=false;
  for (uint8_t i=0; i<sensorNode->datapoints; i++) {
    if (sensor_processReading(i, sensorData, sensorNode, core)) {
      dataSent = true;
    }
  }
  return dataSent;
}

#endif