#ifndef PROJECTS_H__
#define PROJECTS_H__
#include "c_homie.h"
#include "plugins.h"

// select projects
// define your projects in plaformio.ini (build_flags = )

#if INITIAL_UPLOAD
    #include "initial-upload.h"
#endif

#if SENSOR_BME680
    #include "device-BME680.h"
#endif

#if H801_LED_DIMMER
    #include "device-H801-5chLED.h"
#endif

#if PLANT_CONTROL
    #include "device-PLANT-control.h"
#endif

#if GREENHOUSE
    #include "device-GREENHOUSE.h"
#endif

#if MIFRA_2RELAY_HLW8012
    #include "device-MIFRA_2RELAY_HLW8012.h"
#endif

#if LED_6ch10W
    #include "device-LED_6ch10W.h"
#endif

#if HOMIE_FIRMWARE_LED_1ch10W
    #include "HOMIE_FIRMWARE_LED_1ch10W.h"
#endif
#endif