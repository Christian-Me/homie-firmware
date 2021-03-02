#include <Arduino.h>
#include <plugins.h>

const char* Plugin::id() {
  return "default";
}

const uint8_t Plugin::getDatapoints() {
  return _maxDatapoints;
};

bool Plugin::init(MyHomieNode* homieNode) {
  _isInitialized = true;
  myLog.print(LOG_NOTICE,F("   plugin didn't define init(homieNode)!"));
  return false;
}

bool Plugin::init(MyHomieProperty* homieNode, uint8_t gpio) {
  _isInitialized = true;
  myLog.print(LOG_NOTICE,F("   plugin didn't define init(homieNode, gpio)!"));
  return false;
}

bool Plugin::setOption(uint16_t option, bool value) {
  myLog.print(LOG_NOTICE,F("   plugin didn't define setOption(option, (bool) value)!"));
  return false;
}  

bool Plugin::setOption(uint16_t option, int value) {
  myLog.print(LOG_NOTICE,F("   plugin didn't define setOption(option, (int) value)!"));
  return false;
}  

bool Plugin::setOption(uint16_t option, float value) {
  myLog.print(LOG_NOTICE,F("   plugin didn't define setOption(option, (float) value)!"));
  return false;
}  

bool Plugin::setOption(uint16_t option, uint8_t channel, float value) {
  myLog.print(LOG_NOTICE,F("   plugin didn't define setOption(option, channel, (float) value)!"));
  return false;
}  

bool Plugin::checkStatus(void) {
  // myLog.print(LOG_DEBUG,F("   plugin didn't define checkStatus()!"));
  return _isInitialized;
}

bool Plugin::available() {
  myLog.print(LOG_TRACE,F("   plugin didn't define available()!"));
  return true;
}

void Plugin::loop() {
  // myLog.print(LOG_TRACE,F("   plugin didn't define loop()!"));
}

bool Plugin::read() {
  myLog.print(LOG_TRACE,F("   plugin didn't define read()!"));
  return true;
}

float Plugin::get(){
  myLog.print(LOG_NOTICE,F("   plugin didn't define get(void)!"));
  return 0;
};

float Plugin::get(uint8_t channel){
  myLog.print(LOG_NOTICE,F("   plugin didn't define get(unit8_t channel)!"));
  return 0;
};

bool Plugin::set(uint8_t channel, float value) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set(float value)!"));
  return false;
}

bool Plugin::set(uint8_t channel, bool value) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set(bool value)!"));
  return false;
}

bool Plugin::set(uint8_t channel, const String& value) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set(string value)!"));
  return false;
}

bool Plugin::set(uint8_t channel, PropertyData* data) {
  myLog.print(LOG_DEBUG,F("   plugin didn't define set(PropertyData* data)!"));
  return false;
}

bool Plugin::set(PropertyData* data) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set((PropertyData*) data)!"));
  return false;
}

bool Plugin::set(int data) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set((int) data)!"));
  return false;
}
