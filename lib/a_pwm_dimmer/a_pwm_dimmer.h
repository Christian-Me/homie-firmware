#ifndef A_PWM_DIMMER_H__
#define A_PWM_DIMMER_H__

#include <Arduino.h>
#include "../include/config.h"
#include "../include/datatypes.h"
#include <homie.h>
#include "homieSyslog.h"
#include "c_homie.h"

class a_PWMdimmer : public Plugin {
    const MyHomieNode *_homieNode = NULL;
    bool _isInitialized = false;
  public:
    a_PWMdimmer();
    bool checkStatus(void);
    bool available(void);
    float get(uint8_t channel);
    bool init(const MyHomieNode* homieNode);
    void loop();
    bool set(uint8_t channel, PropertyData* data);
};


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
  mySysLog_setAppName("PWM");
  myLog(LOG_INFO,F("PWM Dimmer"));
  myLogf(LOG_DEBUG,F(" id   : %s"),homieNode->getDef()->id);
  myLogf(LOG_DEBUG,F(" name : %s"),homieNode->getDef()->name);

  for (int i=0; i<homieNode->getDef()->datapoints; i++) {
    myLogf(LOG_DEBUG,F(" id:%s GPIO%u"),homieNode->getDef()->dataPoint[i].id, homieNode->getDef()->dataPoint[i].gpio);
    pinMode(homieNode->getDef()->dataPoint[i].gpio, OUTPUT);
    analogWrite(homieNode->getDef()->dataPoint[i].gpio,0);
  }

  _isInitialized = true;
  myLog(LOG_INFO,F("PWM initialized sucsessfull"));
  
  mySyslog_resetAppName();
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
  mySysLog_setAppName("PWM");
  if (_isInitialized) {
    myLogf(LOG_DEBUG,F("  analogWrite #%d %.2fx%.2f=%.2f %d"), channel, data->current, data->scale, data->current*data->scale, (int) floor(data->current*data->scale*10.23));
    analogWrite(_homieNode->getDef()->dataPoint[channel].gpio,(int) floor(data->current*data->scale*10.23));
    return true;
  }
  myLogf(LOG_ERR,F("PWM not initialized!"));
  return false;
}

bool a_PWMdimmer::checkStatus(void) {
  return _isInitialized;
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
        myLogf(LOG_DEBUG,F("confirm channel #%d=%d (%s)"), i+1, currentValue[i], String((float) currentValue[i]/10.23,1).c_str());
        flag[i]=false;
      }
    }
    timeout=micros()+delay;
  }
  */
}

#endif