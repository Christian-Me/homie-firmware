#ifndef DATATYPES_H__
#define DATATYPES_H__
#include <Arduino.h>
// #include <Homie.h>

#define VIRTUAL_ID 0

// sensor Plugins
#define BH1750_ID 1
#define BME280_ID 2
#define BME680_ID 3
#define ADS1115_ID 4
#define HDC1080_ID 5

#define DUMMY_ID 99

// actuator Plugins
// controling the output like LEDs, motors, relays, DACs 
#define PWM_ID 101
#define aGPIO_ID 102

// input Plugins
// controling inputs like Buttons, Pot
#define I_GPIO_ID 201
#define I_ANALOG_ID 202

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
 * @brief Structure holding the datapoint information
 */
typedef struct {
    unsigned long sampleTimeout;    /*< datapoint timeout to sample*/
    unsigned long sendTimeout;      /*< datapoint timeout to send*/
    float current;                  /*< current calibrated reading */
    float read;                     /*< last value raw read form sensor */
    float last;                     /*< last reading sent out*/
    float target;                   /*< target value */
    float temporary;                /*< temporary value */
    float scale;                    /*< value scale for calibration */
    float shift;                    /*< value shift for calibration */
    uint8_t samples;                /*< number of samples in buffer  */
    uint8_t samplePointer;          /*< pointer to next sample */
    bool initialSample;             /*< initial sample sent  */
} PropertyData;

/**
 * @brief homie property configuration, scheduler and sampling definitions
 */
struct HomiePropertyDef {
    const char* id;             //!< homie property id
    const char* name;           //!< homie $name
    const char* unit;           //!< homie $unit
    uint8_t datatype;           //!< homie $datatype
    bool retained;              //!< homie $retained flag
    const char* format;         //!< homie $format
    bool settable;              //!< homie $settable SETTABLE or NON_SETTABLE
    float threshold;            //!< minimum difference to trigger a data send
    uint16_t sampleRate;        //!< timeout between sampling
    uint16_t timeout;           //!< timeout to send data anyways (seconds)
    uint8_t oversampling;       //!< number of values to oversample
};
/**
 * @brief homie node configuration
 */
struct HomieNodeDef {
    const char* id;              //!< homie node id
    const char* name;            //!< homie $name
    const char* type;            //!< homie $type
    uint8_t io;                  //!< GPIO, I2C Addres or CS Pin
};
/**
 * @brief homie device configuration
 */
struct HomieDeviceDef {
    const char* id;         //!< homie device id
    const char* name;       //!< homie $name
};

#endif