#include <Arduino.h>
#include "a_pwm_dimmer.h"
// #include "../include/datatypes.h"
#include <homie.h>
#include "homieSyslog.h"
#include "c_homie.h"

const char* a_PWMdimmer::id() {
  return "PWM";
}

/*!
   @brief    PWM PWM "analog" out actuator
    @param    index     index
*/
a_PWMdimmer::a_PWMdimmer() {
  _isInitialized = false;
}

/*!
   @brief    init PWM controller 
    @param    nodeDef           pointer to node definition
    @returns  result boolean - true=success
*/
bool a_PWMdimmer::init(const MyHomieNode* homieNode) {
  _homieNode = homieNode;
  myLog.setAppName(id());
  myLog.print(LOG_INFO,F("PWM Dimmer"));
  myLog.printf(LOG_DEBUG,F(" id   : %s"),homieNode->getDef()->id);
  myLog.printf(LOG_DEBUG,F(" name : %s"),homieNode->getDef()->name);

  for (int i=0; i<homieNode->getDef()->datapoints; i++) {
    myLog.printf(LOG_DEBUG,F(" id:%s GPIO%u"),homieNode->getDef()->dataPoint[i].id, homieNode->getDef()->dataPoint[i].gpio);
    pinMode(homieNode->getDef()->dataPoint[i].gpio, OUTPUT);
    analogWrite(homieNode->getDef()->dataPoint[i].gpio,0);
  }

  _isInitialized = true;
  myLog.print(LOG_INFO,F("PWM initialized sucsessfull"));
  
  myLog.resetAppName();
  return _isInitialized;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float a_PWMdimmer::get(uint8_t channel) {
  return 0;
}

/*!
   @brief  write a value to channel
    @param  channel   number of channel to write to
    @param  data      pointer to property data set
    @returns    true = success
*/
bool a_PWMdimmer::set(uint8_t channel, PropertyData* data){
  myLog.setAppName("PWM");
  if (_isInitialized) {
    myLog.printf(LOG_DEBUG,F("  analogWrite #%d %.2fx%.2f=%.2f %d"), channel, data->current, data->scale, data->current*data->scale, (int) floor(data->current*data->scale*10.23));
    analogWrite(_homieNode->getDef()->dataPoint[channel].gpio,(int) floor(data->current*data->scale*10.23));
    return true;
  }
  myLog.printf(LOG_ERR,F("PWM not initialized!"));
  return false;
}

bool a_PWMdimmer::checkStatus(void) {
  return _isInitialized;
}

bool a_PWMdimmer::available(void) {
  return true;
}

void a_PWMdimmer::loop() {
  /*
  if (micros()>timeout) {
    for (uint8_t i=0; i<_nodeDef->datapoints; i++) {
      if (targetValue[i] != currentValue[i]) {
        if (targetValue[i] > currentValue[i]) currentValue[i]++;
        if (targetValue[i] < currentValue[i]) currentValue[i]--;
        analogWrite(_nodeDef->dataPoint[i].gpio,currentValue[i]);
      } else if (flag[i]) {
        analogWrite(_nodeDef->dataPoint[i].gpio,currentValue[i]);
        homieNode->setProperty(_nodeDef->dataPoint[i].id).send(String((float) currentValue[i]/10.23,1));
        myLog.printf(LOG_DEBUG,F("confirm channel #%d=%d (%s)"), i+1, currentValue[i], String((float) currentValue[i]/10.23,1).c_str());
        flag[i]=false;
      }
    }
    timeout=micros()+delay;
  }
  */
}