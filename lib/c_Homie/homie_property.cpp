#include "homie_property.hpp"
#include "homie_node.hpp"
#include <signalLED.h>
#include <utils.h>
#include <homie_device.hpp>

#ifdef A_PWM
  #include <a_pwm_dimmer.h>
#endif
#ifdef A_GPIO
  #include <a_gpio.h>
#endif
#ifdef I_GPIO
  #include <i_gpio.h>
#endif

const char taskString1[] PROGMEM = "before sample";
const char taskString2[] PROGMEM = "after sample";
const char taskString3[] PROGMEM = "before read";
const char taskString4[] PROGMEM = "after read";
const char taskString5[] PROGMEM = "before send";
const char taskString6[] PROGMEM = "after send";
const char *const taskString[] PROGMEM = {taskString1, taskString2, taskString3, taskString4, taskString5, taskString6};

bool MyHomieProperty::addPlugin(uint8_t pluginId, uint8_t gpio) {
  myLog.printf(LOG_DEBUG,F("   add plugin for property %s (pluginId:%d) GPIO"),getDef().id, pluginId, gpio);
  switch (pluginId) {
    #ifdef A_PWM
      case PWM_ID: {
        _plugin= new a_PWMdimmer();
        break; }
    #endif
    #ifdef A_GPIO
      case aGPIO_ID: {
        _plugin= new a_GPIO();
        break; }
    #endif
    #ifdef I_GPIO
      case iGPIO_ID: {
        _plugin= new i_GPIO();
        break; }
    #endif
    default: {
      _plugin = new Plugin(); }
  }
  if (_plugin != NULL) {
    myLog.printf(LOG_INFO,F("   registered plugin '%s' for property '%s' (pluginId:%d %#x)"), _plugin->id(), getDef().id, pluginId, _plugin);
    _plugin->init(this, gpio);
    return _plugin->checkStatus();
  } else {
    myLog.printf(LOG_ERR,F("   error initialisation of plugin '%s' for property '%s' (pluginId:%d)"), _plugin->id(), getDef().id, pluginId);
  }
  return false;
}

Plugin* MyHomieProperty::getPlugin() {
  return _plugin;
}

MyHomieProperty::MyHomieProperty(const HomieNode* parentNode, HomiePropertyDef def, uint8_t channel) {
  _parentNode = parentNode;
  _propertyDef = def;
  _channel = channel;
  propertyData.last=0;
  propertyData.current=0;
  propertyData.scale=1;
  propertyData.shift=0;
  propertyData.samples=0;
  propertyData.samplePointer=0;
  propertyData.target=0;
  propertyData.temporary=0;
  propertyData.sendTimeout=0; // millis()+_propertyDef.timeout*1000;
  propertyData.sampleTimeout=0; // millis() + _propertyDef.sampleRate * 1000;
  propertyData.initialSample= false;
  if (_propertyDef.oversampling>0) {
    samples = new float[_propertyDef.oversampling];
    for (uint8_t i = 0; i<_propertyDef.oversampling; i++) samples[i]=0;
  }
  _inputHandler = NULL;
  _readHandler = NULL;
};

const char* MyHomieProperty::getId() {
  return _propertyDef.id;
};

uint8_t MyHomieProperty::channel() {
  return _channel;
}

const HomiePropertyDef MyHomieProperty::getDef() {
  return _propertyDef;
};

bool MyHomieProperty::readyToSample(unsigned long timebase) {
  myLog.printf(LOG_TRACE,F("   %s ready to sample? %dms/%dms d=%dms"), _propertyDef.id, propertyData.sampleTimeout, timebase, propertyData.sampleTimeout-timebase);
  if (_propertyDef.sampleRate>0 && propertyData.sampleTimeout<timebase) {
    propertyData.sampleTimeout= timebase + _propertyDef.sampleRate * 1000;
    return true;
  }
  return false;
};

bool MyHomieProperty::readyToSend(unsigned long timebase) {
  myLog.printf(LOG_TRACE,F("   %s/%s ready to send? current:%.2f last:%.2f"),_parentNode->getId(), _propertyDef.id, propertyData.current, propertyData.last);
  if (_propertyDef.timeout>0 && propertyData.sendTimeout<millis()) {
    myLog.printf(LOG_DEBUG,F("   %s/%s is ready to send by timer"),_parentNode->getId() ,_propertyDef.id);
    propertyData.sendTimeout= timebase + _propertyDef.timeout*1000;
    return true;
  }
  if (fabs(propertyData.current-propertyData.last)>=_propertyDef.threshold) {
    myLog.printf(LOG_DEBUG,F("   %s/%s is ready to send by threshold %.2f>%.2f"),_parentNode->getId() ,_propertyDef.id, fabs(propertyData.current-propertyData.last), _propertyDef.threshold);
    propertyData.sendTimeout= timebase + _propertyDef.timeout*1000;
    return true;
  }
  return false;
};

bool MyHomieProperty::sendValue() {
  float valueFloat = 0;
  switch (_propertyDef.datatype) {
    case DATATYPE_INTEGER: {
      if (!_propertyDef.retained || (_propertyDef.retained && propertyData.last!=propertyData.current)) {
        myLog.printf(LOG_INFO,F("  %s/%s::sendValue((int) %d)"),_parentNode->getId(),getId(), (int) floor(propertyData.current));
        _parentNode->setProperty(getId()).send(String(floor(propertyData.current))); // send homie feedback
        propertyData.last= propertyData.current;
        signalLED.trigger();
        return true;
      }
      break;
    }
    case DATATYPE_FLOAT: {
      if (!_propertyDef.retained || (_propertyDef.retained && propertyData.last!=propertyData.current)) {
        myLog.printf(LOG_INFO,F("  %s/%s::sendValue((float) %.2f)"),_parentNode->getId(),getId(), propertyData.current);
        _parentNode->setProperty(getId()).send(String(propertyData.current)); // send homie feedback
        propertyData.last= propertyData.current;
        signalLED.trigger();
        return true;
      }
      break;
    }
    case DATATYPE_BOOLEAN: {
      valueFloat = propertyData.current;
      if (valueFloat == 0) {
        myLog.printf(LOG_INFO,F("  %s/%s::sendValue((bool) 'false')"),_parentNode->getId(), getId());
        _parentNode->setProperty(getId()).send(String("false")); // send homie feedback
      } else {
        myLog.printf(LOG_INFO,F("  %s/%s::sendValue((bool) 'true')"),_parentNode->getId(), getId());
        _parentNode->setProperty(getId()).send(String("true")); // send homie feedback
      }
      propertyData.last= propertyData.current;
      signalLED.trigger();
      return true;
    }
    case DATATYPE_ENUM: {
      String string = enumGetString(String(_propertyDef.format), propertyData.current);
      myLog.printf(LOG_INFO,F("  %s/%s::sendValue((enum) &d=%s)"),_parentNode->getId(), getId(), string.c_str());
      _parentNode->setProperty(getId()).send(string); // send homie feedback 
      propertyData.last = propertyData.current;     
    }
  }
  return false;
}

bool MyHomieProperty::sendValue(float value) {
  propertyData.current = value;
  return sendValue();
}

bool MyHomieProperty::sendValue(bool value) {
  propertyData.current = (value) ? 1 : 0;
  return sendValue();
}

bool MyHomieProperty::sendValue(const char* string) {
  _parentNode->setProperty(getId()).send(String(string));
  return true;
}

void MyHomieProperty::setInputHandler(InputHandler inputHandler) {
  _inputHandler = inputHandler;
};

void MyHomieProperty::setReadHandler(ReadHandler readHandler) {
  _readHandler = readHandler;
};

bool MyHomieProperty::inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty){
  if (_inputHandler!=NULL) {
    myLog.printf(LOG_TRACE,F("  MyHomieProperty::inputHandler custom: 0x%b"),_inputHandler);
    return _inputHandler(range,value, homieNode, homieProperty);
  } else {
    myLog.printf(LOG_TRACE,F("  MyHomieProperty::inputHandler default: 0x%b"),_inputHandler);
    return defaultPropertyInputHandler(range,value, homieNode, homieProperty);
  }
  return false;
};

bool MyHomieProperty::hasInputHandler(){
  return (_inputHandler!=NULL);
};

bool MyHomieProperty::readHandler(uint8_t task, MyHomieNode*homieNode, MyHomieProperty* homieProperty){
  bool result = false;
  if (_readHandler!=NULL) {
    myLog.printf(LOG_TRACE,F(" custom read handler %s/%s Task: '%s'"),homieNode->getDef().id, homieProperty->getDef().id, taskString[task-1]);
    result = _readHandler(task, homieNode, homieProperty);
  } else {
    myLog.printf(LOG_TRACE,F(" default read handler %s/%s Task: '%s'"),homieNode->getDef().id, homieProperty->getDef().id, taskString[task-1]);
    result = defaultPropertyReadHandler(task, homieNode, homieProperty);
  }
  return result;
};

/*!
   @brief  add a value to the oversampling array and return the average
    @param  sample   sample value
    @returns    average of samples
*/
float MyHomieProperty::oversample(float sample){
  samples[propertyData.samplePointer]=sample;
  propertyData.samplePointer++;
  if (propertyData.samplePointer>=_propertyDef.oversampling){
    propertyData.samplePointer=0;
  }
  if (propertyData.samples<_propertyDef.oversampling) {
    propertyData.samples++;
  }
  float averageSum = 0;
  for (uint8_t i=0; i<propertyData.samples; i++) {
    averageSum += samples[i];
  }
  myLog.printf(LOG_TRACE,F("   sampled %d/%d values %.2f~%.2f"),propertyData.samples,_propertyDef.oversampling,sample,averageSum/propertyData.samples);
  return averageSum/propertyData.samples;
}

bool MyHomieProperty::updateValue (float value) {
  bool result = true;
   if (_propertyDef.oversampling>0) {
    propertyData.current = oversample(value);
  } else {
    propertyData.current = value;
  }
  if (propertyData.last != value) myLog.printf(LOG_TRACE, F("   %s/%s updateValue(float) %.2f : %.2f"), _parentNode->getId(), getId(), value, propertyData.current);
  return result;
};

bool MyHomieProperty::setValue (float value) {
  bool result = updateValue(value);
  if (result && _plugin!=NULL) { // invoke property plugin
    result = _plugin->set(getData());
    if (propertyData.last != value) myLog.printf(LOG_TRACE, F("   %s/%s setValue(float) %.2f : %.2f"), _parentNode->getId(), getId(), value, propertyData.current);
  }
  return result;
};

bool MyHomieProperty::setReadValue (float value) {
  propertyData.read = value;
  return true;
};

bool MyHomieProperty::setValue (bool value) {
  propertyData.current = (value) ? (float)1 : (float)0;
  myLog.printf(LOG_TRACE,F("  %s/%s setValue(bool) %d : %.2f"), _parentNode->getId(), getId(), value, propertyData.current);
  return true;
};

bool MyHomieProperty::setFactor (float value) {
  propertyData.scale = value;
  myLog.printf(LOG_TRACE,F("   %s/%s setFactor(float) %.2f : %.2f"), _parentNode->getId(), getId(), value, propertyData.scale);
  return true;
};

float MyHomieProperty::getReadValue () {
  return propertyData.read;
};

float MyHomieProperty::getValue () {
  myLog.printf(LOG_TRACE,F("   %s/%s getValue %.2f %p"), _parentNode->getId(), getId(), propertyData.current, &propertyData);
  return propertyData.current;
};

PropertyData* MyHomieProperty::getData () {
  return &propertyData;
};

float MyHomieProperty::defaultValue () {
  return propertyData.defaultValue;
};

bool MyHomieProperty::defaultPropertyInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool _result = false;
    
    switch (homieProperty->getDef().datatype) {
      case DATATYPE_FLOAT: {
        float _floatValue = value.toFloat();
        myLog.printf(LOG_INFO,F("defaultPropertyHandler %s/%s=(float)%.2f"), homieNode->getId(), homieProperty->getId(), _floatValue);
        return homieNode->setValue(homieProperty->getDef().id,_floatValue);
      };
      case DATATYPE_BOOLEAN: {
        bool _boolValue = (value.equalsIgnoreCase("true")) ? true : false;
        myLog.printf(LOG_INFO,F("defaultPropertyHandler %s/%s=(bool)%d"), homieNode->getId(), homieProperty->getId(), _boolValue);
        return homieNode->setValue(homieProperty->getDef().id,_boolValue);
      };
      case DATATYPE_STRING: {
        myLog.printf(LOG_INFO,F("defaultPropertyHandler %s/%s=(String)'%s'"), homieNode->getId(), homieProperty->getId(), value.c_str());
        if (_plugin!=NULL) {
          myLog.printf(LOG_DEBUG,F(" invoking plugin id %s"), _plugin->id());
          return _plugin->set(value); // pass string directly to plugin
        } else {
          return false;
        }
      };
    };
    
    return _result;
};

bool MyHomieProperty::defaultPropertyReadHandler(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    bool result = true;
    switch (task) {
      case TASK_BEFORE_SAMPLE: {
//        myLog.print(LOG_TRACE,F("   default before sample task"));
        break;
      };
      case TASK_AFTER_SAMPLE: {
//        myLog.print(LOG_TRACE,F("   default actual sample task"));
        break;
      };
      case TASK_BEFORE_READ: {
//        myLog.print(LOG_TRACE,F("   default before read task"));
        break;
      };
      case TASK_AFTER_READ: {
//        myLog.print(LOG_TRACE,F("   default actual read task"));
        break;
      };
      case TASK_BEFORE_SEND: {
//        myLog.print(LOG_TRACE,F("   default before send task"));
        break;
      };
      case TASK_AFTER_SEND: {
//        myLog.print(LOG_TRACE,F("   default after send task"));
        break;
      };
    };
    
    return result;
};
/*!
   @brief    check if property has matches a specific id
    @param    id    (const char*) id to check
    @param    start (uint8_t)(optional) compare from position
   @returns  true if property has this id
*/
bool MyHomieProperty::isTarget (const char* id, uint8_t start) {
  return (strcmp(getId() + start,id)==0);
};
