#pragma once
// #include "datatypes.h"
#include "homie_node.hpp"

struct NodeVector {
  struct NodeItem {
    MyHomieNode* node = NULL;
    NodeItem* next = NULL;
    NodeItem(MyHomieNode* homieNode) {
      node = homieNode;
      myLog.printf(LOG_TRACE,F("  pushed node %d"),node);
      next = NULL;
    }
  };
  NodeItem* first = NULL;
  uint8_t length = 0;
  MyHomieNode* push(MyHomieNode* homieNode) {
    ++length;
    if (first == NULL) {
      first = new NodeItem(homieNode);
    } else {
      NodeItem* seek = first;
      while (seek->next != NULL) {
        seek=seek->next;
      }
      seek->next = new NodeItem(homieNode);
    }
    return homieNode;
  };
  MyHomieNode* getById(const char* id) const {
    NodeItem* seek = first;
    while (seek != NULL) {
      if (strcmp(seek->node->getId(),id) == 0) {
        return seek->node;
      }
      seek=seek->next;
    }
    return NULL;
  }
  MyHomieNode* get(uint8_t index) const {
    NodeItem* seek = first;
    if (seek==NULL || index>length-1) return NULL;
    for (int i = 0; i<index; i++) { 
      seek=seek->next;
    }
    return seek->node;
  }
};

class MyHomieDevice {
    const HomieDeviceDef* deviceDef = NULL;
    NodeVector nodes;
    unsigned long _nextEvent = 0;
  public:
    void init(const HomieDeviceDef *homieDevice);
    MyHomieNode* addNode(uint8_t pluginId, const HomieNodeDef *homieNode);
    MyHomieNode* getNode(uint8_t index) const;
    MyHomieNode* getNode(const char* id) const;
    uint8_t length();
    const HomieDeviceDef* getDef();
    bool setValue(const char* nodeId, const char* propertyId, float value) const;
    bool setFactor(const char* nodeId, const char* propertyId, float value) const;
    MyHomieNode* createHomieNode(uint8_t pluginId, const HomieNodeDef* nodeDef);
    MyHomieProperty* addHomieParameter(const char* nodeId, const HomiePropertyDef2* propertyDef);
    unsigned long loop();
};

extern MyHomieDevice myDevice;