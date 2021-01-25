#include "homie_property.h"
#include "homie_node.h"

MyHomieProperty::MyHomieProperty(const HomiePropertyDef* def) {
  propertyDef = def;
  propertyData.last=0;
  propertyData.current=0;
  propertyData.scale=1;
  propertyData.shift=0;
  propertyData.oversamplingSum=0;
  propertyData.toSample=propertyDef->oversampling;
  propertyData.sendTimeout= millis()+propertyDef->timeout*1000;
  propertyData.sampleTimeout= millis() + propertyDef->sampleRate * 1000;
  propertyData.initialSample= false;
  samples = new float[propertyDef->oversampling];
  _inputHandler = NULL;
};

const char* MyHomieProperty::getId() {
  return (propertyDef!=NULL) ? propertyDef->id : "";
};

const HomiePropertyDef* MyHomieProperty::getDef() const {
  return propertyDef;
};

bool MyHomieProperty::readyToSample() {
  if (propertyData.sampleTimeout<millis()) {
    propertyData.sampleTimeout= millis() + propertyDef->sampleRate * 1000;
    return true;
  }
  return false;
};

bool MyHomieProperty::readyToSend() {
  if (propertyData.sendTimeout<millis()) {
    propertyData.sendTimeout= millis()+propertyDef->timeout*1000;
    return true;
  }
  return false;
};

void MyHomieProperty::setInputHandler(InputHandler inputHandler) {
  _inputHandler = inputHandler;
};

bool MyHomieProperty::inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty){
  if (_inputHandler!=NULL) {
    return _inputHandler(range,value, homieNode, homieProperty);
  } else {
    defaultPropertyInputHandler(range,value, homieNode, homieProperty);
  }
  return false;
};

bool MyHomieProperty::setValue (float value) {
  triggerLED();
  propertyData.current = value;
  myLogf(LOG_TRACE,F("   MyHomieProperty::setValue %.2f : %.2f %p"), value, propertyData.current, &propertyData);
  return true;
};

bool MyHomieProperty::setValue (bool value) {
  myLogf(LOG_DEBUG,F("   MyHomieProperty::setValue %s %p"), value, (propertyData.current==1) ? "true" : "false", &propertyData);
  triggerLED();
  propertyData.current = (value) ? 1 : 0;
  return true;
};

bool MyHomieProperty::setFactor (float value) {
  triggerLED();
  propertyData.scale = value;
  myLogf(LOG_TRACE,F("   MyHomieProperty::setFactor %.2f : %.2f"), value, propertyData.scale);
  return true;
}
;
float MyHomieProperty::getValue () {
  myLogf(LOG_TRACE,F("   MyHomieProperty::getValue %.2f %p"), propertyData.current, &propertyData);
  return propertyData.current;
};

PropertyData* MyHomieProperty::getData () {
  return &propertyData;
};

bool MyHomieProperty::defaultPropertyInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty) {
    myLogf(LOG_INFO,F("defaultNodeHandler received %s"), value.c_str());
    bool _result = false;
    
    switch (homieProperty->getDef()->datatype) {
      case DATATYPE_FLOAT: {
        float _floatValue = value.toFloat();
        myLogf(LOG_INFO,F(" float value %s = %.2f"), value.c_str(), _floatValue);
        return homieNode->setValue(homieProperty->getDef()->id,_floatValue);
      };
      case DATATYPE_BOOLEAN: {
        bool _boolValue = (value.equalsIgnoreCase("true")) ? true : false;
        myLogf(LOG_INFO,F(" boolean value %s = %d"), value.c_str(), _boolValue);
        return homieNode->setValue(homieProperty->getDef()->id,_boolValue);
      };
    };
    
    return _result;
};