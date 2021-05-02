#ifdef S_HLW8012
#ifndef S_HLW8012_H__
#define S_HLW8012_H__

#include <Arduino.h>
#include <plugins.h>
#include <HLW8012.h>

#define MAX_HLW8012_DATAPOINTS 6    //!> as th number of datapoints / channels are fixed in hardware the size of the reading array can and should be fixed

class InteruptBase{
  public:
    virtual void interuptServer1() = 0;
    virtual void interuptServer2() = 0;
};


static InteruptBase *interuptInstance;

/**
 * @brief HLW8012 Plugin
*/ 
class s_HLW8012 : public Plugin, public InteruptBase{
  private:
    bool _isInitialized = false;
    const MyHomieNode *_homieNode = NULL;
    int _selPin = -1;                                 //!> select Pin
    int _cfPin = -1;                                  //!> current and voltage PWM Pin
    int _cf1Pin = -1;                                 //!> power PWM Pin
    int _selLevel = HIGH;                             //!> Set SEL_PIN to HIGH or LOW to sample current
    double _currentResistor = 1;                       //!> 1R
    double _voltageResistorUpstream = ( 4 * 470000 );    //!> 4x470K (Sonoff POW 5 x 470k)
    double _voltageResistorDownstream = 1000;            //!> 1K

    HLW8012 *_sensor = NULL;;
    float _lastSample[MAX_HLW8012_DATAPOINTS];
    uint8_t _maxDatapoints = MAX_HLW8012_DATAPOINTS;

    virtual void interuptServer1() {
      _sensor->cf_interrupt();
    }
    virtual void interuptServer2() {
      _sensor->cf1_interrupt();
    }

    static void ICACHE_RAM_ATTR ISRFunction1() {
      interuptInstance->interuptServer1();
    }
    static void ICACHE_RAM_ATTR ISRFunction2() {
      interuptInstance->interuptServer2();
    }

  public:
    enum HL8012_Options {
      SET_SEL_PIN = 1,                                //!> current / voltage select pin
      SET_CF_PIN = 2,                                 //!> current or voltage PWM
      SET_CF1_PIN = 3,                                //!> power Pin
      SET_CURRENT_RESISTOR = 4,
      SET_VOLTAGE_RESISTOR_UPSTREAM = 5,
      SET_VOLTAGE_RESISTOR_DOWNSTREAM = 6
    };
    enum HL8912_Channels {
      CHANNEL_POWER = 1,
      CHANNEL_VOLTAGE = 2,
      CHANNEL_CURRENT = 3,
      CHANNEL_APPARENT_POWER = 4,
      CHANNEL_POWER_FACTOR = 5,
      CHANNEL_ENERGY = 6,
    };
    const char* id ();                                //!< returns the id string 
    s_HLW8012(int port);                              //!< constructor port= I2C address or port=0 autodetect 
    bool init(MyHomieNode* homieNode);                //!< initialize the device 
    bool checkStatus();                               //!< check the status of the device 
    bool setOption(uint16_t option, bool value);      //!< set individual plugin option (bool) 
    bool setOption(uint16_t option, int value);       //!< set individual plugin option (int) 
    bool available();                                 //!< true if sensor data is available 
    void loop();                                      //!< main loop 
    bool read();                                      //!< read values from device and write to property database
    float get(uint8_t channel);                       //!< read value from one device channel or from database 
};

#endif
#endif