#include "homie_device.h"
#include "homie_node.h"

bool pluginInit(MyHomieNode* homieNode, uint8_t actuatorId, const HomieNodeDef* homieDef) ;

void MyHomieDevice::init(const HomieDeviceDef *homieDevice) {
  mySysLog_setAppName("SETUP");
  myLogf(LOG_INFO, "Setup Device [%s]", homieDevice->id);
  deviceDef = homieDevice;
};

MyHomieNode* MyHomieDevice::addNode(uint8_t sensorId, const HomieNodeDef* nodeDef) {
  MyHomieNode* homieNode = new MyHomieNode(nodeDef);
  if (homieNode==NULL) return NULL;
  myLogf(LOG_INFO, "Setup plugin for node [%s/%s] %p(%d)", deviceDef->id, nodeDef->id, nodeDef, sizeof(*homieNode));
  pluginInit(homieNode, sensorId, nodeDef);
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
  myLogf(LOG_TRACE,F("  MyHomieDevice::setValue(%s, %s, %.2f)"),nodeId,propertyId,value);
  getNode(nodeId)->setValue(propertyId,value);
  return true;
}
bool MyHomieDevice::setFactor(const char* nodeId, const char* propertyId, float value) const {
  myLogf(LOG_TRACE,F("  MyHomieDevice::setFactor(%s, %s, %.2f)"),nodeId,propertyId,value);
  getNode(nodeId)->setFactor(propertyId,value);
  return true;
}

MyHomieNode* MyHomieDevice::createHomieNode(uint8_t nodeId, const HomieNodeDef* nodeDef) {

  MyHomieNode* node = addNode(nodeId, nodeDef);
  myLogf(LOG_INFO,F("created Node Id:%s Name:'%s' Type:'%s' with %d properties. (%p)"),nodeDef->id,nodeDef->name,nodeDef->type, nodeDef->datapoints,node);

  for (uint8_t i=0; i<nodeDef->datapoints; i++) {
    node->addProperty(&nodeDef->dataPoint[i]);
  }

  return node;
}
