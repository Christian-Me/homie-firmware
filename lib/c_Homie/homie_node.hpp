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
    const HomieNodeDef* nodeDef = NULL;
    uint8_t _pluginId = 0;
    HomieNode *homieNode = NULL;
    PropertyVector properties;
    unsigned long _nextEvent = 0;
  public:
    Plugin* plugin = NULL;
    MyHomieNode(const HomieNodeDef* def, uint8_t pluginId = 0);
    void setPulginId(uint8_t id);
    bool pluginInit(uint8_t pluginId);
    void setDef(const HomieNodeDef *homieNode);
    const char* getId();
    MyHomieProperty* addProperty(const HomiePropertyDef* homieProperty);
    void registerInputHandler(const char* id, InputHandler inputHandler);
    MyHomieProperty* getProperty (int index) const;
    MyHomieProperty* getProperty (const char* id) const;
    uint8_t length() const;
    const HomieNodeDef* getDef() const;
    bool setValue(const char* id, float value);
    bool setValue(const char* id, bool value);
    bool setFactor(const char* id, float value);
    bool sendValue(MyHomieProperty* homieProperty);
    bool sendValue(int index);
    bool sendValue(const char* id);
    unsigned long getNextEvent();
    unsigned long loop();
};