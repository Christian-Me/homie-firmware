#ifndef S_BH1750_H__
#define S_BH1750_H__

#include <Arduino.h>
#include <BH1750.h>
#include <plugins.h>

#define MAX_BH1750_DATAPOINTS 1
const uint8_t BH1750_ADDR[] = {0x23,0x5C,0}; // must end with a 0 value!

/**
 * @brief BME280 enviornment sensor
 */
class s_BH1750 : public Plugin {
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _address = 0;
    BH1750 *_sensor = nullptr; /**< pointer to sensor driver */ 
  public:
    const char* id ();                        /*> returns the id string */
    s_BH1750(int port);                       /*> constructor port= I2C address or port=0 autodetect */
    bool init(const MyHomieNode* homieNode);  /*> initialize the device */
    bool checkStatus(void);                   /*> check the status of the device */
    bool read(bool force);                    /*> read values from device and write to property database*/
    float get(uint8_t channel);               /*> read value from one device channel or from database */
};

#endif