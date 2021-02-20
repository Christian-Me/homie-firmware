#include "homie_device.hpp"
#include "homie_node.hpp"
#include "utils.h"

bool pluginInit(MyHomieNode* homieNode, uint8_t actuatorId, const HomieNodeDef* homieDef) ;

void MyHomieDevice::init(HomieDeviceDef homieDevice) {
  myLog.setAppName("DEVICE");
  myLog.printf(LOG_INFO, "Setup Device [%s]", homieDevice.id);
  deviceDef = homieDevice;
};

MyHomieNode* MyHomieDevice::addNode(uint8_t pluginId, HomieNodeDef nodeDef) {
  myLog.setAppName("DEVICE");
  MyHomieNode* homieNode = new MyHomieNode(nodeDef, pluginId);
  if (homieNode==NULL) {
    myLog.printf(LOG_EMERG,F("  MyHomieDevice::addNode could not store node '%s'!"),nodeDef.id);
    return NULL;
  }
  myLog.printf(LOG_INFO, "Setup plugin for node [%s/%s] %p(%d)", deviceDef.id, nodeDef.id, nodeDef, sizeof(*homieNode));
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
const HomieDeviceDef MyHomieDevice::getDef() {
  return deviceDef;
}
bool MyHomieDevice::setValue(const char* nodeId, const char* propertyId, float value) const {
  myLog.setAppName("DEVICE");
  myLog.printf(LOG_TRACE,F("  MyHomieDevice::setValue(%s, %s, %.2f)"),nodeId,propertyId,value);
  getNode(nodeId)->setValue(propertyId,value);
  return true;
}
bool MyHomieDevice::setFactor(const char* nodeId, const char* propertyId, float value) const {
  myLog.setAppName("DEVICE");
  myLog.printf(LOG_TRACE,F("  MyHomieDevice::setFactor(%s, %s, %.2f)"),nodeId,propertyId,value);
  getNode(nodeId)->setFactor(propertyId,value);
  return true;
}

unsigned long MyHomieDevice::loop() {
  if (_nextEvent < millis()) {
    myLog.setAppName("homieDevice");
    myLog.print(LOG_TRACE,F("Device.loop() started"));
    unsigned long timebase = millis(); // read only once!
    unsigned long nextNodeEvent = -1;
    for (int8_t i=0; i<nodes.length; i++) {
      nextNodeEvent = minimum(getNode(i)->loop(timebase), nextNodeEvent);
    }
    _nextEvent = nextNodeEvent + timebase;
    myLog.printf(LOG_DEBUG,F("Next device event in %.2fs (loop:%dms)"),(float) nextNodeEvent / 1000,millis()-timebase);
  }
  for (int8_t i=0; i<nodes.length; i++) {
    getNode(i)->pluginLoop();
  }

  return _nextEvent;
}

MyHomieDevice myDevice;
