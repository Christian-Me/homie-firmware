#include "homie_node.hpp"
#include "homie_property.hpp"
// #include "homieSyslog.h"
#include "../../include/datatypes.h"
#include <plugins.h>
#include <utils.h>

#ifdef S_BME280
  #include <s_BME280.h>
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
#ifdef S_HDC1080
  #include <s_HDC1080.h>
#endif
#ifdef S_DUMMY
  #include <s_DUMMY.h>
#endif


bool MyHomieNode::pluginInit(uint8_t pluginId) {
  myLog.printf(LOG_DEBUG,F(" Start plugin init for node %s (pluginId:%d) %d"),getDef().id, pluginId, ADS1115_ID);
  plugin = NULL;
  switch (pluginId) {
    #ifdef S_BME280
      case BME280_ID: {
        plugin= new s_BME280(getDef().io);
        myLog.printf(LOG_DEBUG,F("  Plugin: %s"),plugin->id());
        break; }
    #endif
    #ifdef S_BH1750
      case BH1750_ID: {
        plugin= new s_BH1750(getDef().io);
        break; }
    #endif
    #ifdef S_HDC1080
      case HDC1080_ID: {
        plugin= new s_HDC1080(getDef().io);
        break; }
    #endif
    #ifdef S_ADS1115
      case ADS1115_ID: {
        plugin= new s_ADS1115(getDef().io);
        break; }
    #endif
    default: {
      plugin = new Plugin(); }
  }
  if (plugin != NULL) {
    myLog.printf(LOG_INFO,F(" registered plugin '%s' for node %s (pluginId:%d)"),plugin->id(),getDef().id, pluginId);
    plugin->init(this);
    return true;
  } else {
    myLog.printf(LOG_ERR,F(" error initialisation of plugin %s for node %s (pluginId:%d)"),plugin->id(),getDef().id, pluginId);
  }
  return false;
}

MyHomieNode::MyHomieNode(HomieNodeDef def, uint8_t pluginId) {
  nodeDef = def;
  _pluginId = pluginId;
  _nextSample = 0;
  _inputHandler = NULL;
  homieNode = new HomieNode(def.id,def.name, def.type);
  myLog.printf(LOG_INFO, F("Start register Node id:%s name:%s type:%s plugin Id:%d"), def.id, def.name, def.type, _pluginId);
  if (!pluginInit(_pluginId)) {
    myLog.printf(LOG_ERR, " Plugin id=%d reported an error", _pluginId);
  };
};

MyHomieNode* MyHomieNode::setPluginOption(uint16_t option, int value) {
  if (plugin!=NULL) {
    if (!plugin->setOption(option, value)) {
      myLog.printf(LOG_ERR, " Plugin id=%d error setting option:%d value%d", _pluginId, option, value);
    };
  } else {
    myLog.printf(LOG_ERR, " Plugin id=%d undefined", _pluginId);
  }
  return this;
}

void MyHomieNode::setPulginId(uint8_t id = 0) {
  _pluginId = id;
};

const char* MyHomieNode::getId() {
  return nodeDef.id;
};
bool MyHomieNode::defaultNodeInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    myLog.printf(LOG_INFO,F("defaultNodeInputHandler %s/%s=%s (%s)"), homieNode->getId(), homieProperty->getId(), value.c_str(), homieDatatypes[homieProperty->getDef().datatype]);
    bool _result = false;
    
    switch (homieProperty->getDef().datatype) {
      case DATATYPE_FLOAT: {
        float _floatValue = value.toFloat();
        return homieNode->setValue(homieProperty->getDef().id,_floatValue);
      };
      case DATATYPE_BOOLEAN: {
        bool _boolValue = (value.equalsIgnoreCase("true")) ? true : false;
        return homieNode->setValue(homieProperty->getDef().id,_boolValue);
      };
    };
    
    return _result;
};

bool MyHomieNode::inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty){
  myLog.setAppName(homieNode->getId());
  if (_inputHandler!=NULL) {
      return _inputHandler(range,value, homieNode, homieProperty);
  } else {
    return defaultNodeInputHandler(range,value, homieNode, homieProperty);
  }
  return false;
};

MyHomieNode* MyHomieNode::registerInputHandler(InputHandler inputHandler){
  myLog.printf(LOG_INFO, F("Registered input handler for node %s"), getId());
  _inputHandler = inputHandler;
  return this;
};

 MyHomieNode* MyHomieNode::registerInputHandler(const char* id, InputHandler inputHandler){
  myLog.printf(LOG_INFO, F("Registered input handler for property %s/%s"), getId(), id);
  properties.getById(id)->setInputHandler(inputHandler);
  return this;
};

MyHomieNode* MyHomieNode::registerReadHandler(const char* id, ReadHandler readHandler){
  myLog.printf(LOG_INFO, F("Registered read handler for %s/%s"), getId(), id);
  properties.getById(id)->setReadHandler(readHandler);
  return this;
};

MyHomieNode* MyHomieNode::hideLogLevel(const char* id, uint8_t logMask){
  myLog.printf(LOG_INFO, F("Log Mask for %s/%s = %d"), getId(),id,logMask);
  properties.getById(id)->hideLog=logMask;
  return this;
};

MyHomieProperty* MyHomieNode::getProperty (int index) const {
  return properties.get((uint8_t) index);
};

MyHomieProperty* MyHomieNode::getProperty (const char* id) const {
  return properties.getById(id);
};

uint8_t MyHomieNode::length() const {
  return properties.length;
};

HomieNodeDef MyHomieNode::getDef() {
  return nodeDef;
};

bool MyHomieNode::sendValue(MyHomieProperty* homieProperty) {
  return homieProperty->sendValue();
  /*
  float valueFloat = 0;
  switch (homieProperty->getDef().datatype) {
    case DATATYPE_FLOAT: {
      if (!homieProperty->getDef().retained || (homieProperty->getDef().retained && homieProperty->getData()->last!=homieProperty->getData()->current)) {
        myLog.printf(LOG_INFO,F("  MyHomieNode::sendValue(%s) %s=%.2f"),getId(), homieProperty->getId(), homieProperty->getValue());
        homieNode->setProperty(homieProperty->getDef().id).send(String(homieProperty->getValue())); // send homie feedback
        homieProperty->getData()->last=homieProperty->getData()->current;
        triggerLED();
        return true;
      }
      break;
    }
    case DATATYPE_BOOLEAN: {
      valueFloat = homieProperty->getValue();
      if (valueFloat == 0) {
        myLog.printf(LOG_INFO,F("  MyHomieNode::sendValue(%s) %s='false'"),getId(), homieProperty->getId());
        homieNode->setProperty(homieProperty->getDef().id).send(String("false")); // send homie feedback
      } else {
        myLog.printf(LOG_INFO,F("  MyHomieNode::sendValue(%s) %s='true'"),getId(), homieProperty->getId());
        homieNode->setProperty(homieProperty->getDef().id).send(String("true")); // send homie feedback
      }
      homieProperty->getData()->last=homieProperty->getData()->current;
      triggerLED();
      return true;
    }
  }
  return false;
  */
}

void MyHomieNode::setCustomSampleRate(uint16_t rate){
  _customSampleRate = rate;
}
bool MyHomieNode::readyToSample(unsigned long timebase) {
  if (_nextSample<timebase) {
    myLog.printf(LOG_TRACE,F("   Node '%s' ready to sample! (next: %ds)"), nodeDef.id, _pluginSampleRate);
    if (_customSampleRate>0) { // use sample rate defined by plugin (ms)
      _nextSample= timebase + _customSampleRate;
    } else { // use sample rate defined by shortest property sample duration (s)
      _nextSample= timebase + _pluginSampleRate * 1000;
    }
    return true;
  }
  return false;
};

bool MyHomieNode::sendValue(const char* id) {
  return sendValue(getProperty(id));
}

bool MyHomieNode::sendValue(const char* id, float value) {
  return getProperty(id)->sendValue(value);
}

bool MyHomieNode::sendValue(int index) {
  return sendValue(getProperty(index));
}


bool MyHomieNode::setValue(const char* id, float value) {
  if (getProperty(id)!=NULL) {
    bool result = false;
    myLog.printf(LOG_TRACE,F("  MyHomieNode::setValue(%s, (float) %.2f) %.2f %s"),id,value,getProperty(id)->getValue(), getProperty(id)->getId());
    getProperty(id)->setValue(value);
    if (plugin!=NULL) {
      result = plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
      if (result) homieNode->setProperty(id).send(String(value)); // send homie feedback if plugin returns acknowledge
    } else {
      homieNode->setProperty(id).send(String(value));
    }
    return true;
  }
  myLog.printf(LOG_ERR,F("  MyHomieNode::setValue(%s,(float) %.2f) node unknown"),id,value);
  return false;
}

bool MyHomieNode::setValue(const char* id, bool value) {
  if (getProperty(id)!=NULL) {
    myLog.printf(LOG_TRACE,F("  MyHomieNode::setValue(%s,(bool))"),id);
    getProperty(id)->setValue(value);
    if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
    homieNode->setProperty(id).send(String((value) ? "true" : "false")); // send homie feedback
    return true;
  }
  myLog.printf(LOG_ERR,F("  MyHomieNode::setValue(%s,(bool) %d) node unknown"),id,value);
  return false;
}

bool MyHomieNode::setFactor(const char* id, float value) {
  if (getProperty(id)!=NULL) {
    myLog.printf(LOG_INFO,F("  MyHomieNode::setFactor(%s, %.2f)"),id,value);
    getProperty(id)->setFactor(value);
    if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
    // no feedback as homie don't have scale factors
    return true;
  }
  myLog.printf(LOG_ERR,F("  MyHomieNode::setFactor(%s, %.2f) node unknown"),id,value);
  return false;
}
/*
MyHomieNode* MyHomieNode::addProperty2(HomiePropertyDef2 def) {
  myLog.print(LOG_INFO,F("  add property2"));
  myLog.printf(LOG_INFO,F("  add property2 id=%s name=%s unit=%s datatype=%s format=%s"),def.id, def.name, def.unit,homieDatatypes[def.datatype],def.format);
  MyHomieProperty* homieProperty = properties.push(new MyHomieProperty(homieNode, def));
  return this;
};
*/
MyHomieNode* MyHomieNode::addProperty(HomiePropertyDef def) {
  myLog.printf(LOG_INFO,F("  addProperty id=%s name=%s unit=%s datatype=%s format=%s"),def.id, def.name, def.unit,homieDatatypes[def.datatype],def.format);

  MyHomieProperty* homieProperty = properties.push(new MyHomieProperty(homieNode, def));
  if (homieProperty==NULL) {
    myLog.printf(LOG_EMERG,F("  MyHomieNode::addProperty could not store property '%s'!"),def.id);
    return this;
  }
  _pluginSampleRate = minimum(def.sampleRate, _pluginSampleRate);
  if(def.settable == NON_SETTABLE) {
    homieNode->advertise(def.id)
      .setName(def.name)
      .setUnit(def.unit)
      .setRetained(def.retained)
      .setDatatype(homieDatatypes[def.datatype])
      .setFormat(def.format);
  } else {
    homieNode->advertise(def.id)
      .setName(def.name)
      .setUnit(def.unit)
      .setRetained(def.retained)
      .setDatatype(homieDatatypes[def.datatype])
      .setFormat(def.format)
      .settable();
  }
  return this;
}

MyHomieNode* MyHomieNode::addProperty(HomiePropertyDef homiePropertyDef, uint8_t pluginId, uint8_t gpio) {
  addProperty(homiePropertyDef);
  myLog.printf(LOG_INFO,F("   addPlugin to '%s' plugin=%d GPIO%d"),homiePropertyDef.id, pluginId, gpio);

  MyHomieProperty* homieProperty = getProperty(homiePropertyDef.id);
  homieProperty->addPlugin(pluginId, gpio);
  _propertyPluginPresent = true;
  return this;
}
/*!
   @brief    check if node has matches a specific id
    @param    id    (const char*) id to check
   @returns  true if property has this id
*/
bool MyHomieNode::isTarget (const char* id) {
  return (strcmp(getId(),id)==0);
};
/*!
   @brief   determine the next event of all properties of this node 
    @param    timebase    timebase to calculate relative time (duration)
   @returns  next event in seconds
*/
unsigned long MyHomieNode::getNextEvent(unsigned long timebase) {
  unsigned long nextEvent = -1;
  if (_customSampleRate>0) {
    nextEvent = _customSampleRate;
  } else {
    MyHomieProperty* homieProperty = NULL;
    for (int8_t i=0; i<properties.length; i++) {
      homieProperty = getProperty(i);
      if (homieProperty->getDef().sampleRate>0) {
        if (homieProperty->getData()->sampleTimeout-timebase < nextEvent) {
          nextEvent = getProperty(i)->getData()->sampleTimeout-timebase;
          _nextEventIndex = i;
        }
      }
      if (getProperty(i)->getDef().timeout>0) {
        nextEvent = minimum(getProperty(i)->getData()->sendTimeout-timebase,nextEvent);
      }
    }
  }
  return nextEvent;
}

unsigned long MyHomieNode::loop(unsigned long timebase) {
  bool eventOccurred = false;
  bool proceed = true;
  if (_nextEvent < timebase) {
    myLog.setAppName("homieNode");
    MyHomieProperty* property = NULL;
    myLog.printf(LOG_TRACE,F(" sample rate %ds"), _pluginSampleRate);
    if (plugin!=NULL && readyToSample(timebase)) {
      myLog.printf(LOG_TRACE,F(" Node %s sample timer triggered"), getDef().id);
      property = getProperty(_nextEventIndex);
      if (property->readHandler(TASK_BEFORE_SAMPLE,this,property)) {
        if (_pluginId<100) myLog.printf(LOG_TRACE,F("  Property %s/%s read form plugin %s"),getDef().id, property->getId(), (plugin!=NULL) ? plugin->id() : "NULL");
        plugin->read();
        eventOccurred = true;
        proceed = property->readHandler(TASK_AFTER_SAMPLE,this,property);
      }
      if (proceed) {
        for (int8_t i=0; i<properties.length; i++) {
          property = getProperty(i);
          if (property->readyToSample(timebase)) {
            if (property->readHandler(TASK_BEFORE_READ,this,property)) {
              property->setReadValue(plugin->get(i));
              myLog.printf(LOG_TRACE,F("   sample %s = %.2f"), property->getId(), property->getReadValue());
              if (property->readHandler(TASK_AFTER_READ,this,property)) {
                property->setValue(property->getReadValue());
                eventOccurred = true;
              }
            }
          }
        }
      }
    }
    for (int8_t i=0; i<properties.length; i++) {
      if (getProperty(i)!=NULL) {
        if (getProperty(i)->readyToSend(timebase)) {
          myLog.printf(LOG_TRACE,F("  %s ready to send!"),getProperty(i)->getId());
          property = getProperty(i);
          if (property->readHandler(TASK_BEFORE_SEND,this,property)) {
            sendValue(i);
            eventOccurred = true;
          }
          proceed = property->readHandler(TASK_AFTER_SEND,this,property);
        }
      } else {
        myLog.printf(LOG_ERR,F("  property #%d not defined!"), i);
      }
    }
  }
  unsigned long nextEvent = getNextEvent(timebase);
  if (eventOccurred) myLog.printf(LOG_DEBUG,F("  Next Node %s event in %.2fs (%d)"), getDef().id, (float) nextEvent / 1000, _nextEventIndex);
  return nextEvent;
}

void MyHomieNode::pluginLoop() {
  // first node plugin
  if (plugin!=NULL) {
    plugin->loop();
  }
  // second properties plugin
  if (_propertyPluginPresent) {
    MyHomieProperty* property = NULL;
    for (int8_t i=0; i<properties.length; i++) {
      property = getProperty(i);
      if (property->getPlugin()!=NULL) {
       property->getPlugin()->loop();
      }
    }
  }
}
