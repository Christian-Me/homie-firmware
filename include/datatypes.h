#ifndef DATATYPES_H__
#define DATATYPES_H__
#include <Arduino.h>
#include <Homie.h>
#define MAX_DATAPOINTS 3

#define SETTABLE true
#define NON_SETTABLE false

#define RETAINED true
#define NOT_RETAINED false

#define DATATYPE_STRING 1
#define DATATYPE_INTEGER 2
#define DATATYPE_FLOAT 3
#define DATATYPE_BOOLEAN 4
#define DATATYPE_ENUM 5
#define DATATYPE_COLOR 6
#define DATATYPE_DATETIME 7
#define DATATYPE_DURATION 8
/*
const char datatype0[] PROGMEM = "none";
const char datatype1[] PROGMEM = "string";
const char datatype2[] PROGMEM = "integer";
const char datatype3[] PROGMEM = "float";
const char datatype4[] PROGMEM = "boolean";
const char datatype5[] PROGMEM = "enum";
const char datatype6[] PROGMEM = "color";
const char datatype7[] PROGMEM = "datetime";
const char datatype8[] PROGMEM = "duration";
const char *const homieDatatypes[] PROGMEM = {datatype0, datatype1, datatype2, datatype3, datatype4, datatype5, datatype6, datatype7, datatype8};
*/

const char *const homieDatatypes[] = {"none","string","integer","float","boolean","enum","color","datetime","duration"};

/**
 * @brief Structure holding the core sensor data
 */
typedef struct {
    unsigned long sampleTimeout;    /*< datapoint timeout to sample*/
    unsigned long sendTimeout;      /*< datapoint timeout to send*/
    float current;                  /*< current calibrated reading */
    float last;                     /*< last reading sent */
    float scale;                    /*< value scale for calibration */
    float shift;                    /*< value shift for calibration */
    uint8_t toSample;               /*< number of samples to be averaged  */
    float oversamplingSum;          /*< number of samples to be averaged  */
    bool initialSample;             /*< initial sample sent  */
} PropertyData;

// classes

// class MyHomieProperty;
// class MyHomieNode;

// callback functions

// typedef bool (*InputHandler)(const HomieRange&, const String&, MyHomieNode*, MyHomieProperty*);

// sensor node
/**
 * @brief sensor datapoint configuration
 */
typedef struct {
    const char* id;                 /*< homie id */
    const char* name;               /*< homie name */
    const char* unit;               /*< homie unit */
    const uint8_t datatype;         /*< homie datatype */
    const bool retained;            /*< homie retained flag */
    const char* format;             /*< homie format*/
    const bool settable;            /*< homie settable SETTABLE or NON_SETTABLE*/
    const float threshold;          /*< minimum difference to trigger a data send */
    const uint16_t sampleRate;      /*< timeout between sampling */
    const uint16_t timeout;         /*< timeout to send data anyways (seconds)*/
    const uint8_t oversampling;     /*< number of values to oversample */
    const uint8_t gpio;             /*< number of GPIO channel */
} HomiePropertyDef;


typedef struct {
    const char* id;
    const char* name;
    const char* type;
    const int datapoints;
    const HomiePropertyDef dataPoint[MAX_DATAPOINTS];
} HomieNodeDef;

typedef struct {
    const char* id;
    const char* name;
} HomieDeviceDef;

#endif