#ifndef HOMIE_SYSLOG_H__
#define HOMIE_SYSLOG_H__

#include "Arduino.h"
#include <Syslog.h>

extern bool normalOperation;

// extra log level
#define LOG_TRACE 8 // trace level added to be able to filter gradual information

// log devices
#define LOG2SERIAL 1
#define LOG2SYSLOG 2
#define LOG2MQTT 4

// default Log prints
#define LOG_MEMORY 109    // key 'm'
#define LOG_NETWORK 110   // key 'n' 
#define LOG_INFORMATION 105      // key "i"

class MyLog {
    bool _isInitialized = false;
    bool vmylogf(uint16_t pri, const char *fmt, va_list args);
    bool vmylogf(uint16_t pri, const __FlashStringHelper *fmt, va_list args);
    bool _syslogStarted = false;
    uint8_t _maxLogLevel = LOG_DEBUG;
    Syslog* _syslog;
    HardwareSerial* _serial = NULL;
    uint8_t _incomingByte = 0;
    uint32_t _lastMemory = 81920;
  public:
    void setup(HardwareSerial* serial, uint8_t protocol = SYSLOG_PROTO_BSD);
    void start();
    void device(uint8_t device);
    bool setDeviceName(const char* deviceName);
    bool setAppName(const char* appName);
    bool resetAppName();
    void printInfo(uint8_t key);
    bool print(uint16_t pri, const __FlashStringHelper *fmt);
    bool print(uint16_t pri, const char *fmt);
    bool printf(uint16_t pri, const __FlashStringHelper *fmt, ...);
    bool printf(uint16_t pri, const char *fmt, ...);
    void loop();
};

extern MyLog myLog;

#endif