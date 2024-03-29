#include "utils.h"
#include "homieSyslog.h"

String toString(float value, uint8_t decimals)
{
  String sValue = String(value, decimals);
  sValue.trim();
  return sValue;
}


uint32_t minimum(uint32_t a, uint32_t b) {
  if (a < b) return a;
  return b;
}

uint32_t maximum(uint32_t a, uint32_t b) {
  if (a > b) return a;
  return b;
}
/*!
   @brief    get the nth part of a comma seperated enum list
    @param    listString  string with comma seperated values
    @param    index       index of string to return starting from 0
    @returns  result String - empty string is index is out of range
*/
String enumGetString(String listString, uint8_t index) {
  if (listString.length()==0) return ""; 
  int startPos = listString.indexOf(',');
  if (index==0) return listString.substring(0,startPos); // 
  if (startPos==-1) return listString; // no commas found: return probing string (?)
  int currentIndex = 1;
  //find start
  while (currentIndex!=index || startPos<0) { // loop until index found or end of string
    startPos = listString.indexOf(',',startPos+1);
    currentIndex++;
  };
  if (startPos<0) return ""; // listString has not enough entries
  //find end
  int endPos = listString.indexOf(',',startPos+1);
  if (endPos<0) endPos = listString.length(); // search @ the end of list
  return listString.substring(startPos+1,endPos);
}
/*!
   @brief    get the index of a comma seperated enum list
    @param    listString  String with comma seperated values
    @param    searchSting  String of witch index should be found
    @returns  index of the given String in list - -1 if search Sting is not included in list
*/
int enumGetIndex(String listString, const String& searchString){
  if (listString.length()==0) return -1; 
  if (listString.equals(searchString)) return 0;
  int startPos = 0;
  int endPos = listString.indexOf(',');
  int currentIndex = 0;
  while (!listString.substring(startPos,endPos).equals(searchString)) { // loop until index found or end of string
    startPos = endPos+1;
    if (startPos>=listString.length()) return -1;  // end of list reached
    endPos = listString.indexOf(',',startPos+1); // next member of list
    if (endPos<0) endPos = listString.length(); // last member of list
    currentIndex++;
  };
  return currentIndex;
}

/*!
   @brief    round a float value to a give number of decimal places with halfway cases
    @param    value   value to be rounded      
    @param    places  number of decimal places
    @returns  rounded value
*/
float roundTo(float value, uint8_t places){
  return round(value*(places*10.0))/(places*10.0);
};


/*!
   @brief    Matches a string against a wildcard string such as &quot;*.*&quot; or &quot;bl?h.*&quot; etc. This is good for file globbing or to match hostmasks.
    @author   Jack Handy - jakkhandy@hotmail.com
    @link     https://codeproject.com/Articles/1088/Wildcard-string-compare-globbing
    @param    wild  String including wildcards (? or *)
    @param    sting  String of to be checked against wildcard
    @returns  true if string matches the wildcard pattern
*/
int wildcmp(const char *wild, const char *string) {
  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

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

uint8_t scanI2C(const uint8_t addresses[], uint8_t address, uint8_t sdaPin, uint8_t sclPin){

  // Wire.pins(sdaPin, sclPin);
  Wire.begin();
  myLog.setAppName("I2C");
  if (address == 0) {
    myLog.printf(LOG_INFO,F("  Autodetecting I2C Device SDA:%d SCL:%d"),address,sdaPin,sclPin);
  } else {
    myLog.printf(LOG_INFO,F("  Searching for I2C Device @0x%X SDA:%d SCL:%d"),address,sdaPin,sclPin);
  }
  
  int count=0;
  uint8_t j = 0;
  uint8_t result=0;
  uint8_t sensorAddress=0;

  for (uint8_t i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);
    result=Wire.endTransmission();
    if (result == 4) {
      myLog.printf(LOG_ERR,F("   0x%X unknown error"),i);
    } else if (result == 0) {
      count++;
      if (address!=0) {
        if (i==address) {
          myLog.printf(LOG_DEBUG,F("   0x%X found specific I2C Device"),i);
          sensorAddress=i;
          break;
        }
      } else {
        j = 0;
        while (addresses[j]!=0) {
          if (i==addresses[j]) {
            myLog.printf(LOG_DEBUG,F("   0x%X autodetected I2C Device"),i);
            sensorAddress=i;
            break;
          } 
          j++;        
        }
      }
      if (sensorAddress==0) myLog.printf(LOG_DEBUG,F("   0x%X device ACK"),i);
    }
    delay(1);
  }

  if (count==0) {
    myLog.print(LOG_ERR,F("  no i2c devices found!"));
  } else {
    myLog.printf(LOG_DEBUG,F("  %d total i2c devices found!"),count);
  }
  return sensorAddress;
}
#endif

/**** sort arrays https://github.com/emilv/ArduinoSort/blob/master/ArduinoSort.h ****/

namespace ArduinoSort {
	template<typename AnyType> bool builtinLargerThan(AnyType first, AnyType second) {
		return first > second;
	}

	template<> bool builtinLargerThan(char* first, char* second) {
		return strcmp(first, second) > 0;
	}

	template<typename AnyType> void insertionSort(AnyType array[], size_t sizeOfArray, bool reverse, bool (*largerThan)(AnyType, AnyType)) {
		for (size_t i = 1; i < sizeOfArray; i++) {
			for (size_t j = i; j > 0 && (largerThan(array[j-1], array[j]) != reverse); j--) {
				AnyType tmp = array[j-1];
				array[j-1] = array[j];
				array[j] = tmp;
			}
		}
	}
}

template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray) {
	ArduinoSort::insertionSort(array, sizeOfArray, false, ArduinoSort::builtinLargerThan);
}

template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray) {
	ArduinoSort::insertionSort(array, sizeOfArray, true, ArduinoSort::builtinLargerThan);
}

template<typename AnyType> void sortArray(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType)) {
	ArduinoSort::insertionSort(array, sizeOfArray, false, largerThan);
}

template<typename AnyType> void sortArrayReverse(AnyType array[], size_t sizeOfArray, bool (*largerThan)(AnyType, AnyType)) {
	ArduinoSort::insertionSort(array, sizeOfArray, true, largerThan);
}
