#pragma once

#include <Homie.h>
#include "..\..\include\datatypes.h"

#define TASK_BEFORE_SAMPLE 1    // before data is read from hardware
#define TASK_AFTER_SAMPLE 2     // before data is read from hardware
#define TASK_BEFORE_READ 3      // before reading individual property
#define TASK_AFTER_READ 4       // after reading individual property
#define TASK_BEFORE_SEND 5      // befor sending to mqtt server
#define TASK_AFTER_SEND 6       // after sending to mqtt server

class MyHomieNode;
class MyHomieProperty;
class Plugin;
typedef bool (*InputHandler)(const HomieRange&, const String&, MyHomieNode*, MyHomieProperty*);
typedef bool (*ReadHandler)(uint8_t, MyHomieNode*, MyHomieProperty*);

/**
 * @brief MyHomieProperty Object
 * 
 */
class MyHomieProperty {
    HomiePropertyDef _propertyDef = {"","", "", DATATYPE_FLOAT, RETAINED, "0:1", NON_SETTABLE,0,0,0,0};
    const HomieNode *_parentNode = NULL;
    PropertyData propertyData;
    Plugin* _plugin = NULL;
    uint8_t _pluginId = 0;
    uint8_t _channel = 0;
    float* samples;
    InputHandler _inputHandler;
    ReadHandler _readHandler;
    float oversample(float sample);
  public:
    uint8_t hideLog = 0b11111111; // all logs visible
    MyHomieProperty(const HomieNode* parentNode, HomiePropertyDef def, uint8_t channel);
    const char* getId();
    const HomiePropertyDef getDef();
    bool addPlugin(uint8_t pluginId, uint8_t gpio);
    Plugin* getPlugin();
    uint8_t channel();
    bool readyToSample(unsigned long timebase);
    bool readyToSend(unsigned long timebase);
    bool sendValue();
    bool sendValue(float value);
    bool sendValue(bool value);
    bool sendValue(const char* string);
    bool inputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    bool hasInputHandler();
    bool defaultPropertyInputHandler(const HomieRange& range, const String& value, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    void setInputHandler(InputHandler inputHandler);
    bool readHandler(uint8_t task, MyHomieNode*homieNode, MyHomieProperty* homieProperty);
    bool defaultPropertyReadHandler(uint8_t task, MyHomieNode* homieNode, MyHomieProperty* homieProperty);
    bool defaultReadHandler(uint8_t task, MyHomieNode*homieNode, MyHomieProperty* homieProperty);
    void setReadHandler(ReadHandler readHandler);
    bool setReadValue (float value);
    bool updateValue (float value);
    bool setValue (float value);
    bool setValue (bool value);
    bool setFactor (float value);
    bool isTarget (const char* id, uint8_t start = 0);
    float getReadValue ();
    float getValue ();
    float defaultValue();
    PropertyData* getData ();
};