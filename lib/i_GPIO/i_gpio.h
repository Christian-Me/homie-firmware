#ifndef I_GPIO_H__
#define I_GPIO_H__

#include <Arduino.h>
#include "plugins.h"

/**
 * @class i_gpio i_gpio.h 
 * @brief gpio actuator plugin
 * @details Handles one gpio input
 */
class i_GPIO : public Plugin {
    MyHomieProperty* _homieProperty = NULL;
    uint8_t _gpio = 0;
    bool _isInitialized = false;
    bool _current;
    bool _lastRead;
    uint16_t _debounceDelay;
    uint8_t _mode = BUTTON_RISING;
    unsigned long _timer = 0;
  public:
    enum OPTIONS {
       DEBOUNCE_TIME = 1,           //!< debounce duration in ms
       MODE = 2,                    //!< set the gpio mode
    };
    enum MODE {
      BUTTON_RISING = 1,            //!< button send on rising edge
      BUTTON_FALLING = 2,           //!< button send on falling edge
      FREQUENCY = 3,                //!< frequency in Hz
      COUNTER_RISING = 4,           //!< counts rising edges
      COUNTER_FALLING = 5,          //!< counts falling edges
    };
    const char* id();
    i_GPIO();
    bool init(MyHomieProperty* homieProperty, uint8_t gpio);  //!< initialize property plugin
    bool checkStatus(void);                                   //!< check the status of the device returns true if initalized
    bool setOption(uint16_t option, int value);               //!> set plugin option
    float get();                                              //!< get value
    void loop();                                              //!< main loop
};

#endif