#include "homie_node.h"
#include "homie_property.h"
// #include "homieSyslog.h"
#include "../../include/datatypes.h"
#include "plugins.h"


MyHomieNode::MyHomieNode(const HomieNodeDef* def) {
  nodeDef = def;
  homieNode = new HomieNode(def->id,def->name, def->type);
  myLogf(LOG_INFO, F("Registered Node id:%s name:%s type:%s"), def->id, def->name, def->type);
};

void MyHomieNode::setPulginId(uint8_t id = 0) {
  pluginId = id;
};

void MyHomieNode::setDef(const HomieNodeDef *homieNode) {
  nodeDef = homieNode;
};

const char* MyHomieNode::getId() {
  return (nodeDef!=NULL) ? nodeDef->id : "";
};

void MyHomieNode::registerInputHandler(const char* id, InputHandler inputHandler){
  myLogf(LOG_INFO, F("Registered input handler for %s"), id);
  properties.getById(id)->setInputHandler(inputHandler);
};

MyHomieProperty* MyHomieNode::getProperty (uint8_t index) const {
  return properties.get(index);
};

MyHomieProperty* MyHomieNode::getProperty (const char* id) const {
  myLogf(LOG_TRACE,F("  getProperty(%s)"),id);
  return properties.getById(id);
};

uint8_t MyHomieNode::length() const {
  return properties.length;
};

const HomieNodeDef* MyHomieNode::getDef() const {
  return nodeDef;
};

bool MyHomieNode::setValue(const char* id, float value) {
  getProperty(id)->setValue(value);
  myLogf(LOG_TRACE,F("  MyHomieNode::setValue(%s, (float) %.2f) %.2f %s"),id,value,getProperty(id)->getValue(), getProperty(id)->getId());
  if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
  homieNode->setProperty(id).send(String(value)); // send homie feedback
  return true;
}

bool MyHomieNode::setValue(const char* id, bool value) {
  myLogf(LOG_DEBUG,F("  MyHomieNode::setValue(%s,(bool) %s)"),id,(value) ? "true" : "false");
  if (getProperty(id)!=NULL) {
    getProperty(id)->setValue(value);
    // if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
  }
  
  homieNode->setProperty(id).send(String((value) ? "true" : "false")); // send homie feedback
  return true;
}

bool MyHomieNode::setFactor(const char* id, float value) {
  myLogf(LOG_TRACE,F("  MyHomieNode::setFactor(%s, %.2f)"),id,value);
  getProperty(id)->setFactor(value);
  if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
  // no feedback as homie don't have scale factors
  return true;
}

MyHomieProperty* MyHomieNode::addProperty(const HomiePropertyDef* def) {
  myLogf(LOG_INFO,F("  add property id=%s name=%s unit=%s datatype=%s format=%s"),def->id, def->name, def->unit,homieDatatypes[def->datatype],def->format);

  MyHomieProperty* homieProperty = properties.push(new MyHomieProperty(def));
  if (homieProperty==NULL) return NULL;

  if(def->settable == NON_SETTABLE) {
    homieNode->advertise(def->id)
      .setName(def->name)
      .setUnit(def->unit)
      .setRetained(def->retained)
      .setDatatype(homieDatatypes[def->datatype])
      .setFormat(def->format);
  } else {
    homieNode->advertise(def->id)
      .setName(def->name)
      .setUnit(def->unit)
      .setRetained(def->retained)
      .setDatatype(homieDatatypes[def->datatype])
      .setFormat(def->format)
      .settable();
  }
  return homieProperty;
}
