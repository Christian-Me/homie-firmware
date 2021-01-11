#ifndef HOMIE_SYSLOG_H__
#define HOMIE_SYSLOG_H__

#include <ESP8266WiFi.h>
#include <Syslog.h>
#include <WiFiUdp.h>
#include <Homie.h>

HomieSetting<long> syslogProtocol("syslogProtocol", "Syslog Protocol");
HomieSetting<const char*> syslogServer("syslogServer", "Syslog Server");
HomieSetting<long> syslogPort("syslogPort", "Syslog Port");
HomieSetting<long> syslogLevel("syslogLevel", "Syslog Level");

// Default Syslog server connection info
#define SYSLOG_SERVER "192.168.2.14"
#define SYSLOG_PORT 514

#define LOG2SERIAL 1
#define LOG2SYSLOG 2
#define LOG2MQTT 4

#define LOG_TRACE 8 // trace level added to be able to filter gradua information

uint8_t logDevice = LOG2SERIAL;

const char level0[] PROGMEM = "EMERG "; // "String 0" etc are strings to store - change to suit.
const char level1[] PROGMEM = "ALERT ";
const char level2[] PROGMEM = "CRIT  ";
const char level3[] PROGMEM = "ERR   ";
const char level4[] PROGMEM = "WARN  ";
const char level5[] PROGMEM = "NOTICE";
const char level6[] PROGMEM = "INFO  ";
const char level7[] PROGMEM = "DEBUG ";
const char level8[] PROGMEM = "TRACE ";
const char *const logLevel[] PROGMEM = {level0, level1, level2, level3, level4, level5, level6, level7, level8};

WiFiUDP udpClient;

// Create a new empty syslog instance
Syslog * syslog;
bool syslogStarted = false;
uint8_t maxLogLevel = LOG_DEBUG;

void mSyslog_setup(uint8_t protocol = SYSLOG_PROTO_IETF) {

  syslogProtocol.setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0) && (candidate <= 1);
  });
  syslogServer.setDefaultValue(SYSLOG_SERVER).setValidator([] (const char *candidate) {
    return true; // should be OK ;)
  });
  syslogPort.setDefaultValue(SYSLOG_PORT).setValidator([] (long candidate) {
    return (candidate >= 1) && (candidate <= 65535);
  });
  syslogLevel.setDefaultValue(LOG_DEBUG).setValidator([] (long candidate) {
    return (candidate >= LOG_EMERG) && (candidate <= LOG_DEBUG);
  });
  syslog = new Syslog(udpClient, protocol);
}

void mySysLog_device(uint8_t device) {
  Homie.getLogger() << F("Log to device: ") << ((device & LOG2SERIAL) ? "SERIAL " : "") << ((device & LOG2SYSLOG) ? "SYSLOG " : "") << endl;
  logDevice= device;
}

bool vmylogf(uint16_t pri, const char *fmt, va_list args) {
  char *message;
  size_t initialLen;
  size_t len;
  bool result = true;

  initialLen = strlen(fmt);

  message = new char[initialLen + 1];

  len = vsnprintf(message, initialLen + 1, fmt, args);
  if (len > initialLen) {
    delete[] message;
    message = new char[len + 1];
    vsnprintf(message, len + 1, fmt, args);
  }

  #ifdef HOMIE_DISABLE_LOGGING
    Serial.printf("[%s] %s\n",logLevel[pri],message) ;
  #else
    Homie.getLogger() << "[" << logLevel[pri] << "] " << message <<  endl;
  #endif
  delete[] message;
  return result;
}

bool vmylogf(uint16_t pri, const __FlashStringHelper *fmt, va_list args) {
  char *message;
  size_t initialLen;
  size_t len;
  bool result = true;

  initialLen = strlen((const char*) fmt);

  message = new char[initialLen + 1];

  len = vsnprintf(message, initialLen + 1, (const char*) fmt, args);
  if (len > initialLen) {
    delete[] message;
    message = new char[len + 1];
    vsnprintf(message, len + 1, (const char*) fmt, args);
  }
  #ifdef HOMIE_DISABLE_LOGGING
    Serial.printf("[%s] %s\n",logLevel[pri],message) ;
  #else
    Homie.getLogger() << "[" << logLevel[pri] << "] " << message <<  endl;
  #endif
  delete[] message;
  return result;
}

bool myLogf(uint16_t pri, const char *fmt, ...) {
  va_list args;
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= maxLogLevel)) {
    va_start(args,fmt);
    result = vmylogf(pri, fmt, args);
    va_end(args);
  }
  if (logDevice & LOG2SYSLOG) {
    if (syslogStarted) {
      va_start(args,fmt);
      result = syslog->vlogf(pri,fmt, args);
      va_end(args);
    }
  }
  return result;
}

bool myLogf(uint16_t pri, const __FlashStringHelper *fmt, ...) {
  va_list args;
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= maxLogLevel)) {
    va_start(args,fmt);
    result = vmylogf(pri, fmt, args);
    va_end(args);
  }
  if (logDevice & LOG2SYSLOG) {
    if (syslogStarted) {
      va_start(args,fmt);
      result = syslog->vlogf(pri,(const char*) fmt, args);
      va_end(args);
    }
  }
  return result;
}

bool myLog(uint16_t pri, const char *fmt) {
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= maxLogLevel)) {
    #ifdef HOMIE_DISABLE_LOGGING
      Serial.printf("[%s] %s\n",logLevel[pri],fmt) ;
    #else
      Homie.getLogger() << "[" << logLevel[pri] << "] " << fmt << endl;
    #endif
    result=true;
  }
  if (logDevice & LOG2SYSLOG) {
    if (syslogStarted) {
      result = syslog->log(pri,fmt);
    }
  }
  return result;
}

bool myLog(uint16_t pri, const __FlashStringHelper *fmt) {
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= maxLogLevel)) {
    #ifdef HOMIE_DISABLE_LOGGING
      Serial.printf("[%s] %s\n",logLevel[pri],(const char*) fmt) ;
    #else
      Homie.getLogger() << "[" << logLevel[pri] << "] " << fmt << endl;
    #endif
    result=true;
  }
  if (logDevice & LOG2SYSLOG) {
    if (syslogStarted) {
      result = syslog->log(pri,fmt);
    }
  }
  return result;
}

void mSysLog_start() {

  // prepare syslog configuration here (can be anywhere before first call of 
	// log/logf method)
  syslog->server(syslogServer.get(), syslogPort.get());
  syslog->deviceHostname(Homie.getConfiguration().deviceId);
  syslog->appName(Homie.getConfiguration().name);
  syslog->defaultPriority(LOG_KERN);
  maxLogLevel =  syslogLevel.get();
  syslog->logMask(LOG_UPTO(maxLogLevel));

  syslogStarted=true;
  myLogf(LOG_DEBUG, "Syslog connected: %s:%ld Host:%s App:%s",syslogServer.get(),syslogPort.get(),Homie.getConfiguration().deviceId,Homie.getConfiguration().name);
}

bool mySysLog_setDeviceName(const char* deviceName) {
  if (syslogStarted) syslog->deviceHostname(deviceName);
  return syslogStarted;
}

bool mySysLog_setAppName(const char* appName) {
  if (syslogStarted) syslog->appName(appName);
  return syslogStarted;
}

bool mySyslog_resetAppName() {
  if (syslogStarted) syslog->appName(Homie.getConfiguration().name);
  return syslogStarted;
}
void mSyslog_loop() {
  // nothing to do here
}

#endif