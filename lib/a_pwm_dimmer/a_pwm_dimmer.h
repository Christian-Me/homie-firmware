#ifndef A_PWM_DIMMER_H__
#define A_PWM_DIMMER_H__

#include <Arduino.h>
#include "../include/config.h"
#include "../include/datatypes.h"
#include <homie.h>
#include "homieSyslog.h"

#define PWM_MAX_CHANNELS 5

class a_PWMdimmer{
    HomieNode *homieNode = nullptr;
    uint8_t index = 0;
    bool isInitialized = false;
    int currentValue[PWM_MAX_CHANNELS] = {0,0,0,0,0};
    int targetValue[PWM_MAX_CHANNELS] = {0,0,0,0,0};
    bool flag[PWM_MAX_CHANNELS] = {false,false,false,false,false};
    int temp[2] = {0,0};
    int dimmer[2] = {0,0};
    unsigned long delay = 0;
    unsigned long timeout = 0;
  public:
    a_PWMdimmer(uint8_t);
    bool checkStatus(void);
    bool available(void);
    float readPWM(uint8_t channel);
    bool writePWM(uint8_t channel, float value, bool log);
    bool init();
    void loop();
    bool inputHandler(const String& property, const String& value);
};


/*!
   @brief    PWM PWM "analog" out actuator
    @param    index     index
*/
a_PWMdimmer::a_PWMdimmer(uint8_t Index = 1) {
  if (Index>0 && Index<=4) index = Index-1;
  isInitialized = false;
}

/*!
   @brief    init BME680 environmental sensor 
    @param    minSampleRate     sample Rate milli seconds to read data (0=default)
    @param    maxSampleRate     sample Rate seconds to update Data 0=only send updates (0=default)
    @returns  result boolean - true=success
*/
bool a_PWMdimmer::init() {
  mySysLog_setAppName("PWM");
  myLog(LOG_INFO,F("PWM Dimmer"));

  homieNode = createHomieActuatorNode(&PWMactuatorNode);

  for (uint8_t i=0; i<PWM_MAX_CHANNELS; i++) {
    pinMode(PWMactuatorNode.actuator[i].pin, OUTPUT);
    analogWrite(PWMactuatorNode.actuator[i].pin,0);
    currentValue[i]=0;
    targetValue[i]=0;
  }

  isInitialized = true;
  myLog(LOG_INFO,F("PWM initialized sucsessfull"));
  
  mySyslog_resetAppName();
  return isInitialized;
}

/*!
   @brief    read current value
    @param    channel   number of channel to read
    @returns    current value
*/
float a_PWMdimmer::readPWM(uint8_t channel) {
  if (channel<PWM_MAX_CHANNELS) {
    return currentValue[channel];
  }
  return 0;
}

/*!
   @brief write a value to channel
    @param  channel   number of channel to write to
    @param  value     value to write
    @returns    true = success
*/
bool a_PWMdimmer::writePWM(uint8_t channel, float value, bool log = false){
  if (isInitialized && channel<PWM_MAX_CHANNELS) {
    currentValue[channel]=floor(value*10.23);
    if (log) myLogf(LOG_DEBUG,F("analogWrite %d  to %d"), channel, floor(value*10.23));
    analogWrite(PWMactuatorNode.actuator[channel].pin,floor(value*10.23));
    return true;
  }
  myLogf(LOG_ERR,F("PWM not initialized!"));
  return false;
}

bool a_PWMdimmer::checkStatus(void) {
  return isInitialized;
}

bool a_PWMdimmer::inputHandler(const String& property, const String& value) {
  mySysLog_setAppName("PWM");
  for (uint8_t i=0; i<PWMactuatorNode.actuators; i++) {
    if (property.equals(PWMactuatorNode.actuator[i].id)) {
      if (i < PWM_MAX_CHANNELS) {
        targetValue[i]=floor(value.toFloat()*10.23);
        myLogf(LOG_INFO,F ("Set #%d : %s = %s (%d)"), i, PWMactuatorNode.actuator[i].name, value.c_str(), targetValue[i]);
        if (delay==0) currentValue[i]=targetValue[i];
        flag[i]=true;
        timeout=micros()+delay;
      } else {
        int dimmerIndex = 0;
        if (property.equals("delay")) {
          delay=value.toInt();
          myLogf(LOG_INFO,F("delay = %d"), delay);
          homieNode->setProperty(property).send(value);
        } else if (property.startsWith("temp")) {
          dimmerIndex = property.substring(property.length()-1,property.length()).toInt()-1;
          temp[dimmerIndex] = value.toInt();
          targetValue[1+(dimmerIndex*2)]=(dimmer[dimmerIndex]*10.23) * ((float) temp[dimmerIndex]/100);
          targetValue[2+(dimmerIndex*2)]=(dimmer[dimmerIndex]*10.23) * ((float) (100-temp[dimmerIndex])/100);
          myLogf(LOG_INFO,F("set #%d temp=%d (dimmer=%d LED%d=%d LED%d=%d)"), dimmerIndex, temp[dimmerIndex], dimmer[dimmerIndex], 1+(dimmerIndex*2), targetValue[1+(dimmerIndex*2)], 2+(dimmerIndex*2), targetValue[2+(dimmerIndex*2)]);
          flag[1+(dimmerIndex*2)]=true;
          flag[2+(dimmerIndex*2)]=true;
          homieNode->setProperty(property).send(value);
          timeout=micros()+delay;
        } else if (property.startsWith("dim")) {
          dimmerIndex = property.substring(property.length()-1,property.length()).toInt()-1;
          dimmer[dimmerIndex] = value.toInt();
          targetValue[1+(dimmerIndex*2)]=(dimmer[dimmerIndex]*10.23) * ((float) temp[dimmerIndex]/100);
          targetValue[2+(dimmerIndex*2)]=(dimmer[dimmerIndex]*10.23) * ((float) (100-temp[dimmerIndex])/100);
          myLogf(LOG_INFO,F("set #%d dimmer=%d (temp=%d LED%d=%d LED%d=%d)"), dimmerIndex, dimmer[dimmerIndex], temp[dimmerIndex], 1+(dimmerIndex*2), targetValue[1+(dimmerIndex*2)], 2+(dimmerIndex*2), targetValue[2+(dimmerIndex*2)]);
          flag[1+(dimmerIndex*2)]=true;
          flag[2+(dimmerIndex*2)]=true;
          homieNode->setProperty(property).send(value);
          timeout=micros()+delay;
        }
      }
    }
  }
  return true;
}

void a_PWMdimmer::loop() {
  if (micros()>timeout) {
    for (uint8_t i=0; i<PWM_MAX_CHANNELS; i++) {
      if (targetValue[i] != currentValue[i]) {
        if (targetValue[i] > currentValue[i]) currentValue[i]++;
        if (targetValue[i] < currentValue[i]) currentValue[i]--;
        analogWrite(PWMactuatorNode.actuator[i].pin,currentValue[i]);
      } else if (flag[i]) {
        analogWrite(PWMactuatorNode.actuator[i].pin,currentValue[i]);
        homieNode->setProperty(PWMactuatorNode.actuator[i].id).send(String((float) currentValue[i]/10.23,1));
        myLogf(LOG_DEBUG,F("confirm LED%d=%d (%s)"), i+1, currentValue[i], String((float) currentValue[i]/10.23,1).c_str());
        flag[i]=false;
      }
    }
    timeout=micros()+delay;
  }
}

#endif