#include "homie_device.hpp"
#include "homie_node.hpp"
#include <utils.h>
#include <signalLED.h>

bool pluginInit(MyHomieNode* homieNode, uint8_t actuatorId, const HomieNodeDef* homieDef) ;

void MyHomieDevice::init(HomieDeviceDef homieDevice) {
  myLog.setAppName("DEVICE");
  myLog.printf(LOG_INFO, "Setup Device [%s]", homieDevice.id);
  deviceDef = homieDevice;
};

MyHomieNode* MyHomieDevice::addNode(HomieNodeDef nodeDef, uint8_t pluginId, uint8_t gpio) {
  myLog.setAppName("DEVICE");
  MyHomieNode* homieNode = new MyHomieNode(nodeDef, pluginId, gpio);
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
/*!
   @brief    set option of one or many properties. Will exits on first error!
    @param    nodeId      (string) the nodeId, wildcards (*,?,.) allowed.
    @param    propertyId  (string) the propertyId, wildcards (*,?,.) allowed. Provide a empty propertyId for node plugins
    @param    option      (uint18_t) option id
    @param    value       (integer) value to be set
    @returns  true if successfull
*/
bool MyHomieDevice::setOption(const char* nodeId, const char* propertyId, uint16_t option, int value) const {
  NodeVector::NodeItem* seek = nodes.getFirst();
  bool success = false;
  while (seek!=NULL) {
    if (wildcmp(nodeId,seek->node->getId())) {
      success = seek->node->setOption(propertyId, option, value);
      if (!success) {
        myLog.printf(LOG_ERR, "%s reported an error (%s)!", seek->node->getId(), nodeId);
        return false;
      }
    }
    seek = nodes.getNext(seek);
  }
  return success;
}
/*!
   @brief    set option of one or many properties. Will exits on first error!
    @param    nodeId      (string) the nodeId, wildcards (*,?,.) allowed.
    @param    propertyId  (string) the propertyId, wildcards (*,?,.) allowed. Provide a empty propertyId for node plugins
    @param    filter      (floatValueFilter) filter callback function
    @returns  true if successfull
*/
bool MyHomieDevice::setFilter(const char* nodeId, const char* propertyId, floatValueFilter filter) const {
  NodeVector::NodeItem* seek = nodes.getFirst();
  bool success = false;
  while (seek!=NULL) {
    if (wildcmp(nodeId,seek->node->getId())) {
      success = seek->node->setFilter(propertyId, filter);
      if (!success) {
        myLog.printf(LOG_ERR, "%s reported an error (%s)!", seek->node->getId(), nodeId);
        return false;
      }
    }
    seek = nodes.getNext(seek);
  }
  return success;
}
/*!
   @brief    set filter callback of one or many properties. Will exits on first error!
    @param    nodeId      (string) the nodeId, wildcards (*,?,.) allowed.
    @param    propertyId  (string) the propertyId, wildcards (*,?,.) allowed. Provide a empty propertyId for node plugins
    @param    filter      (intValueFilter) filter callback function
    @returns  true if successfull
*/
bool MyHomieDevice::setFilter(const char* nodeId, const char* propertyId, intValueFilter filter) const {
  NodeVector::NodeItem* seek = nodes.getFirst();
  bool success = false;
  while (seek!=NULL) {
    if (wildcmp(nodeId,seek->node->getId())) {
      success = seek->node->setFilter(propertyId, filter);
      if (!success) {
        myLog.printf(LOG_ERR, "%s reported an error (%s)!", seek->node->getId(), nodeId);
        return false;
      }
    }
    seek = nodes.getNext(seek);
  }
  return success;
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
  uint16_t loopDuration = 0;
  // start of node loop
  if (_nodeLoopCounter<0 && _nextEvent < millis()) {
    signalLED.trigger(512);
    _nodeLoopCounter=0;
    _timebase = millis();
    _nextNodeEvent = ULONG_MAX;
    myLog.print(LOG_TRACE,F("Device loop started"));
  }
  // call single node
  if (_nodeLoopCounter>=0 && _nodeLoopCounter<nodes.length) {
    myLog.setAppName("homieDevice");
    myLog.printf(LOG_TRACE,F(" Node#%d loop started %s"),_nodeLoopCounter ,getNode(_nodeLoopCounter)->getId());
    _nextNodeEvent = minimum(getNode(_nodeLoopCounter)->loop(_timebase), _nextNodeEvent);
    myLog.printf(LOG_TRACE,F(" Node#%d loop end %s next in %dms"),_nodeLoopCounter ,getNode(_nodeLoopCounter)->getId(), _nextNodeEvent);
    _nodeLoopCounter++;
  }
  // end of node loop
  if (_nodeLoopCounter>=nodes.length) {
    _nodeLoopCounter=-1;
    _nextEvent = _nextNodeEvent + _timebase;
    loopDuration = millis()-_timebase;
    myLog.printf(LOG_TRACE,F("Next device event in %.2fs (took:%dms)"),(float) _nextNodeEvent / 1000, loopDuration);
  }

  // plugin loop runs cuntiously
  if (_pluginLoopCounter<nodes.length) {
    getNode(_pluginLoopCounter)->pluginLoop();
    _pluginLoopCounter++;
  } else {
    _pluginLoopCounter=0;
  }

  return loopDuration;
}

MyHomieDevice myDevice;
