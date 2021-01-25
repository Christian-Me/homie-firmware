#include "Arduino.h"
#include "plugins.h"

bool Plugin::init(const MyHomieNode* homieNode) {
  myLog(LOG_TRACE,F(" plugin didn't define init(def)!"));
  return false;
}

void Plugin::loop() {
  myLog(LOG_TRACE,F(" plugin didn't define loop()!"));
}

bool Plugin::read() {
  myLog(LOG_TRACE,F(" plugin didn't define read()!"));
  return false;
}

bool Plugin::set(uint8_t channel, float value) {
  myLog(LOG_TRACE,F(" plugin didn't define set(float value)!"));
  return false;
}

bool Plugin::set(uint8_t channel, const String& value) {
  myLog(LOG_TRACE,F(" plugin didn't define set(string value)!"));
  return false;
}

bool Plugin::set(uint8_t channel, PropertyData* data) {
  myLog(LOG_TRACE,F(" plugin didn't define set(PropertyData* data)!"));
  return false;
}

bool pluginInit(MyHomieNode* homieNode, uint8_t pluginId, const HomieNodeDef* homieDef) {
  switch (pluginId) {
    #ifdef A_PWM
      case PWM_ID: 
        homieNode->plugin= new a_PWMdimmer;
        break;
    #endif
  }
  if (homieNode->plugin != NULL) {
    homieNode->plugin->init(homieNode);
  }
  return true;
}