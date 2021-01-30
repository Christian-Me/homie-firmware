#ifndef S_ADS1115_H__
#define S_ADS1115_H__

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include <plugins.h>

#define MAX_ADS1115_DATAPOINTS 4
const uint8_t ADS1115_ADDR[] = {0x48,0x49,0x4A,0x4B,0}; // ADR-> GND, ADR->VDD, ADR->SDA, ADR-SCL must end with a 0 value!

/**
 * @brief ADS1115 analog to digital converter
 */
class s_ADS1115 : public Plugin {
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _address = 0;
    Adafruit_ADS1115 _sensor;
  public:
    const char* id ();                        /*> returns the id string */
    s_ADS1115(int port);                      /*> constructor port= I2C address or port=0 autodetect */
    bool init(const MyHomieNode* homieNode);  /*> initialize the device */
    bool checkStatus(void);                   /*> check the status of the device */
    bool read(bool force);                    /*> read values from device and write to property database*/
    float get(uint8_t channel);               /*> read value from one device channel or from database */
};

#endif