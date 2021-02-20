#ifndef PLUGINS_H__
#define PLUGINS_H__

#include <Arduino.h>
#include <homie_node.hpp>


/**
 * @brief parent plugin object
 */
class Plugin {
  private:
    bool _isInitialized = false;
    uint8_t _maxDatapoints = 0;
  protected:
  public:
    virtual const char* id ();                                              //!< returns the id string
    virtual const uint8_t getDatapoints();                                  //!< returns maxDatapoints
    virtual bool init(MyHomieNode* homieNode);                              //!< initialize the device
    virtual bool checkStatus(void);                                         //!< check the status of the device

    virtual bool setOption(uint16_t option, int value);                     //!< set individual plugin option (int)
    virtual bool setOption(uint16_t option, bool value);                    //!< set individual plugin option (bool)
    virtual bool setOption(uint16_t option, float value);                   //!< set individual plugin option (float)
    virtual bool setOption(uint16_t option, uint8_t channel, float value);  //!< set individual channel option (float)

    virtual bool available(void);                                           //!< true if sensor data is available
    virtual void loop();                                                    //!< main loop
    virtual bool read();                                                    //!< read values from device
    virtual float get(uint8_t channel);                                     //!< get value for channel
    virtual bool set(uint8_t channel, float value);                         //!< set value for channel (float)
    virtual bool set(uint8_t channel, const String& value);                 //!< set value for channel (String)
    virtual bool set(uint8_t channel, PropertyData* data);                  //!< set value for channel (complete data set)
};

#endif