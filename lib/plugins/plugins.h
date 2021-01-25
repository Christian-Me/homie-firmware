#ifndef PLUGINS_H__
#define PLUGINS_H__

#include "../../include/datatypes.h"
#include "homie_node.h"

// sensor Plugins
#define BH1750_ID 1
#define BME280_ID 2
#define BME680_ID 3
#define ADS1115_ID 4

// actuator Plugins
#define PWM_ID 101
#define GPIO_ID 102


class Plugin {
  protected:
    const char* id;
  public:
    virtual bool init(const MyHomieNode* homieNode);
    virtual void loop();
    virtual bool read();
    virtual bool set(uint8_t channel, float value);           /*> for datatype = float */
    virtual bool set(uint8_t channel, const String& value);   /*> for datatype = enum,string */
    virtual bool set(uint8_t channel, PropertyData* data);    /*> complete data set */
};

#ifdef A_PWM
  #include "a_pwm_dimmer.h"
#endif

bool pluginInit(MyHomieNode* homieNode, uint8_t pluginId, const HomieNodeDef* homieDef);

#endif