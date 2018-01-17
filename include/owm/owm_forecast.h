/*============================================================================
 * libopenweathermap
 * owm_forecast.h
 * Copyright (c)2018 Kevin Boone, GPL v3.0
 * =========================================================================*/

#pragma once

#include <owm/owm_data.h>
#include <time.h>
#include <owm/owm_weather.h>

struct OwmForecast;
typedef struct _OwmForecast OwmForecast;

#ifdef __CPLUSPLUS
  extern "C" {
#endif

OwmForecast *owm_forecast_get (const char *app_id, const char *location_id, 
    char **error);

/** Cleans up memory reserved by the forecast object. */
void               owm_forecast_destroy (OwmForecast *self);

/** Get the number of forecast data points in the forecast list -- usually 40 */
int                owm_forecast_get_points (const OwmForecast *self);

/** Get a specific OwmWeather entry from the forecast. Entries start
 at zero, and continue to owm_forecast_get_points() - 1 -- usually 39 */
const OwmWeather  *owm_forecast_get_point (const OwmForecast *self, int n);


/** Given a time_t argument, extract a summary of conditions for the day in
 which it falls. Note that there are certain mathematical objections to
 trying to "average" a set of wind directions, but in practice it isn't
 usually a problem, as the prevailing wind direction tends to be similar
 during a day. We return maximum and minimum temperatures; average daily
 temperaturs are not reall the same as the average of these two values, but
 they are likely to be similar */
void               owm_forecast_get_daily_summary (const OwmForecast *forecast, 
                     time_t t, double *min_temp, double *max_temp, 
                     OwmConditions *modal_conditions, double *wind_direction, 
                     double *wind_speed, char **error);

/* Get the sunrise and sunset times from the forecast. Note that these only
 apply, strictly speaking, to the first day of the forecast */
void               owm_forecast_get_rise_set (OwmForecast *self, 
                     time_t *rise, time_t *set);

#ifdef __CPLUSPLUS
  } 
#endif


