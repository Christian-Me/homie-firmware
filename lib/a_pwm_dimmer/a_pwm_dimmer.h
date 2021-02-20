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
    MyHomieProperty *_homieProperty = NULL;
    uint8_t _gpio = 0;
    bool _isInitialized = false;
    float _target;
    float _current;
    float _increment;
    float _last;
    uint16_t _duration;
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
    bool init(MyHomieProperty* homieProperty, uint8_t gpio);
    bool setOption(uint16_t option, int value);    //!> set plugin option
    bool setOption(uint16_t option, float value);  //!> set float plugin option
    bool checkStatus(void);
    bool available(void);
    float get();
    void loop();
    bool set(PropertyData* data);
};

#endif