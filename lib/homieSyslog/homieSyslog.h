#ifndef HOMIE_SYSLOG_H__
#define HOMIE_SYSLOG_H__

#include "Arduino.h"
#include <Syslog.h>

#define LOG_TRACE 8 // trace level added to be able to filter gradua information

#define LOG2SERIAL 1
#define LOG2SYSLOG 2
#define LOG2MQTT 4

void mSyslog_setup(uint8_t protocol = SYSLOG_PROTO_IETF);
void mSysLog_start();
void mySysLog_device(uint8_t device);
bool mySysLog_setDeviceName(const char* deviceName);
bool mySysLog_setAppName(const char* appName);
bool mySyslog_resetAppName();
bool myLog(uint16_t pri, const __FlashStringHelper *fmt);
bool myLog(uint16_t pri, const char *fmt);
bool myLogf(uint16_t pri, const __FlashStringHelper *fmt, ...);
bool myLogf(uint16_t pri, const char *fmt, ...);


#endif