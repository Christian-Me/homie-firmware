#ifndef PLUGINS_H__
#define PLUGINS_H__

#include <Arduino.h>
// #include "datatypes.h"
#include <homie_node.hpp>

// sensor Plugins
#define BH1750_ID 1
#define BME280_ID 2
#define BME680_ID 3
#define ADS1115_ID 4

#define DUMMY_ID 99

// actuator Plugins
#define PWM_ID 101
#define GPIO_ID 102

/**
 * @brief parent plugin object
 */
class Plugin {
  private:
    bool _isInitialized = false;
  protected:
  public:
    virtual const char* id ();                                /*> returns the id string */
    virtual bool init(const MyHomieNode* homieNode);          /*> initialize the device */
    virtual bool checkStatus(void);                           /*> check the status of the device */
    virtual bool available(void);                             /*> true if sensor data is available */
    virtual void loop();                                      /*> main loop */
    virtual bool read(bool force);                            /*> read values from device */
    virtual float get(uint8_t channel);                       /*> get value for channel */
    virtual bool set(uint8_t channel, float value);           /*> for datatype = float */
    virtual bool set(uint8_t channel, const String& value);   /*> for datatype = enum,string */
    virtual bool set(uint8_t channel, PropertyData* data);    /*> complete data set */
};

#endif