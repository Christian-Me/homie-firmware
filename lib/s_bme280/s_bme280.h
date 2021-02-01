#ifndef S_BME280_H__
#define S_BME280_H__

#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <plugins.h>

#define MAX_BME280_DATAPOINTS 3
const uint8_t BME280_ADDR[] = {0x76,0x77,0}; // must end with a 0 value!

/**
 * @brief BME280 enviornment sensor
 */
class s_BME280 : public Plugin {
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _address = 0;
    Adafruit_BME280 _sensor;
    float _lastSample[MAX_BME280_DATAPOINTS];
    uint8_t _maxDatapoints = MAX_BME280_DATAPOINTS;
  public:
    const char* id ();                        /*> returns the id string */
    s_BME280(int port);                       /*> constructor port= I2C address or port=0 autodetect */
    bool init(const MyHomieNode* homieNode);  /*> initialize the device */
    bool checkStatus(void);                   /*> check the status of the device */
    bool read(bool force);                    /*> read values from device and write to property database*/
    float get(uint8_t channel);               /*> read value from one device channel or from database */
};

#endif