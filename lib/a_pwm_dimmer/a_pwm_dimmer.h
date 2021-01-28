#ifndef A_PWM_DIMMER_H__
#define A_PWM_DIMMER_H__

#include <Arduino.h>
// #include "../../include/datatypes.h"
// #include "c_homie.h"
#include "plugins.h"

class a_PWMdimmer : public Plugin {
    const MyHomieNode *_homieNode = NULL;
    bool _isInitialized = false;
  public:
    const char* id();
    a_PWMdimmer();
    bool init(const MyHomieNode* homieNode);
    bool checkStatus(void);
    bool available(void);
    float get(uint8_t channel);
    void loop();
    bool set(uint8_t channel, PropertyData* data);
};

#endif