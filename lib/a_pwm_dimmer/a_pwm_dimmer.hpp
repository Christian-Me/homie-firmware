#ifndef A_PWM_DIMMER_H__
#define A_PWM_DIMMER_H__

#include <Arduino.h>
// #include "../../include/datatypes.h"
// #include "c_homie.h"
#include "plugins.h"

/**
 * @brief PWM dimmer Plugin
 */
class a_PWMdimmer : public Plugin {
    MyHomieNode *_homieNode = NULL;
    bool _isInitialized = false;
    float _target[MAX_DATAPOINTS];
    float _current[MAX_DATAPOINTS];
    float _increment[MAX_DATAPOINTS];
    float _last[MAX_DATAPOINTS];
    uint16_t _duration[MAX_DATAPOINTS];
    uint32_t _delay;
    unsigned long _timer = 0;
  public:
    enum PWM_Options {
       PWM_FREQUENCY = 1,              // PWM switching frequency
       PWM_FADE_INCREMENT = 2,        // Fade increment (steps/loop) (per channel)
       DIMM_SPEED = 3,                 // delay per loop
       PWM_FADE_DURATION = 4,          // Fade duration in seconds (per channel)
    };
    const char* id();
    a_PWMdimmer();
    bool init(MyHomieNode* homieNode);
    bool setOption(uint16_t option, int value);                     /*> set plugin option */
    bool setOption(uint16_t option, uint8_t channel, float value);  /*> set channel plugin option */
    bool checkStatus(void);
    bool available(void);
    float get(uint8_t channel);
    void loop();
    bool set(uint8_t channel, PropertyData* data);
};

#endif