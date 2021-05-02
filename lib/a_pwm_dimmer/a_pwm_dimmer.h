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
    // device configuration
    uint8_t _gpio = 0;
    bool _isInitialized = false;
    uint16_t _maximum = 1023;
    uint16_t _minimum = 0;
    float _default = 0;
    // device variables
    float _factor = 100 / (_maximum - _minimum);
    int _current;
    // value modifications
    float _scale = 1;
    float _shift = 0;
    intValueFilter _filter = NULL;
    // fading variables
    int _target;
    int _increment;
    int _shortestDelay = 10;
    uint16_t _duration;
    uint32_t _delay;
    unsigned long _timer = 0;
    // commands
    uint8_t _activeCommand = 0;
    bool pwmWrite();
  public:
    enum PWM_Options {
       PWM_FREQUENCY = 1,               //!< PWM switching frequency
       PWM_MINIMUM = 2,                 //!< minimum Value
       PWM_MAXIMUM = 3,                 //!< maximum Value
       PWM_FADE_DURATION_S = 4,         //!< Fade duration in seconds
       PWM_FADE_DURATION_MS = 5,        //!< Fade duration in seconds
       PWM_FADE_DELAY = 6,              //!< shortest delay per fade step
       SCALE = 7,                       //!< set scaling factor and update output
       SHIFT = 8,                       //!< set shift value and update output
    };
    enum PWM_Commands {
      NO_COMMAND = 0,
      FADE = 1,                         //!< fading in progress
      TONE = 2,                         //!< play a tone on a buzzer or speaker. Parameters: frequency, duration
    };
    const char* id();
    a_PWMdimmer();
    uint16_t minValue();                                      //!< get the minimum generic value
    uint16_t maxValue();                                      //!< get the maximum generic value
    bool init(MyHomieProperty* homieProperty, uint8_t gpio);  //!< initialize property plugin
    bool setOption(uint16_t option, int value);               //!> set plugin option
    bool setFilter(intValueFilter filter);                    //!< set individual channel option (filter callback function)
    bool checkStatus(void);                                   //!< check the status of the device returns true if initalized
    bool available();                                         //!< true if plugin / sensor data is available
    float get();                                              //!< get value
    void loop();                                              //!< main loop
    bool set(PropertyData* data);                             //!< set value for channel (complete data set)
    bool set(const String& value);                            //!< set command string
};

#endif