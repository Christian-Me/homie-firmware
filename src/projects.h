#ifndef PROJECTS_H__
#define PROJECTS_H__

// select projects
// define your projects in plaformio.ini (build_flags = )

#if INITIAL_UPLOAD
    #include "initial-upload.h"
#endif

#if SENSOR_TESTBENCH
    #include "device-testbench.h"
#endif

#if SENSOR_BME680
    #include "device-BME680.h"
#endif

#if ACTUATOR_H801
    #include "device-H801-5chLED.h"
#endif

#endif