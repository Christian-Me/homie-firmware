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
/*!
   @brief    get the nth part of a comma seperated enum list
    @param    listString  string with comma seperated values
    @param    index       index of string to return starting from 0
    @returns  result String - empty string is index is out of range
*/
String enumGetString(String listString, uint8_t index);
/*!
   @brief    get the index of a comma seperated enum list
    @param    listString  String with comma seperated values
    @param    searchSting  String of witch index should be found
    @returns  index of the given String in list - -1 if search Sting is not included in list
*/
int enumGetIndex(String listString, const String& searchString);

#ifdef USE_I2C
// scan i2c bus for specific device
/*!
   @brief    scan i2c bus for specific decvice
    @param    addresses         pointer to array of i2c addresses 8 to 119
    @param    index             (optional) device index 0..max devices
    @param    sdaPin            (optional) GPIO Pin of data signal
    @param    sclPin            (optional) GPIO Pin of clock signal
    @returns  address of found device. 0 if no device is found on specific address
*/

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