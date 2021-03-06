#include "homie_node.hpp"
#include "homie_property.hpp"
// #include "homieSyslog.h"
#include "../../include/datatypes.h"
#include "plugins.h"
#include <utils.h>

#ifdef A_PWM
  #include <a_pwm_dimmer.h>
#endif
#ifdef S_BME280
  #include <s_bme280.h>
#endif
#ifdef S_BME680
  #include <s_BME680.h>
#endif
#ifdef S_ADS1115
  #include <s_ADS1115.h>
#endif
#ifdef S_BH1750
  #include <s_BH1750.h>
#endif
#ifdef S_DUMMY
  #include <s_DUMMY.h>
#endif

bool MyHomieNode::pluginInit(uint8_t pluginId) {
  myLog.printf(LOG_DEBUG,F(" Start plugin init for node %s (pluginId:%d) %d"),getDef()->id, pluginId, ADS1115_ID);
  plugin = NULL;
  switch (pluginId) {
    #ifdef A_PWM
      case PWM_ID: {
        myLog.print(LOG_DEBUG,F("  PWM"));
        plugin= new a_PWMdimmer();
        break; }
    #endif
    #ifdef S_BME280
      case BME280_ID: {
        myLog.print(LOG_DEBUG,F("  BME280"));
        plugin= new s_BME280(getDef()->io);
        break; }
    #endif
    #ifdef S_BH1750
      case BH1750_ID: {
        myLog.print(LOG_DEBUG,F("  BH1750"));
        plugin= new s_BH1750(getDef()->io);
        break; }
    #endif
    #ifdef S_ADS1115
      case ADS1115_ID: {
        myLog.print(LOG_DEBUG,F("  ADS1115"));
        plugin= new s_ADS1115(getDef()->io);
        break; }
    #endif
    default: {
      myLog.printf(LOG_INFO,F("  virtual"));
      plugin = new Plugin(); }
  }
  if (plugin != NULL) {
    myLog.printf(LOG_INFO,F(" registered plugin %s for node %s (pluginId:%d)"),plugin->id(),getDef()->id, pluginId);
    plugin->init(this);
    return true;
  } else {
    myLog.printf(LOG_ERR,F(" error initialisation of plugin %s for node %s (pluginId:%d)"),plugin->id(),getDef()->id, pluginId);
  }
  return false;
}

MyHomieNode::MyHomieNode(const HomieNodeDef* def, uint8_t pluginId) {
  nodeDef = def;
  _pluginId = pluginId;
  homieNode = new HomieNode(def->id,def->name, def->type);
  myLog.printf(LOG_INFO, F("Start register Node id:%s name:%s type:%s plugin Id:%d"), def->id, def->name, def->type, _pluginId);
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

void MyHomieNode::registerReadHandler(const char* id, ReadHandler readHandler){
  myLog.printf(LOG_INFO, F("Registered read handler for %s"), id);
  properties.getById(id)->setReadHandler(readHandler);
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

unsigned long MyHomieNode::getNextEvent(unsigned long timebase) {
  unsigned long nextEvent = -1;
  for (int8_t i=0; i<properties.length; i++) {
    if (getProperty(i)->getDef()->sampleRate>0) {
      nextEvent = minimum(getProperty(i)->getData()->sampleTimeout-timebase,nextEvent);
    }
    if (getProperty(i)->getDef()->timeout>0) {
      nextEvent = minimum(getProperty(i)->getData()->sendTimeout-timebase,nextEvent);
    }
  }
  return nextEvent;
}

unsigned long MyHomieNode::loop(unsigned long timebase) {
  if (_nextEvent < timebase) {
    myLog.printf(LOG_DEBUG,F(" Node %s timer triggered"), getDef()->id);
    MyHomieProperty* property = getProperty(0);
    if (property!=NULL) {
      if (property->readyToSample(timebase)) {
          myLog.printf(LOG_DEBUG,F("  Node %s read form plugin %s"),getDef()->id, (plugin!=NULL) ? plugin->id() : "NULL");
          if (plugin!=NULL) {
            if (property->readHandler(TASK_BEFORE_READ,this,property)) {
              plugin->read(true);
            }
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
        if (getProperty(i)->readyToSend(timebase)) {
          myLog.printf(LOG_DEBUG,F("  property %s send!"),getProperty(i)->getDef()->id);
          sendValue(i);
        }
      } else {
        myLog.printf(LOG_ERR,F("  property #%d not defined!"), i);
      }
    }
  }
  myLog.print(LOG_TRACE,F("  next Event?"));
  uint32_t nextEvent = getNextEvent(timebase);
  if (nextEvent<-1) myLog.printf(LOG_INFO,F(" Next Node %s event in %.2fs"), getDef()->id, (float) nextEvent / 1000);
  return nextEvent;
}
