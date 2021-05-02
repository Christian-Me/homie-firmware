#include <Arduino.h>
#include <plugins.h>

const char* Plugin::id() {
  return "default";
}
/*!
   @brief    initialize a node  plugin
    @param    homieNode (MyHomieNode*) pointer to the coresponding homie node
    @returns true on success
*/
bool Plugin::init(MyHomieNode* homieNode) {
  _isInitialized = true;
  myLog.print(LOG_NOTICE,F("   plugin didn't define init(homieNode)!"));
  return false;
}
/*!
   @brief    initialize a property plugin
    @param    homieNode (MyHomieNode*) pointer to the coresponding homie node
    @param    gpio (uint8_t) IO port (I2C address or GPIO port number)
    @returns true on success
*/
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

bool Plugin::setFilter(floatValueFilter filter) {
  return true;
}

bool Plugin::setFilter(intValueFilter filter) {
  return true;
}

uint16_t Plugin::minValue() {return 0;};
uint16_t Plugin::maxValue() {return 1;};   

bool Plugin::checkStatus(void) {
  // myLog.print(LOG_DEBUG,F("   plugin didn't define checkStatus()!"));
  return _isInitialized;
}

bool Plugin::available() {
  // myLog.print(LOG_TRACE,F("   plugin didn't define available()!"));
  return true;
}

void Plugin::loop() {
  // myLog.print(LOG_TRACE,F("   plugin didn't define loop()!"));
}

bool Plugin::read() {
  // myLog.print(LOG_TRACE,F("   plugin didn't define read()!"));
  return true;
}

float Plugin::get(){
  // myLog.print(LOG_NOTICE,F("   plugin didn't define get(void)!"));
  return 0;
};

float Plugin::get(uint8_t channel){
  // myLog.print(LOG_NOTICE,F("   plugin didn't define get(unit8_t channel)!"));
  return 0;
};
/*!
   @brief    set value passed by a float value. To be used by a multi datapoint (node) plugin
    @param    channel (uint8_t) datapoint index
    @param    value (vloat) floating point number
    @returns    (bool) true on success
*/
bool Plugin::set(uint8_t channel, float value) {
  // myLog.print(LOG_TRACE,F("   plugin didn't define set(float value)!"));
  return false;
}
/*!
   @brief    set value passed by a boolean value. To be used by a multi datapoint (node) plugin
    @param    channel (uint8_t) datapoint index
    @param    value (bool) boolean value
    @returns    (bool) true on success
*/
bool Plugin::set(uint8_t channel, bool value) {
  // myLog.print(LOG_TRACE,F("   plugin didn't define set(bool value)!"));
  return false;
}
/*!
   @brief    set value passed by a PropertyData struct. To be used by a multi datapoint (node) plugin. This value will be directly processed and NOT stored.
    @param    channel (uint8_t) datapoint index
    @param    value (String) pointer to text object
    @returns    (bool) true on success
*/
bool Plugin::set(uint8_t channel, const String& value) {
  // myLog.print(LOG_TRACE,F("   plugin didn't define set(string value)!"));
  return false;
/*!
   @brief    set value passed by a PropertyData struct. To be used by a multi datapoint (node) plugin
    @param    channel (uint8_t) datapoint index
    @param    value (PropertyData) data structure
    @returns    (bool) true on success
*/
}
bool Plugin::set(uint8_t channel, PropertyData* data) {
  // myLog.print(LOG_DEBUG,F("   plugin didn't define set(PropertyData* data)!"));
  return false;
}
/*!
   @brief    set value passed by a PropertyData struct. To be used by a single datapoint (property) plugin
    @param    value (PropertyData) data structure
    @returns    (bool) true on success
*/
bool Plugin::set(PropertyData* data) {
  // myLog.print(LOG_TRACE,F("   plugin didn't define set((PropertyData*) data)!"));
  return false;
}
/*!
   @brief    set a integer value.
    @param    value (int) integer number
    @returns    (bool) true on success
*/
bool Plugin::set(int data) {
  // myLog.print(LOG_TRACE,F("   plugin didn't define set((int) data)!"));
  return false;
}
/*!
   @brief    set a string value. This value will be directly processed and NOT stored
    @param    value (String) text value
    @returns    (bool) true on success
*/
bool Plugin::set(const String& value) {
  myLog.print(LOG_DEBUG,F("   plugin didn't define set(string value)!"));
  return false;
}