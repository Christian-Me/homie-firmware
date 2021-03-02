#ifndef A_PWM_DIMMER_H__
#define A_PWM_DIMMER_H__

#include <Arduino.h>
#include "plugins.h"

/**
 * @class a_PWMdimmer a_pwm_dimmer.h 
 * @brief PWM dimmer actuator plugin
 * @details Handles one PWM output including smooth fading with an given duration amd fade steps
 * @todo logarithmic as LED for examples fades brightness on low duty cycles stringer than on higher values
 * @todo lower limits as motors for example don't spin on low duty cycles (some motors can do slow rpm if you start them with a higher value and then step down)
 */
class a_PWMdimmer : public Plugin {
    MyHomieProperty* _homieProperty = NULL;
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
       PWM_FREQUENCY = 1,               //!< PWM switching frequency
       PWM_FADE_INCREMENT = 2,          //!< Fade increment (steps/loop) (per channel)
       DIMM_SPEED = 3,                  //!< delay per loop
       PWM_FADE_DURATION = 4,           //!< Fade duration in seconds (per channel)
    };
    const char* id();
    a_PWMdimmer();
    bool init(MyHomieProperty* homieProperty, uint8_t gpio);  //!< initialize property plugin
    bool setOption(uint16_t option, int value);               //!> set plugin option
    bool setOption(uint16_t option, float value);             //!> set float plugin option
    bool checkStatus(void);                                       //!< check the status of the device returns true if initalized
    bool available();                                         //!< true if plugin / sensor data is available
    float get();                                              //!< get value
    void loop();                                              //!< main loop
    bool set(PropertyData* data);                             //!< set value for channel (complete data set)
};

#endif