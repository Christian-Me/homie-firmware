#include "homie_device.hpp"
#include "homie_node.hpp"
#include "utils.h"

bool pluginInit(MyHomieNode* homieNode, uint8_t actuatorId, const HomieNodeDef* homieDef) ;

void MyHomieDevice::init(const HomieDeviceDef *homieDevice) {
  myLog.setAppName("SETUP");
  myLog.printf(LOG_INFO, "Setup Device [%s]", homieDevice->id);
  deviceDef = homieDevice;
};

MyHomieNode* MyHomieDevice::addNode(uint8_t pluginId, const HomieNodeDef* nodeDef) {
  MyHomieNode* homieNode = new MyHomieNode(nodeDef, pluginId);
  if (homieNode==NULL) return NULL;
  myLog.printf(LOG_INFO, "Setup plugin for node [%s/%s] %p(%d)", deviceDef->id, nodeDef->id, nodeDef, sizeof(*homieNode));
  return nodes.push(homieNode);
};

MyHomieNode* MyHomieDevice::getNode(uint8_t index) const {
  return nodes.get(index);
}
MyHomieNode* MyHomieDevice::getNode(const char* id) const {
  return nodes.getById(id);
};
uint8_t MyHomieDevice::length() {
  return nodes.length;
}
const HomieDeviceDef* MyHomieDevice::getDef() {
  return deviceDef;
}
bool MyHomieDevice::setValue(const char* nodeId, const char* propertyId, float value) const {
  myLog.printf(LOG_TRACE,F("  MyHomieDevice::setValue(%s, %s, %.2f)"),nodeId,propertyId,value);
  getNode(nodeId)->setValue(propertyId,value);
  return true;
}
bool MyHomieDevice::setFactor(const char* nodeId, const char* propertyId, float value) const {
  myLog.printf(LOG_TRACE,F("  MyHomieDevice::setFactor(%s, %s, %.2f)"),nodeId,propertyId,value);
  getNode(nodeId)->setFactor(propertyId,value);
  return true;
}

MyHomieNode* MyHomieDevice::createHomieNode(uint8_t pluginId, const HomieNodeDef* nodeDef) {

  MyHomieNode* node = addNode(pluginId, nodeDef);
  myLog.printf(LOG_INFO,F("created Node Id:%s Name:'%s' Type:'%s' with %d properties. (%p)"),nodeDef->id,nodeDef->name,nodeDef->type, nodeDef->datapoints,node);

  for (uint8_t i=0; i<nodeDef->datapoints; i++) {
    node->addProperty(&nodeDef->dataPoint[i]);
  }

  return node;
}


MyHomieProperty* MyHomieDevice::addHomieParameter(const char* nodeId, const HomiePropertyDef2* propertyDef) {
  myLog.printf(LOG_INFO,F("Add parameter to node Id:%s parmeter:'%s'"),nodeId,propertyDef->id);
}

unsigned long MyHomieDevice::loop() {
  if (_nextEvent < millis()) {
    unsigned long timebase = millis(); // read only once!
    unsigned long nextNodeEvent = -1;
    for (int8_t i=0; i<nodes.length; i++) {
      nextNodeEvent = minimum(getNode(i)->loop(timebase), nextNodeEvent);
    }
    _nextEvent = nextNodeEvent + timebase;
    myLog.printf(LOG_INFO,F("Next device event in %.2fs"),(float) nextNodeEvent / 1000);
  }
  return _nextEvent;
}

MyHomieDevice myDevice;
