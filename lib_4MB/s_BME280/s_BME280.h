#ifdef S_BME280 // don't compile if not defined in platformio.ini

#ifndef S_BME280_H__
#define S_BME280_H__

#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <plugins.h>

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
    Adafruit_BME280::sensor_mode _mode = Adafruit_BME280::MODE_FORCED;
    Adafruit_BME280::sensor_sampling _tempSampling = Adafruit_BME280::SAMPLING_X1;
    Adafruit_BME280::sensor_sampling _humiditySampling = Adafruit_BME280::SAMPLING_X1;
    Adafruit_BME280::sensor_sampling _pressureSampling = Adafruit_BME280::SAMPLING_X1;
    Adafruit_BME280::sensor_filter _filter = Adafruit_BME280::FILTER_OFF;
    Adafruit_BME280::standby_duration _duration = Adafruit_BME280::STANDBY_MS_1000;
  public:
    /*!
        @brief  BME280 options
    */
    enum BME280_Options {
      MODE = 201,              // MODE_SLEEP, MODE_FORCED, MODE_NORMAL
      TEMP_SAMPLING = 202,     // SAMPLING_NONE, SAMPLING_X1, SAMPLING_X2, SAMPLING_X4, SAMPLING_X8, SAMPLING_X16
      HUMIDITY_SAMPLING = 203, // SAMPLING_NONE, SAMPLING_X1, SAMPLING_X2, SAMPLING_X4, SAMPLING_X8, SAMPLING_X16
      PRESSURE_SAMPLING = 204, // SAMPLING_NONE, SAMPLING_X1, SAMPLING_X2, SAMPLING_X4, SAMPLING_X8, SAMPLING_X16
      FILTER = 205,            // FILTER_OFF, FILTER_X2, FILTER_X4, FILTER_X8, FILTER_X16  
      DURATION = 206,          // STANDBY_MS_0_5, STANDBY_MS_20, STANDBY_MS_62_5, STANDBY_MS_125, STANDBY_MS_250, STANDBY_MS_500, STANDBY_MS_1000
      TEMP_CALIBRATION = 207   // As the BME280 is known to have an offset by 1-2°C you can adjust your sensor  
    };
    enum BME280_Channels {
      CHANNEL_TEMPERATURE = 1,
      CHANNEL_HUMIDITY = 2,
      CHANNEL_PRESSURE = 3,
    };
    const char* id ();                                      /*> returns the id string */
    s_BME280(int port);                                     /*> constructor port= I2C address or port=0 autodetect */
    bool init(MyHomieNode* homieNode);                      /*> initialize the device */
    bool setOption(uint16_t option, int value);             /*> set individual plugin option */
    bool setOption(uint16_t option, float value);           /*> set individual plugin option */
    bool checkStatus();                                     /*> check the status of the device */
    bool read();                                            /*> read values from device and write to property database*/
    float get(uint8_t channel);                             /*> read value from one device channel or from database */
};

#endif
#endif