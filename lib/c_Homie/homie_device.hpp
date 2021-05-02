#pragma once
#include "homie_node.hpp"
#include <plugins.h>

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
  NodeItem* getFirst() const {
    return first;
  }
  NodeItem* getNext(NodeItem* item) const {
    return item->next;
  }
  MyHomieNode* getById(const char* id) const {
    NodeItem* seek = first;
    while (seek != NULL) {
      if (strcmp(seek->node->getId(),id) == 0) {
        return seek->node;
      }
      seek=seek->next;
    }
    myLog.printf(LOG_ALERT,F("Node %s unknown! Returning NULL!"),id);
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
    HomieDeviceDef deviceDef = {"",""};
    NodeVector nodes;
    unsigned long _nextEvent = 0;
    int _nodeLoopCounter = -1; // inactive
    int _pluginLoopCounter = 0;
    unsigned long _timebase = 0;
    unsigned long _nextNodeEvent = 0;
  public:
    void init(HomieDeviceDef homieDevice);
    MyHomieNode* addNode(HomieNodeDef nodeDef,uint8_t pluginId = VIRTUAL_ID, uint8_t gpio = 0);
    MyHomieNode* getNode(uint8_t index) const;
    MyHomieNode* getNode(const char* id) const;
    uint8_t length();
    const HomieDeviceDef getDef();
    bool setOption(const char* nodeId, const char* propertyId, uint16_t option, int value) const;
    bool setFilter(const char* nodeId, const char* propertyId, floatValueFilter filter) const;
    bool setFilter(const char* nodeId, const char* propertyId, intValueFilter filter) const;
    bool setValue(const char* nodeId, const char* propertyId, float value) const;
    bool setFactor(const char* nodeId, const char* propertyId, float value) const;
    unsigned long loop();
};

extern MyHomieDevice myDevice;