#ifndef DATATYPES_H__
#define DATATYPES_H__
#include <Arduino.h>
#include <Homie.h>


/**
 * @brief Structure holding the core sensor data
 */
typedef struct {
    unsigned long sampleTimeout;    /**< datapoint timeout to sample*/
    unsigned long sendTimeout;      /**< datapoint timeout to send*/
    float current;                  /**< current calibrated reading */
    float last;                     /**< last reading sent */
    float scale;                    /**< value scale for calibration */
    float shift;                    /**< value shift for calibration */
    uint8_t toSample;               /**< number of samples to be averaged  */
    float oversamplingSum;          /**< number of samples to be averaged  */
    bool initialSample;             /**< initial sample sent  */
} SenstorData;

/**
 * @brief Structure holding the core sensor configuration
 */
typedef struct {
    uint8_t sensorIndex;    /**< index number of the sensor (1..n) */
    HomieNode *homieNode;   /**< Homie Node */
    bool isInitialized;     /**< Flag true if sensor is initialized */
    bool force;             /**< Flag to force sending data */
} SensorCore;

// callback functions

typedef void (*Callback)(char*,SenstorData);

// sensor node
/**
 * @brief sensor datapoint configuration
 */
typedef struct {
    const char* id;                 /**< homie id */
    const char* name;               /**< homie name */
    const char* unit;               /**< homie unit */
    const char* datatype;           /**< homie datatype */
    const bool retained;            /**< homie retained flag */
    const char* format;             /**< homie format*/
    const float threshold;          /**< minimum difference to trigger a data send */
    const uint16_t sampleRate;      /**< timeout between sampling */
    const uint16_t timeout;         /**< timeout to send data anyways (seconds)*/
    const uint8_t oversampling;     /**< number of values to oversample */
    Callback beforeRead;            /**< called before reading the sensor */
    Callback afterRead;             /**< called after reading the sensor */
    Callback processValue;          /**< called after sensor was read and before data will be processed */
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