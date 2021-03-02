#ifndef S_HDC1080_H__
#define S_HDC1080_H__

#include <Arduino.h>
#include <ClosedCube_HDC1080.h>
#include <plugins.h>

#define MAX_HDC1080_DATAPOINTS 2
const uint8_t HDC1080_ADDR[] = {0x40, 0}; // sadly the HDC1080 has only one address available (array must end with a 0 value)!

/**
 * @brief HDC1080 Plugin
 */
class s_HDC1080 : public Plugin {
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _address = 0;
    ClosedCube_HDC1080 *_sensor = nullptr;
    double _lastSample[MAX_HDC1080_DATAPOINTS];
    uint8_t _maxDatapoints = MAX_HDC1080_DATAPOINTS;
    HDC1080_MeasurementResolution _humidityResolution = HDC1080_RESOLUTION_14BIT;
    HDC1080_MeasurementResolution _temperatureResolution = HDC1080_RESOLUTION_14BIT;
  public:
    /*!
        @brief  HDC1080 options
    */
    enum HDC1080_Options {
      HUMIDITY_RESOLUTION = 1,                        //!< HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_14BIT
      TEMPERATURE_RESOLUTION = 2,                     //!< HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_14BIT
      HEATER = 3,                                     //!< Heater duration in seconds
    };
    const char* id ();                                //!< returns the id string
    s_HDC1080(int port);                              //!< constructor port= I2C address or port=0 autodetect
    bool init(MyHomieNode* homieNode);                //!< initialize the device
    bool checkStatus();                               //!< check the status of the device
    bool setOption(uint16_t option, bool value);      //!< set individual plugin option (bool)
    bool setOption(uint16_t option, int value);       //!< set individual plugin option (int)
    bool available();                                 //!< true if sensor data is available
    void loop();                                      //!< main loop
    bool read();                                      //!< read values from device and write to property database*/
    float get(uint8_t channel);                       //!< read value from one device channel or from database
};

#endif