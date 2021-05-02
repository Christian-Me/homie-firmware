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
#ifdef S_HLW8012
  #include <s_HLW8012.h>
#endif  


bool MyHomieNode::pluginInit(uint8_t pluginId, uint8_t gpio) {
  myLog.printf(LOG_DEBUG,F(" Start plugin init for node %s (pluginId:%d) %d"),getDef().id, pluginId, ADS1115_ID);
  plugin = NULL;
  switch (pluginId) {
    #ifdef S_BME280
      case BME280_ID: {
        plugin= new s_BME280(gpio);
        myLog.printf(LOG_DEBUG,F("  Plugin: %s"),plugin->id());
        break; }
    #endif
    #ifdef S_BH1750
      case BH1750_ID: {
        plugin= new s_BH1750(gpio);
        break; }
    #endif
    #ifdef S_HDC1080
      case HDC1080_ID: {
        plugin= new s_HDC1080(gpio);
        break; }
    #endif
    #ifdef S_ADS1115
      case ADS1115_ID: {
        plugin= new s_ADS1115(gpio);
        break; }
    #endif
    #ifdef S_HLW8012
      case HLW8012_ID: {
        plugin= new s_HLW8012(gpio);
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

MyHomieNode::MyHomieNode(HomieNodeDef def, uint8_t pluginId, uint8_t gpio) {
  nodeDef = def;
  _pluginId = pluginId;
  _gpio = gpio;
  _nextSample = 0;
  _inputHandler = NULL;
  homieNode = new HomieNode(def.id,def.name, def.type);
  myLog.printf(LOG_INFO, F("Start register Node id:%s name:%s type:%s plugin Id:%d"), def.id, def.name, def.type, _pluginId);
  if (!pluginInit(_pluginId, _gpio)) {
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
/*!
   @brief    set option of one or many properties. Will exits on first error!
    @param    propertyId  (string) the propertyId, wildcards (*,?,.) allowed.
    @param    option      (uint18_t) option id
    @param    value       (integer) value to be set
    @returns  true if successfull
*/
bool MyHomieNode::setOption(const char* propertyId, uint16_t option, int value) const {
  bool success = false;
  if (strcmp(propertyId,"")==0) {
    if (plugin!=NULL) {
        if (plugin != NULL) {
          success = plugin->setOption(option, value);
        } else {
          myLog.printf(LOG_ERR, " No plugin defined for node:'%s'!", getId());
        }
    }
  } else {
    PropertyVector::PropertyItem* seek = properties.getFirst();
    Plugin* plugin = NULL;
    while (seek!=NULL) {
      if (wildcmp(propertyId,seek->property->getId())) {
        plugin = seek->property->getPlugin();
        if (plugin != NULL) {
          success = plugin->setOption(option, value);
        } else {
          myLog.printf(LOG_ERR, " No plugin defined for property:'%s' (%s)!", seek->property->getId(), propertyId);
          return false;
        }
        if (!success) return false;
      }
      seek = properties.getNext(seek);
    }
  }
  return success;
};
/*!
   @brief    set option of one or many properties. Will exits on first error!
    @param    propertyId  (string) the propertyId, wildcards (*,?,.) allowed.
    @param    filter      (floatValueFilter) filter callback function
    @returns  true if successfull
*/
bool MyHomieNode::setFilter(const char* propertyId, floatValueFilter filter) const {
  bool success = false;
  if (strcmp(propertyId,"")==0) {
    if (plugin!=NULL) {
        if (plugin != NULL) {
          success = plugin->setFilter(filter);
        } else {
          myLog.printf(LOG_ERR, " No plugin defined for node:'%s'!", getId());
        }
    }
  } else {
    PropertyVector::PropertyItem* seek = properties.getFirst();
    Plugin* plugin = NULL;
    while (seek!=NULL) {
      if (wildcmp(propertyId,seek->property->getId())) {
        plugin = seek->property->getPlugin();
        if (plugin != NULL) {
          success = plugin->setFilter(filter);
        } else {
          myLog.printf(LOG_ERR, " No plugin defined for property:'%s' (%s)!", seek->property->getId(), propertyId);
          return false;
        }
        if (!success) return false;
      }
      seek = properties.getNext(seek);
    }
  }
  return success;
};
/*!
   @brief    set filter callback of one or many properties. Will exits on first error!
    @param    propertyId  (string) the propertyId, wildcards (*,?,.) allowed.
    @param    filter      (intValueFilter) filter callback function
    @returns  true if successfull
*/
bool MyHomieNode::setFilter(const char* propertyId, intValueFilter filter) const {
  bool success = false;
  if (strcmp(propertyId,"")==0) {
    if (plugin!=NULL) {
        if (plugin != NULL) {
          success = plugin->setFilter(filter);
        } else {
          myLog.printf(LOG_ERR, " No plugin defined for node:'%s'!", getId());
        }
    }
  } else {
    PropertyVector::PropertyItem* seek = properties.getFirst();
    Plugin* plugin = NULL;
    while (seek!=NULL) {
      if (wildcmp(propertyId,seek->property->getId())) {
        plugin = seek->property->getPlugin();
        if (plugin != NULL) {
          success = plugin->setFilter(filter);
        } else {
          myLog.printf(LOG_ERR, " No plugin defined for property:'%s' (%s)!", seek->property->getId(), propertyId);
          return false;
        }
        if (!success) return false;
      }
      seek = properties.getNext(seek);
    }
  }
  return success;
};

void MyHomieNode::setPulginId(uint8_t id = 0) {
  _pluginId = id;
};

const char* MyHomieNode::getId() const {
  return nodeDef.id;
};
bool MyHomieNode::defaultNodeInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    myLog.printf(LOG_INFO,F("defaultNodeInputHandler %s/%s=%s (%s)"), homieNode->getId(), homieProperty->getId(), value.c_str(), homieDatatypes[homieProperty->getDef().datatype]);
    
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
    
    return false;
};

bool MyHomieNode::inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty){
  bool done = false;
  myLog.setAppName(homieNode->getId());
  if (_inputHandler!=NULL) {
      done = _inputHandler(range,value, homieNode, homieProperty);
      if (done) homieProperty->sendValue();
  }
  if (!done) {
    done = defaultNodeInputHandler(range,value, homieNode, homieProperty);
  }
  return done;
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

Plugin* MyHomieNode::getPropertyPlugin (const char* id) const {
  return properties.getById(id)->getPlugin();
};

uint8_t MyHomieNode::length() const {
  return properties.length;
};

HomieNodeDef MyHomieNode::getDef() {
  return nodeDef;
};

bool MyHomieNode::sendValue(MyHomieProperty* homieProperty) {
  return homieProperty->sendValue();
}

void MyHomieNode::setCustomSampleRate(uint16_t rate){
  _customSampleRate = rate;
}

bool MyHomieNode::readyToSample(unsigned long timebase) {
  if ((_pluginSampleRate>0 || _customSampleRate>0) && _nextSample<timebase) {
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
    bool result = true;
    myLog.printf(LOG_TRACE,F("  MyHomieNode::setValue(%s, (float) %.2f) %.2f %s"),id,value,getProperty(id)->getValue(), getProperty(id)->getId());
    getProperty(id)->setValue(value);
    if (plugin!=NULL) { // if node plugin present
      result = plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
    }
    if (result) homieNode->setProperty(id).send(String(value)); // send homie feedback if plugin returns true=acknowledge
    return result;
  }
  myLog.printf(LOG_ERR,F("  MyHomieNode::setValue(%s,(float) %.2f) node unknown"),id,value);
  return false;
}

bool MyHomieNode::setValue(const char* id, bool value) {
  if (getProperty(id)!=NULL) {
    myLog.printf(LOG_TRACE,F("  MyHomieNode::setValue(%s,(bool))"),id);
    getProperty(id)->setValue(value);
    if (plugin!=NULL && strcmp(plugin->id(),"default")!=0){
      myLog.printf(LOG_TRACE,F("  node:%s.set(%d)"),plugin->id(),properties.getIndex(id),getProperty(id)->getData());
      plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the node plugin to do it's bussiness
      homieNode->setProperty(id).send(String((value) ? "true" : "false")); // send homie feedback
      return true;
    } else {
      Plugin*  propertyPlugin = getProperty(id)->getPlugin();
      if (propertyPlugin!=NULL) {
        myLog.printf(LOG_TRACE,F("  property:%s.set(%.2f)"),propertyPlugin->id(), getProperty(id)->getData()->current);
        propertyPlugin->set(getProperty(id)->getData()); // trigger the property plugin to do it's bussiness
        return true;
      }
      myLog.printf(LOG_TRACE,F("   no plugin present!"),id);
      return false;
    }
  }
  myLog.printf(LOG_ERR,F("  MyHomieNode::setValue(%s,(bool) %d) node unknown"),id,value);
  return false;
}

bool MyHomieNode::setFactor(const char* id, float value) {
  if (getProperty(id)!=NULL) {
    myLog.printf(LOG_INFO,F("  MyHomieNode::setFactor('%s', %.2f)"),id,value);
    getProperty(id)->setFactor(value);
    if (plugin!=NULL) plugin->set(properties.getIndex(id),getProperty(id)->getData()); // trigger the plugin to do it's bussiness
    // no feedback as homie don't have scale factors
    return true;
  }
  myLog.printf(LOG_ERR,F("  MyHomieNode::setFactor(%s, %.2f) node unknown"),id,value);
  return false;
}
/*!
   @class  homie_node
   @brief   adds a property to a node
            by calling this mehtod a property is created defined by the `HomiePropertyDef` structure. 
            in addition a channel of a node plugin or a property plugin including the basic port value can be associated
    @param def      (HomiePropertyDef) {(string)id, (sting)name, (string)unit, (uint8_t) datatype, (bool) retained, (string) format, (bool) settable, (float) threshold, (uint16_t) sampleRate [s], (uint16_t) timeout [s], (uint8_t) oversampling}
    @param channel  (unit8_t) channel
   @returns pointer to homie Node
*/
MyHomieNode* MyHomieNode::addProperty(HomiePropertyDef def, uint8_t channel) {
  myLog.printf(LOG_INFO,F("  addProperty id=%s name=%s unit=%s datatype=%s format=%s"),def.id, def.name, def.unit,homieDatatypes[def.datatype],def.format);

  MyHomieProperty* homieProperty = properties.push(new MyHomieProperty(homieNode, def, channel));
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
/*!
   @class  homie_node
   @brief   adds a property to a node
            by calling this mehtod a property is created defined by the `HomiePropertyDef` structure. 
            in addition a channel of a node plugin or a property plugin including the basic port value can be associated
    @param def      (HomiePropertyDef) {(string)id, (sting)name, (string)unit, (uint8_t) datatype, (bool) retained, (string) format, (bool) settable, (float) threshold, (uint16_t) sampleRate [s], (uint16_t) timeout [s], (uint8_t) oversampling}
    @param pluginId optional (unit8_t) id of the plugin
    @param gpio     optional (unit8_t) gpio / port value
    @param defaultValue optional (float) default value during intialisation
   @returns pointer to homie Node
*/
MyHomieNode* MyHomieNode::addProperty(HomiePropertyDef homiePropertyDef, uint8_t pluginId, uint8_t gpio, float defaultValue) {
  addProperty(homiePropertyDef);
  myLog.printf(LOG_INFO,F("  addPlugin to '%s' plugin=%d GPIO%d"),homiePropertyDef.id, pluginId, gpio);

  MyHomieProperty* homieProperty = getProperty(homiePropertyDef.id);
  homieProperty->getData()->defaultValue=defaultValue;
  if (homieProperty->addPlugin(pluginId, gpio)) {
    _propertyPluginPresent = true;
  };
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
   @returns  next event in seconds and 0 if no event should be scheduled
*/
unsigned long MyHomieNode::getNextEvent(unsigned long timebase) {
  unsigned long nextEvent = ULONG_MAX;
  if (_customSampleRate>0) {
    nextEvent = _customSampleRate;
  } else {
    MyHomieProperty* homieProperty = NULL;
    for (int8_t i=0; i<properties.length; i++) {
      homieProperty = getProperty(i);
      if (homieProperty->getDef().sampleRate>0) {
        if (homieProperty->getData()->sampleTimeout-timebase < nextEvent) {
          nextEvent = getProperty(i)->getData()->sampleTimeout-timebase;
          myLog.printf(LOG_TRACE,F("  next sample event: %d"), nextEvent);
          _nextEventIndex = i;
        }
      } 
      if (getProperty(i)->getDef().timeout>0) {
        if (homieProperty->getData()->sendTimeout-timebase < nextEvent) {
          nextEvent = getProperty(i)->getData()->sendTimeout-timebase;
          myLog.printf(LOG_TRACE,F("  next send event: %d"), nextEvent);
          _nextEventIndex = i;
        }
      }
    }
  }
  if (nextEvent==ULONG_MAX) nextEvent = 0;
  return nextEvent;
}
/*!
   @brief   main node loop: read / sample / send
   @details   - if sample timer triggers take initiate read
              - then read all values
              - if send timer triggers send sample
    @param    timebase    timebase to calculate relative time (duration)
   @returns  next event in seconds and ULONG_MAX if no event should be scheduled
*/
unsigned long MyHomieNode::loop(unsigned long timebase) {
  bool eventOccurred = false;
  bool proceed = true;
  myLog.printf(LOG_TRACE,F(" Node %s loop start"),getId());
  if (_nextEvent < timebase) {
    myLog.setAppName("homieNode");
    MyHomieProperty* property = NULL;
    if (plugin!=NULL && plugin->checkStatus() && readyToSample(timebase)) {
      myLog.printf(LOG_DEBUG,F(" Node %s sample timer triggered"), getDef().id);
      property = getProperty(_nextEventIndex);
      // initiate read from node plugin
      if (property->readHandler(TASK_BEFORE_SAMPLE,this,property)) {
        if (_pluginId<100) myLog.printf(LOG_DEBUG,F("  Property %s/%s read form plugin %s"),getDef().id, property->getId(), (plugin!=NULL) ? plugin->id() : "NULL");
        plugin->read();
        eventOccurred = true;
        proceed = property->readHandler(TASK_AFTER_SAMPLE,this,property);
      }
      // get data from node plugin
      if (proceed) {
        for (int8_t i=0; i<properties.length; i++) {
          property = getProperty(i);
          if (property->readyToSample(timebase)) {
            if (property->readHandler(TASK_BEFORE_READ,this,property)) {
              if (property->getPlugin()!=NULL) {
                property->setReadValue(property->getPlugin()->get()); // get data from property plugin
              } else {
                property->setReadValue(plugin->get(property->channel())); // get data from node plugin
              }
              myLog.printf(LOG_TRACE,F("   sample %s = %.2f"), property->getId(), property->getReadValue());
              if (property->readHandler(TASK_AFTER_READ,this,property)) {
//                property->setValue(property->getReadValue());
                property->updateValue(property->getReadValue());
                eventOccurred = true;
              }
            }
          }
        }
      }
    }
    // send data from property plugins
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
  // if (eventOccurred)
  if (nextEvent>0) {
    myLog.printf(LOG_TRACE,F("  Next Node %s event in %.2fs (%s)"), getDef().id, (float) nextEvent / 1000, getProperty(_nextEventIndex)->getId());
  } else {
    myLog.printf(LOG_TRACE,F("  Node %s no next Event"), getDef().id);
    nextEvent = ULONG_MAX;
  }
  myLog.printf(LOG_TRACE,F(" Node %s loop end"),getId());
  return nextEvent;
}
/*!
   @brief   plugin loop: go through all properties plugins and call their loop function
    @param   void nothing
   @returns  nothing
*/
void MyHomieNode::pluginLoop() {
  // first node plugin
  if (plugin!=NULL && plugin->checkStatus()) {
    plugin->loop();
  }
  // second properties plugin
  if (_propertyPluginPresent) {
    MyHomieProperty* property = NULL;
    Plugin* plugin = NULL;
    for (int8_t i=0; i<properties.length; i++) {
      property = getProperty(i);
      plugin = property->getPlugin();
      if (plugin!=NULL) {
        // myLog.printf(LOG_TRACE,"Plugin Loop id:%s plugin:%s (%#x)",property->getId(),plugin->id(),plugin);
        if (plugin->checkStatus()) {
          plugin->loop();
        }
      }
    }
  }
}
