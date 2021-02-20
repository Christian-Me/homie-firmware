#ifndef S_DUMMY_H__
#define S_DUMMY_H__

#include <Arduino.h>
#include <plugins.h>

#define MAX_DUMMY_DATAPOINTS 1
const uint8_t DUMMY_ADDR[] = {0}; // must end with a 0 value!

/**
 * @brief DUMMY Plugin
 */
class s_DUMMY : public Plugin {
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _address = 0;
//    SENSOR_DRIVER *_sensor = nullptr; /**< pointer to sensor driver */ 
    float _lastSample[MAX_DUMMY_DATAPOINTS];
    uint8_t _maxDatapoints = MAX_DUMMY_DATAPOINTS;
  public:
    const char* id ();                                /*> returns the id string */
    s_DUMMY(int port);                                /*> constructor port= I2C address or port=0 autodetect */
    bool init(MyHomieNode* homieNode);          /*> initialize the device */
    bool checkStatus(void);                           /*> check the status of the device */
    bool setOption(uint16_t option, bool value);      /*> set individual plugin option (bool) */
    bool setOption(uint16_t option, int value);       /*> set individual plugin option (int) */
    bool available(void);                             /*> true if sensor data is available */
    void loop();                                      /*> main loop */
    bool read();                            /*> read values from device and write to property database*/
    float get(uint8_t channel);                       /*> read value from one device channel or from database */
    bool set(uint8_t channel, float value);           /*> for datatype = float */
    bool set(uint8_t channel, const String& value);   /*> for datatype = enum,string */
    bool set(uint8_t channel, PropertyData* data);    /*> complete data set */
};

#endif