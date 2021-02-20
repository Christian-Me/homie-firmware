#ifndef CALC_FUNCTIONS_H__
#define CALC_FUNCTIONS_H__

#include <Arduino.h>

/*!
   @brief    calculates the absolute Humidity as an approximation from table values
    @param  temperature temperature in °C
    @param  relativeHumidity relative humidity in %
    @returns  absolute Humidity in g/m³ 
*/
double absoluteHumidity(double temperature, double relativeHumidity) {
  return (
    (0.000002*pow(temperature,4.0))+
    (0.0002*pow(temperature,3.0))+
    (0.0095*pow(temperature,2.0))+
    (0.337*temperature)+4.9034) * relativeHumidity;
}
/*!
   @brief    calculates the drew point 
    @param  temperature temperature in °C
    @param  relativeHumidity relative humidity in %
    @returns  drew point in °C 
*/
double drewPoint(double temperature, double relativeHumidity) {
	double drewPoint;
	drewPoint = 243.04 * ( log(relativeHumidity/100) + (17.625*temperature)/(243.04+temperature));
	drewPoint /= 17.625- log(relativeHumidity/100) - (17.625*temperature)/(243.04+temperature);
  return drewPoint;
}
/*!
   @brief    compensate realtive Humidity in relation between measured and compensated temperature
    @param  temperatureMeasured temperature in °C
    @param  temperatureMeasured temperature in °C
    @param  relativeHumidity relative humidity in %
    @returns  compensated relative humidity in %
*/
double compensatedHumidity(double temperatureMeasured, double temperatureCompensated, double relativeHumidity) {
	double new_relativeHumidity;
  double _drewPoint = drewPoint(temperatureMeasured, relativeHumidity);

	// Calculate new relativeHumidity
	new_relativeHumidity = 100 * exp((17.625*_drewPoint) / (243.04+_drewPoint)) / exp((17.625*temperatureCompensated)/(243.04+temperatureCompensated));

	return new_relativeHumidity;
}

#endif
