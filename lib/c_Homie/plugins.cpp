#include <Arduino.h>
#include <plugins.h>

const char* Plugin::id() {
  return "default";
}

bool Plugin::init(const MyHomieNode* homieNode) {
  myLog.print(LOG_TRACE,F("   plugin didn't define init(def)!"));
  return false;
}

bool Plugin::checkStatus(void) {
  return _isInitialized;
}

bool Plugin::available(void) {
  return true;
}

void Plugin::loop() {
  myLog.print(LOG_TRACE,F("   plugin didn't define loop()!"));
}

bool Plugin::read(bool force) {
  myLog.print(LOG_TRACE,F("   plugin didn't define read()!"));
  return false;
}

float Plugin::get(uint8_t channel){
  myLog.print(LOG_DEBUG,F("   plugin didn't define get(unit8_t channel)!"));
  return 0;
};

bool Plugin::set(uint8_t channel, float value) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set(float value)!"));
  return false;
}

bool Plugin::set(uint8_t channel, const String& value) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set(string value)!"));
  return false;
}

bool Plugin::set(uint8_t channel, PropertyData* data) {
  myLog.print(LOG_TRACE,F("   plugin didn't define set(PropertyData* data)!"));
  return false;
}
