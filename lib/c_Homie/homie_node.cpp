#include "homie_node.hpp"
#include "homie_property.hpp"
// #include "homieSyslog.h"
#include "../../include/datatypes.h"
#include "plugins.h"
#include "utils.h"

#ifdef A_PWM
  #include "a_pwm_dimmer.h"
#endif
#ifdef S_BME280
  #include "s_bme280.h"
#endif

bool MyHomieNode::pluginInit(uint8_t pluginId) {
  switch (pluginId) {
    #ifdef A_PWM
      case PWM_ID: 
        plugin= new a_PWMdimmer();
        break;
    #endif
    #ifdef S_BME280
      case BME280_ID: 
        plugin= new s_BME280(getDef()->io);

        break;
    #endif
    default:
      myLog.printf(LOG_INFO,F(" virtual plugin for node %s (pluginId:%d)"),getDef()->id, pluginId);
      plugin = new Plugin();
  }
  myLog.printf(LOG_INFO,F(" registered plugin %s for node %s (pluginId:%d)"),plugin->id(),getDef()->id, pluginId);
  if (plugin != NULL) {
    plugin->init(this);
    return true;
  }
  return false;
}

MyHomieNode::MyHomieNode(const HomieNodeDef* def, uint8_t pluginId) {
  nodeDef = def;
  _pluginId = pluginId;
  homieNode = new HomieNode(def->id,def->name, def->type);
  myLog.printf(LOG_INFO, F("Registered Node id:%s name:%s type:%s"), def->id, def->name, def->type);
  if (!pluginInit(_pluginId)) {
    myLog.printf(LOG_ERR, " Plugin id=%d reported an error", _pluginId);
  };
};

void MyHomieNode::setPulginId(uint8_t id = 0) {
  _pluginId = id;
};

void MyHomieNode::setDef(const HomieNodeDef *homieNode) {
  nodeDef = homieNode;
};

const char* MyHomieNode::getId() {
  return (nodeDef!=NULL) ? nodeDef->id : "";
};

void MyHomieNode::registerInputHandler(const char* id, InputHandler inputHandler){
  myLog.printf(LOG_INFO, F("Registered input handler for %s"), id);
  properties.getById(id)->setInputHandler(inputHandler);
};

MyHomieProperty* MyHomieNode::getProperty (int index) const {
  return properties.get((uint8_t) index);
};

MyHomieProperty* MyHomieNode::getProperty (const char* id) const {
  myLog.printf(LOG_TRACE,F("  getProperty(%s)"),id);
  return properties.getById(id);
};

uint8_t MyHomieNode::length() const {
  return properties.length;
};

const HomieNodeDef* MyHomieNode::getDef() const {
  return nodeDef;
};

bool MyHomieNode::sendValue(MyHomieProperty* homieProperty) {
  switch (homieProperty->getDef()->datatype) {
    case DATATYPE_FLOAT: {
      myLog.printf(LOG_DEBUG,F("  MyHomieNode::sendValue(%s) %.2f %s"),homieProperty->getDef()->id,homieProperty->getValue(), homieProperty->getId());
      homieNode->setProperty(homieProperty->getDef()->id).send(String(homieProperty->getValue())); // send homie feedback
    }
  }
  return true;
}

bool MyHomieNode::sendValue(const char* id) {
  return sendValue(getProperty(id));
}

bool MyHomieNode::sendValue(int index) {
  return sendValue(getProperty(index));
}


bool MyHomieNode::setValue(const char* id, float value) {
  getProperty(id)->setValue(value);
  myLog.printf(LOG_TRACE,F("  MyHomieNode::setValue(%s, (float) %.2f) %.2f %s"),id,value,getProperty(id)->getValue(), getProperty(id)->getId());
  if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
  homieNode->setProperty(id).send(String(value)); // send homie feedback
  return true;
}

bool MyHomieNode::setValue(const char* id, bool value) {
  myLog.printf(LOG_DEBUG,F("  MyHomieNode::setValue(%s,(bool) %s)"),id,(value) ? "true" : "false");
  if (getProperty(id)!=NULL) {
    getProperty(id)->setValue(value);
    // if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
  }
  
  homieNode->setProperty(id).send(String((value) ? "true" : "false")); // send homie feedback
  return true;
}

bool MyHomieNode::setFactor(const char* id, float value) {
  myLog.printf(LOG_TRACE,F("  MyHomieNode::setFactor(%s, %.2f)"),id,value);
  getProperty(id)->setFactor(value);
  if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
  // no feedback as homie don't have scale factors
  return true;
}

MyHomieProperty* MyHomieNode::addProperty(const HomiePropertyDef* def) {
  myLog.printf(LOG_INFO,F("  add property id=%s name=%s unit=%s datatype=%s format=%s"),def->id, def->name, def->unit,homieDatatypes[def->datatype],def->format);

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

unsigned long MyHomieNode::getNextEvent() {
  unsigned long nextEvent = -1;
  for (int8_t i=0; i<properties.length; i++) {
    for (int8_t i=0; i<properties.length; i++) {
      if (getProperty(i)->getDef()->sampleRate>0) {
        nextEvent = minimum(getProperty(i)->getData()->sampleTimeout,nextEvent);
      }
      if (getProperty(i)->getDef()->timeout>0) {
        nextEvent = minimum(getProperty(i)->getData()->sendTimeout,nextEvent);
      }
    }
  }
  return nextEvent;
}

unsigned long MyHomieNode::loop() {
  if (_nextEvent < millis()) {
    myLog.printf(LOG_DEBUG,F(" Node %s timer triggered"), getDef()->id);
    if (getProperty(0)!=NULL) {
      if (getProperty(0)->readyToSample()) {
          myLog.printf(LOG_DEBUG,F("  Node %s read form plugin %s"),getDef()->id, (plugin!=NULL) ? plugin->id() : "NULL");
          if (plugin!=NULL) {
            plugin->read(true);
          } else {
            myLog.print(LOG_DEBUG,F("  no plugin defined"));
          }
      }
    } else {
      myLog.print(LOG_ERR,F("  no property #0 defined!"));
    }
    myLog.print(LOG_TRACE,F("  ready to send?"));
    for (int8_t i=0; i<properties.length; i++) {
      if (getProperty(i)!=NULL) {
        if (getProperty(i)->readyToSend()) {
          myLog.printf(LOG_DEBUG,F("  property %s send!"),getProperty(i)->getDef()->id);
          sendValue(i);
        }
      } else {
        myLog.printf(LOG_ERR,F("  property #%d not defined!"), i);
      }
    }
  }
  myLog.print(LOG_TRACE,F("  next Event?"));
  uint32_t nextEvent = getNextEvent();
  if (nextEvent<-1) myLog.printf(LOG_INFO,F(" Next Node %s event in %.2fs"), getDef()->id, (float) nextEvent / 1000);
  return nextEvent;
}
