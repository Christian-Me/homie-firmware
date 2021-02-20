#pragma once
#include <homieSyslog.h>
#include "homie_property.hpp"
// #include "datatypes.h"
// #include "plugins.h"

struct PropertyVector {
  struct PropertyItem {
    MyHomieProperty* property = NULL;
    PropertyItem* next = NULL;
    PropertyItem(MyHomieProperty* homieProperty) {
      property = homieProperty;
      next = NULL;
    }
  };
  PropertyItem* first = NULL;
  uint8_t length = 0;
  MyHomieProperty* push(MyHomieProperty* homieProperty) {
    ++length;
    if (first == NULL) {
      first = new PropertyItem(homieProperty);
    } else {
      PropertyItem* seek = first;
      while (seek->next != NULL) {
        seek=seek->next;
      }
      seek->next = new PropertyItem(homieProperty);
    }
    return homieProperty;
  };
  MyHomieProperty* getById(const char* id) const {
    PropertyItem* seek = first;
    while (seek != NULL) {
      if (strcmp(seek->property->getId(),id)==0) {
        return seek->property;
      }
      seek=seek->next;
    }
    return NULL;
  }
  MyHomieProperty* get(uint8_t index) const {
    PropertyItem* seek = first;
    if (seek==NULL || index>length-1) return NULL;
    for (int i = 0; i<index; i++) { 
      seek=seek->next;
    }
    return seek->property;
  }
  int getIndex(const char* id) const {
    PropertyItem* seek = first;
    uint8_t _index=0;
    while (seek != NULL) {
      if (strcmp(seek->property->getId(),id)==0) {
        return _index;
      }
      _index++;
      seek=seek->next;
    }
    return -1;
  }
};

class Plugin;
class MyHomieNode {
    HomieNodeDef nodeDef = {"", "", "", 0};
    uint8_t _pluginId = 0;
    bool _propertyPluginPresent = false;
    HomieNode *homieNode = NULL;
    PropertyVector properties;
    unsigned long _nextEvent = 0;
    uint8_t _nextEventIndex = 0;
    uint16_t _pluginSampleRate = -1;
    uint16_t _customSampleRate = 0;
    unsigned long _nextSample = 0;
    InputHandler _inputHandler;
    bool defaultNodeInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
  public:
    Plugin* plugin = NULL;
    MyHomieNode(HomieNodeDef def, uint8_t pluginId = 0);
    MyHomieNode* setPluginOption(uint16_t option, int value);
    void setPulginId(uint8_t id);
    bool pluginInit(uint8_t pluginId);
    void setCustomSampleRate(uint16_t rate);
    const char* getId();
    bool isTarget (const char* id);
    MyHomieNode* addProperty(HomiePropertyDef homieProperty);
    MyHomieNode* addProperty(HomiePropertyDef homieProperty, uint8_t pluginId, uint8_t gpio);
//     MyHomieNode* addProperty2(HomiePropertyDef2 homieProperty);
    bool inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    MyHomieNode* registerInputHandler(InputHandler inputHandler);
    MyHomieNode* registerInputHandler(const char* id, InputHandler inputHandler);
    MyHomieNode* registerReadHandler(const char* id, ReadHandler readHandler);
    MyHomieNode* hideLogLevel(const char* id, uint8_t logMask);
    MyHomieProperty* getProperty (int index) const;
    MyHomieProperty* getProperty (const char* id) const;
    uint8_t length() const;
    HomieNodeDef getDef();
    bool readyToSample(unsigned long timebase);
    bool setValue(const char* id, float value);
    bool setValue(const char* id, bool value);
    bool setFactor(const char* id, float value);
    bool sendValue(MyHomieProperty* homieProperty);
    bool sendValue(int index);
    bool sendValue(const char* id);
    bool sendValue(const char* id, float value);
    unsigned long getNextEvent(unsigned long timebase);
    unsigned long loop(unsigned long timebase);
    void pluginLoop();
};