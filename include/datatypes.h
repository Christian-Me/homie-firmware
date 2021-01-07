#ifndef DATATYPES_H__
#define DATATYPES_H__
#include <Arduino.h>

typedef enum DataTypes {StringT,IntegerT,FloatT,BooleanT,EnumT,ColorT,DateTimeT,DurationT} DataTypes;

// sensor node
typedef struct {
    const char* id;
    const char* name;
    const char* unit;
    const char* datatype;
    const bool retained;
    const char* format;
    const float threshold;
    const uint16_t sampleRate;
    const uint16_t timeout;
    const uint16_t oversampling;
} DataPoint;

typedef struct {
    const char* id;
    const char* name;
    const char* type;
    const uint8_t datapoints;
    const DataPoint dataPoint[MAX_DATAPOINTS];
} SensorNode;

// actuator node
typedef struct {
    const char* id;
    const char* name;
    const char* unit;
    const char* datatype;
    const bool retained;
    const char* format;
    const uint8_t pin;
} Actuator;

typedef struct {
    const char* id;
    const char* name;
    const char* type;
    const uint8_t actuators;
    const Actuator actuator[MAX_ACTUATORS];
} ActuatorNode;

#endif