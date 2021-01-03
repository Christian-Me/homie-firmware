#ifndef S_BME280_H__
#define S_BME280_H__

#include <Arduino.h>
#include "../../include/config.h"
#include <Wire.h>

#include "Adafruit_BME280.h"
const uint8_t ADS1X15_ADDR[] = {0x76,0x77}; 

Adafruit_BME280 bme280Sensor;

class s_bme280{
    bool initialized = false;
    uint8_t sensorIndex = 0;
    uint16_t sensorInterval = 30000; // Inteval for sensor readings in seconds
    unsigned long sensorTimeout = 0;
    String output;
    rtcmem rtcMem;
    rtcBufferType rtcBuffer;
    void checkSensorStatus(void);
    void errLeds(void);
    void loadState(void);
    void updateState(void);
  public:
    s_bme280(uint8_t);
    bool available(void);
    bool read(dataPacket*);
    bool init(uint16_t delaySec);
    void loop();
};


#define STATE_SAVE_PERIOD	UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day

s_bme280::s_bme280(uint8_t index = 0) {
  sensorIndex = index;
  initialized = false;
}

bool s_bme280::init(uint16_t delaySec = 30) {
  uint8_t sensorAddress = 0;
  sensorInterval=delaySec*1000;
  sensorTimeout=millis()+sensorInterval;

  Wire.begin();
  Serial.println();
  Serial.println(F("Bosch BME280"));
  Serial.println(F("-----------------------------------------------"));

  sensorAddress=scanI2C(BME280_ADDR_PRIMARY, BME280_ADDR_SECONDARY);

  if (sensorAddress==BME280_ADDR_SECONDARY || sensorAddress==BME280_ADDR_PRIMARY) {
    Serial.print(F(" BME280 found at 0x"));
    Serial.println(sensorAddress, HEX);

    if (!bme280Sensor.begin(sensorAddress, &Wire))
    {
      Serial.println(F(" BME280 error. Please check wiring."));
      return false;
    }

    Serial.println(F(" forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,"));
    Serial.println(F(" filter off"));
    bme280Sensor.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );
                      
    // suggested rate is 1/60Hz (1m)
    // sensorInterval = 60000; // in milliseconds

    #ifdef USE_RTC
      loadState();
    #endif
    initialized = true;
  } else {
    Serial.println(F(" BME280 not found. Please check wiring."));
    initialized = false;
  }
  return initialized;
}


bool s_bme280::read(dataPacket* packet)
{
  if (sensorTimeout<millis()) {
    sensorTimeout=millis()+sensorInterval;
    bme280Sensor.takeForcedMeasurement();
    
    unsigned long time_trigger = millis();

    float temperature = bme280Sensor.readTemperature();
    float preassure = bme280Sensor.readPressure() / 100.0F;
    float humidity = bme280Sensor.readHumidity();

    #ifdef SERIAL_DEBUG
        output = F("BME280 Sensor");
        output += F("\n Timestamp: ");
        output += String(time_trigger);
        output += F("\n Temperature: ");
        output += String(temperature);
        output += F("\n Humidity: ");
        output += String(humidity);
        output += F("\n Preassure: ");
        output += String(preassure);
        Serial.println(output);
    #endif
    #ifdef USE_RTC
        updateState();
    #endif

    initDataPacket(packet);
    packet -> sensorType= SENSOR_BME280;
    packet -> sensorIndex= sensorIndex; // default

    packet -> temperature= temperature;
    packet -> pressure= preassure;
    packet -> humidity= humidity;

    // do not change
    packet -> uptime= millis()/1000;
    packet -> battery= 90;     // ToDo: replace by real battery levels
    return true;
  } 
  return false;
}


void s_bme280::checkSensorStatus(void) {
}

void s_bme280::errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

// loading calibration data from non volatile memory
// not implemented jet 
void s_bme280::loadState(void)
{
}

// save calibration data to non volatile memory
// not implemented jet 
void s_bme280::updateState(void)
{
}


void s_bme280::loop() {
  
}

#endif