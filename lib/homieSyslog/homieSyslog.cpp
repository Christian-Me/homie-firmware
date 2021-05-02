
#include "homieSyslog.h"
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

uint8_t logDevice = LOG2SERIAL;

const char level0[] PROGMEM = "EMERG ";
const char level1[] PROGMEM = "ALERT ";
const char level2[] PROGMEM = "CRIT  ";
const char level3[] PROGMEM = "ERR   ";
const char level4[] PROGMEM = "WARN  ";
const char level5[] PROGMEM = "NOTICE";
const char level6[] PROGMEM = "INFO  ";
const char level7[] PROGMEM = "DEBUG ";
const char level8[] PROGMEM = "TRACE ";
const char *const logLevel[] PROGMEM = {level0, level1, level2, level3, level4, level5, level6, level7, level8};

const char wifi0[] PROGMEM = "IDLE";
const char wifi1[] PROGMEM = "AP unavailable";
const char wifi2[] PROGMEM = "scan completed";
const char wifi3[] PROGMEM = "connected";
const char wifi4[] PROGMEM = "wrong password";
const char wifi5[] PROGMEM = "lost";
const char wifi6[] PROGMEM = "disconnected";
const char *const wifiStatus[] PROGMEM = {wifi0, wifi1, wifi2, wifi3, wifi4, wifi5, wifi6};
        
WiFiUDP udpClient;

void MyLog::setup(HardwareSerial* serial, uint8_t protocol) {

  _serial = serial;
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
    return (candidate >= LOG_EMERG) && (candidate <= LOG_TRACE);
  });
  _syslog = new Syslog(udpClient, protocol);
}

void MyLog::device(uint8_t device) {
  Homie.getLogger() << F("[INFO  ] Log to device: ") << ((device & LOG2SERIAL) ? "SERIAL " : "") << ((device & LOG2SYSLOG) ? "SYSLOG " : "") << endl;
  logDevice= device;
}

bool MyLog::vmylogf(uint16_t pri, const char *fmt, va_list args) {
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

  _serial->printf("[%s] %s\n",logLevel[pri],message) ;
  delete[] message;
  return result;
}

bool MyLog::vmylogf(uint16_t pri, const __FlashStringHelper *fmt, va_list args) {
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
  _serial->printf("[%s] %s\n",logLevel[pri],message) ;
  delete[] message;
  return result;
}

bool MyLog::printf(uint16_t pri, const char *fmt, ...) {
  va_list args;
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= _maxLogLevel)) {
    va_start(args,fmt);
    result = vmylogf(pri, fmt, args);
    va_end(args);
  }
  if ((logDevice & LOG2SYSLOG) && _syslogStarted && (pri <= _maxLogLevel)) {
    va_start(args,fmt);
    result = _syslog->vlogf((pri<8) ? pri : 7,fmt, args);
    va_end(args);
  }
  return result;
}

bool MyLog::printf(uint16_t pri, const __FlashStringHelper *fmt, ...) {
  va_list args;
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= _maxLogLevel)) {
    va_start(args,fmt);
    result = vmylogf(pri, fmt, args);
    va_end(args);
  }
  if ((logDevice & LOG2SYSLOG) && _syslogStarted && (pri <= _maxLogLevel)) {
    va_start(args,fmt);
    result = _syslog->vlogf((pri<8) ? pri : 7,(const char*) fmt, args);
    va_end(args);
  }
  return result;
}

bool MyLog::print(uint16_t pri, const char *fmt) {
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= _maxLogLevel)) {
    _serial->printf("[%s] %s\n",logLevel[pri],fmt) ;
    result=true;
  }
  if ((logDevice & LOG2SYSLOG) && _syslogStarted && (pri <= _maxLogLevel)) {
    result = _syslog->log((pri<8) ? pri : 7,fmt);
  }
  return result;
}

bool MyLog::print(uint16_t pri, const __FlashStringHelper *fmt) {
  bool result=false;
  if ((logDevice & LOG2SERIAL) && (pri <= _maxLogLevel)) {
    _serial->printf("[%s] %s\n",logLevel[pri],(const char*) fmt) ;
    result=true;
  }
  if ((logDevice & LOG2SYSLOG) && _syslogStarted && (pri <= _maxLogLevel)) {
    result = _syslog->log((pri<8) ? pri : 7,fmt);
  }
  return result;
}

void MyLog::start() {

  _syslog->server(syslogServer.get(), syslogPort.get());
  _syslog->deviceHostname(Homie.getConfiguration().deviceId);
  _syslog->appName(Homie.getConfiguration().name);
  _syslog->defaultPriority(LOG_KERN);
  _maxLogLevel =  syslogLevel.get();
  _syslog->logMask(LOG_UPTO(_maxLogLevel));

  _syslogStarted=true;
  printf(LOG_DEBUG, "Syslog connected: %s:%ld Host:%s App:%s",syslogServer.get(),syslogPort.get(),Homie.getConfiguration().deviceId,Homie.getConfiguration().name);
}

bool MyLog::setDeviceName(const char* deviceName) {
  if (_syslogStarted) _syslog->deviceHostname(deviceName);
  return _syslogStarted;
}

bool MyLog::setAppName(const char* appName) {
  if (_syslogStarted) _syslog->appName(appName);
  return _syslogStarted;
}

bool MyLog::resetAppName() {
  if (_syslogStarted) _syslog->appName(Homie.getConfiguration().name);
  return _syslogStarted;
}

void MyLog::printInfo(uint8_t key) {
  uint32_t freeMemory = 0;
  switch (key) {
    case LOG_MEMORY:
      freeMemory = ESP.getFreeHeap();
      printf(LOG_INFO,F("Free Memory: heap %.3fk (%d) maxBlock %.3fk fragmentation: %d%%"), (float) freeMemory/1024, freeMemory-_lastMemory, (float) ESP.getMaxFreeBlockSize()/1024, ESP.getHeapFragmentation()); 
      _lastMemory = freeMemory;
      break;
    case LOG_INFORMATION:
      printf(LOG_INFO,F("Operation: %s"),(normalOperation) ? "normal" : "idle" );
      break;
    case LOG_NETWORK:
      printf(LOG_INFO,F("Network: IP:%s/%s GW:%s MAC:%s Status: '%s'"), WiFi.localIP().toString().c_str(), WiFi.subnetMask().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.macAddress().c_str(),wifiStatus[WiFi.status()]);
      break;
    default:
      printf(LOG_ERR,F("unknown key #%d! try: 1-8,m,n"),key);
  }
}

void MyLog::loop() {
  if (_serial==NULL) return;
  if (_serial->available()) {
    _incomingByte = _serial->read();
    if (_incomingByte>48 && _incomingByte<58) {
      _maxLogLevel = _incomingByte-48;
      _syslog->logMask(LOG_UPTO((_maxLogLevel<LOG_TRACE) ? _maxLogLevel : LOG_DEBUG));
      _serial->printf("[LOGGER] max log level set to #%d %s\n",_maxLogLevel, logLevel[_maxLogLevel]);
    } else {
      printInfo(_incomingByte);
    }
  }
}

MyLog myLog;
