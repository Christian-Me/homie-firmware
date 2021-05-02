#ifndef UTILS_H__
#define UTILS_H__
#include <Arduino.h>
#ifdef USE_I2C
  #include <Wire.h>
#endif
//#include "config.h"
//#include <homie.h>
//#include "homieSyslog.h"

template <typename T, size_t N>
char ( &_ArraySizeHelper( T (&array)[N] ))[N];

#define countof( array ) (sizeof( _ArraySizeHelper( array ) ))

String toString(float value, uint8_t decimals);
uint32_t minimum(uint32_t a, uint32_t b);
uint32_t maximum(uint32_t a, uint32_t b);
String enumGetString(String listString, uint8_t index);
int enumGetIndex(String listString, const String& searchString);
int wildcmp(const char *wild, const char *string);
float roundTo(float value, uint8_t places);

#ifdef USE_I2C
uint8_t scanI2C(const uint8_t addresses[], uint8_t address, uint8_t sdaPin = 4, uint8_t sclPin=5);
#endif
/**** sort arrays https://github.com/emilv/ArduinoSort/blob/master/ArduinoSort.h ****/

// Sort an array
template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray);

// Sort in reverse
template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray);

// Sort an array with custom comparison function
template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType));

// Sort in reverse with custom comparison function
template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType));

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _b : _a; })

#endif