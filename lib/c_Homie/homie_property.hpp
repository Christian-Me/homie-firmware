#pragma once

// #include "signalLED.h"
#include <Homie.h>
// #include "homieSyslog.h"
#include "..\..\include\datatypes.h"

#define TASK_BEFORE_READ 1
#define TASK_ACTUAL_READ 2
#define TASK_AFTER_READ 3

class MyHomieNode;
class MyHomieProperty;
typedef bool (*InputHandler)(const HomieRange&, const String&, MyHomieNode*, MyHomieProperty*);
typedef bool (*ReadHandler)(uint8_t, MyHomieNode*, MyHomieProperty*);

/**
 * @brief MyHomieProperty Object
 * 
 */
class MyHomieProperty {
    const HomiePropertyDef* propertyDef = NULL;
    PropertyData propertyData;
    float* samples;
    InputHandler _inputHandler;
    ReadHandler _readHandler;
    float oversample(float sample);
  public:
    MyHomieProperty(const HomiePropertyDef* def);
    const char* getId();
    const HomiePropertyDef* getDef() const;
    bool readyToSample(unsigned long timebase);
    bool readyToSend(unsigned long timebase);
    bool inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    bool defaultPropertyInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    void setInputHandler(InputHandler inputHandler);
    bool readHandler(uint8_t task, MyHomieNode*homieNode, MyHomieProperty* homieProperty);
    bool defaultPropertyReadHandler(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    bool defaultReadHandler(uint8_t task, MyHomieNode*homieNode, MyHomieProperty* homieProperty);
    void setReadHandler(ReadHandler readHandler);
    bool setValue (float value);
    bool setValue (bool value);
    bool setFactor (float value);
    float getValue ();
    PropertyData* getData ();
};