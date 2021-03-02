#ifndef A_GPIO_H__
#define A_GPIO_H__

#include <Arduino.h>
#include "plugins.h"

/**
 * @class a_gpio a_gpio.h 
 * @brief gpio actuator plugin
 * @details Handles one gpio output including duration 
 */
class a_GPIO : public Plugin {
    MyHomieProperty* _homieProperty = NULL;
    uint8_t _gpio = 0;
    bool _isInitialized = false;
    bool _current;
    uint16_t _duration;
    unsigned long _timer = 0;
  public:
    enum GPIO_Options {
       GPIO_DURATION = 1,           //!< duration in ms to hold the given state and the invert again
    };
    const char* id();
    a_GPIO();
    bool init(MyHomieProperty* homieProperty, uint8_t gpio);  //!< initialize property plugin
    bool checkStatus(void);                                       //!< check the status of the device returns true if initalized
    bool setOption(uint16_t option, int value);               //!> set plugin option
    float get();                                              //!< get value
    void loop();                                              //!< main loop
    bool set(PropertyData* data);                             //!< set value for channel (complete data set)
    bool set(int data);
};

#endif