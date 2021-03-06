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
#endif