

#ifdef S_BH1750
  #include "s_BH1750.h"
  s_BH1750 BH1750(1);
#endif

bool sensorInit(uint8_t sensorId) {
  switch (sensorId) {
    #ifdef S_BH1750
      case BH1750_ID: 
        BH1750.init();
        break;
    #endif
  }
  return true;
}