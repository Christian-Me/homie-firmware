#ifndef S_DUMMY_H__
#define S_DUMMY_H__

#include <Arduino.h>
// #include <SENSOR_DRIVER.h>
#include <plugins.h>

#define MAX_DUMMY_DATAPOINTS 1
const uint8_t DUMMY_ADDR[] = {0}; // must end with a 0 value!

/**
 * @brief BME280 enviornment sensor
 */
class s_DUMMY : public Plugin {
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _address = 0;
//    SENSOR_DRIVER *_sensor = nullptr; /**< pointer to sensor driver */ 
  public:
    const char* id ();                        /*> returns the id string */
    s_DUMMY(int port);                       /*> constructor port= I2C address or port=0 autodetect */
    bool init(const MyHomieNode* homieNode);  /*> initialize the device */
    bool checkStatus(void);                   /*> check the status of the device */
    bool read(bool force);                    /*> read values from device and write to property database*/
    float get(uint8_t channel);               /*> read value from one device channel or from database */
};

#endif