#ifndef HOMIE_PROPERTY_H__
#define HOMIE_PROPERTY_H__
#include "signalLED.h"
// #include "homieSyslog.h"
#include "../../include/datatypes.h"

class MyHomieNode;
class MyHomieProperty;
typedef bool (*InputHandler)(const HomieRange&, const String&, MyHomieNode*, MyHomieProperty*);
bool defaultPropertyInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);

/**
 * @brief MyHomieProperty Object
 * 
 */
class MyHomieProperty {
    const HomiePropertyDef* propertyDef = NULL;
    PropertyData propertyData;
    float* samples;
    InputHandler _inputHandler;
  public:
    MyHomieProperty(const HomiePropertyDef* def);
    const char* getId();
    const HomiePropertyDef* getDef() const;
    bool readyToSample();
    bool readyToSend();
    bool defaultPropertyInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    void setInputHandler(InputHandler inputHandler);
    bool inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    bool setValue (float value);
    bool setValue (bool value);
    bool setFactor (float value);
    float getValue ();
    PropertyData* getData ();
};

#endif