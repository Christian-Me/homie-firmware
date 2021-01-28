#ifndef S_BME280_H__
#define S_BME280_H__

#include <Arduino.h>
#include <Adafruit_BME280.h>
// #include <Wire.h>
// #include "c_homie.h"
#include <plugins.h>

#define MAX_BME280_DATAPOINTS 3
const uint8_t BME280_ADDR[] = {0x76,0x77,0}; // must end with a 0 value!

/**
 * @brief BME280 enviornment sensor
 * 
 */
class s_BME280 : public Plugin {
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _address = 0;
    Adafruit_BME280 _sensor;
  public:
    const char* id ();
    s_BME280(int port);
    bool init(const MyHomieNode* homieNode);
    bool checkStatus(void);
    bool available(void);
    // void loop();
    bool read(bool force);                            /*> read values from device */
    float get(uint8_t channel);
    // bool set(uint8_t channel, PropertyData* data);
};

#endif