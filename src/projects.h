#ifndef PROJECTS_H__
#define PROJECTS_H__

// select projects

#define CURRENT_PROJECT ADS1115_TEST

#if CURRENT_PROJECT==ADS1115_TEST
    #include "ADS1115.h"
#endif

#endif