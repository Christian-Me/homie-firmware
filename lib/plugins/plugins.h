#ifndef PLUGINS_H__
#define PLUGINS_H__

#include <Arduino.h>
#include <homie_node.hpp>


// typedef float (*valueFilter)(float);

/**
 * @brief parent plugin object
 */
class Plugin {
  private:
    bool _isInitialized = false;
  protected:
  public:
    virtual const char* id ();                                              //!< returns the id string
    virtual bool init(MyHomieNode* homieNode);                              //!< initialize node plugin
    virtual bool init(MyHomieProperty* homieNode, uint8_t gpio);            //!< initialize property plugin
    virtual bool checkStatus(void);                                         //!< check the status of the device returns true if initalized

    virtual bool setOption(uint16_t option, int value);                     //!< set individual plugin option (int)
    virtual bool setOption(uint16_t option, bool value);                    //!< set individual plugin option (bool)
    virtual bool setOption(uint16_t option, float value);                   //!< set individual plugin option (float)
    virtual bool setOption(uint16_t option, uint8_t channel, float value);  //!< set individual channel option (float)

    virtual bool setFilter(floatValueFilter filter);                        //!< set individual channel option (filter callback function)
    virtual bool setFilter(intValueFilter filter);                          //!< set individual channel option (filter callback function)

    virtual uint16_t minValue();                                            //!< get the minimum generic value
    virtual uint16_t maxValue();                                            //!< get the maximum generic value

    virtual bool available();                                               //!< true if plugin / sensor data is available
    virtual void loop();                                                    //!< main loop
    virtual bool read();                                                    //!< read values from device
    virtual float get();                                                    //!< get value
    virtual float get(uint8_t channel);                                     //!< get value for channel
    virtual bool set(uint8_t channel, float value);                         //!< set value for channel (float)
    virtual bool set(uint8_t channel, bool value);                          //!< set value for channel (bool)
    virtual bool set(uint8_t channel, const String& value);                 //!< set value for channel (String)
    virtual bool set(uint8_t channel, PropertyData* data);                  //!< set value for channel (complete data set)
    virtual bool set(PropertyData* data);                                   //!< set value for channel (complete data set)
    virtual bool set(int data);                                             //!< set value property (integer)
    virtual bool set(const String& value);                                  //!< set value property (String)
};

#endif