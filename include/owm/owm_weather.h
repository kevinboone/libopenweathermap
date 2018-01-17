/*============================================================================
  owm_weather.h
  Copyright (c)2017 Kevin Boone, GPL v3.0
============================================================================*/

#pragma once

#include <stdint.h>
#include <time.h>

struct _OwmWeather;
typedef struct _OwmWeather OwmWeather;

typedef int OwmPrecipitation;
typedef int OwmSymbol;
typedef int OwmConditions;


/* Validity codes -- which properties in the OwmWeather object 
 * may assumed to be have been supplied by the provider */
#define OWM_VALID_CONDITIONS     0x00000001
#define OWM_VALID_TEMP           0x00000002
#define OWM_VALID_START          0x00000004
#define OWM_VALID_END            0x00000008
#define OWM_VALID_PRECIPITATION  0x00000010
#define OWM_VALID_WIND_DIRECTION 0x00000020
#define OWM_VALID_WIND_SPEED     0x00000040
#define OWM_VALID_PRESSURE       0x00000080
#define OWM_VALID_HUMIDITY       0x00000100
#define OWM_VALID_CLOUD_COVER    0x00000200


/* Precipitation types */
#define OWM_PRECIP_NONE 0
#define OWM_PRECIP_DRIZZLE 1 
#define OWM_PRECIP_RAIN 2 
#define OWM_PRECIP_SLEET 3
#define OWM_PRECIP_SNOW 4
#define OWM_PRECIP_GRAUPEL 5
#define OWM_PRECIP_HAIL 6


/* Weather condition codes */
#define OWM_COND_THUNDERSTORMS_LIGHT_RAIN       200
#define OWM_COND_THUNDERSTORMS_RAIN             201
#define OWM_COND_THUNDERSTORMS_HEAVY_RAIN       202
#define OWM_COND_LIGHT_THUNDERSTORMS            210
#define OWM_COND_THUNDERSTORM                   211
#define OWM_COND_HEAVY_THUNDERSTORMS            212
#define OWM_COND_RAGGED_THUNDERSTORMS           221
#define OWM_COND_THUNDERSTORSMS_LIGHT_DRIZZLE   230
#define OWM_COND_THUNDERSTORSMS_DRIZZLE         231
#define OWM_COND_THUNDERSTORSMS_HEAVYDRIZZLE    232
#define OWM_COND_LIGHT_DRIZZLE                  300
#define OWM_COND_DRIZZLE                        301
#define OWM_COND_HEAVY_DRIZZLE                  302
#define OWM_COND_LIGHT_DRIZZLE_RAIN             310
#define OWM_COND_DRIZZLE_RAIN                   311
#define OWM_COND_HEAVY_DRIZZLE_RAIN             312
#define OWM_COND_DRIZZLE_RAIN_SHOWERS           313
#define OWM_COND_HEAVY_DRIZZLE_RAIN_SHOWERS     314
#define OWM_COND_DRIZZLE_SHOWERS                321
#define OWM_COND_LIGHT_RAIN                     500
#define OWM_COND_MODERATE_RAIN                  501
#define OWM_COND_HEAVY_RAIN                     502
#define OWM_COND_VERY_HEAVY_RAIN                503
#define OWM_COND_TORRENTIAL_RAIN                504
#define OWM_COND_FREEZING_RAIN                  511
#define OWM_COND_LIGHT_RAIN_SHOWERS             520
#define OWM_COND_RAIN_SHOWERS                   521
#define OWM_COND_HEAVY_RAIN_SHOWERS             522
#define OWM_COND_RAGGED_RAIN_SHOWERS            531
#define OWM_COND_LIGHT_SNOW                     600
#define OWM_COND_SNOW                           601
#define OWM_COND_HEAVY_SNOW                     602
#define OWM_COND_SLEET                          611
#define OWM_COND_SLEET_SHOWERS                  612
#define OWM_COND_LIGHT_RAIN_SNOW                615
#define OWM_COND_RAIN_SNOW                      616
#define OWM_COND_LIGHT_SNOW_SHOWERS             620
#define OWM_COND_SNOW_SHOWERS                   621
#define OWM_COND_HEAVY_SNOW_SHOWERS             622
#define OWM_COND_HEAVY_SNOW_SHOWERS             622
#define OWM_COND_MIST                           701
#define OWM_COND_SMOKE                          711
#define OWM_COND_HAZE                           721
#define OWM_COND_SAND_DUST_SWIRLS               731
#define OWM_COND_FOG                            741
#define OWM_COND_SAND                           751
#define OWM_COND_DUST                           761
#define OWM_COND_VOLCANIC_ASH                   762
#define OWM_COND_SQUALLS                        771
#define OWM_COND_TORNADO1                       781
#define OWM_COND_CLEAR_SKY                      800 
#define OWM_COND_LIGHT_CLOUD                    801
#define OWM_COND_SCATTERED_CLOUD                802
#define OWM_COND_BROKEN_CLOUD                   803
#define OWM_COND_OVERCAST                       804
#define OWM_COND_TORNADO2                       900
#define OWM_COND_TROPICAL_STORM                 901
#define OWM_COND_HURRICANE1                     902
#define OWM_COND_COLD                           903
#define OWM_COND_HOT                            904
#define OWM_COND_WINDY                          905
#define OWM_COND_HAIL                           906
#define OWM_COND_CALM                           951
#define OWM_COND_LIGHT_BREEZE                   952
#define OWM_COND_GENTLE_BREEZE                  953
#define OWM_COND_MODERATE_BREEZE                954
#define OWM_COND_FRESH_BREEZE                   955
#define OWM_COND_STRONG_BREEZE                  956
#define OWM_COND_GALE                           958
#define OWM_COND_SEVERE_GALE                    959
#define OWM_COND_STORM                          960
#define OWM_COND_VIOLENT_STORM                  961
#define OWM_COND_HURRICANE2                     962

// MAX_CONDS is a value that can be used to size an array that holds
//  a count of instances of condition codes. However we cannot guarantee
//  that OWM will never use a cond code > MAX_CONDS, and applications
//  should check for this
#define OWM_MAX_CONDS 963


#ifdef __CPLUSPLUS
  extern "C" {
#endif

OwmWeather   *owm_weather_create (void);
void          owm_weather_destroy (OwmWeather *self);

int           owm_weather_get_valid (const OwmWeather *self);
double        owm_weather_get_temperature (const OwmWeather *self); //celcis
void          owm_weather_set_temperature (OwmWeather *self, double temp); //celcius
time_t        owm_weather_get_start_time (const OwmWeather *self);
void          owm_weather_set_start_time (OwmWeather *self, time_t t);
time_t        owm_weather_get_end_time (const OwmWeather *self);
void          owm_weather_set_end_time (OwmWeather *self, time_t t);
OwmPrecipitation owm_weather_get_precipitation (const OwmWeather *self);
void          owm_weather_set_precipitation (OwmWeather *self, 
                 OwmPrecipitation precip);
const char   *owm_weather_get_precipitation_name (const OwmWeather *self);
void          owm_weather_set_conditions (OwmWeather *self, 
                 OwmConditions precip);
OwmConditions owm_weather_get_conditions (const OwmWeather *self);
const char   *owm_weather_get_conditions_string (const OwmWeather *self);
const char   *owm_weather_conditions_to_string (OwmConditions conditions);
double        owm_weather_get_wind_direction (const OwmWeather *self);
void          owm_weather_set_wind_direction (OwmWeather *self, double wind);
double        owm_weather_get_wind_speed (const OwmWeather *self); //MPH
void          owm_weather_set_wind_speed(OwmWeather *self, double wind); //MPH
double        owm_weather_get_pressure (const OwmWeather *self); //mbar
void          owm_weather_set_pressure (OwmWeather *self, double wind); //mbar
double        owm_weather_get_humidity (const OwmWeather *self); //%
void          owm_weather_set_humidity (OwmWeather *self, double humidity); //%
double        owm_weather_get_cloud_cover (const OwmWeather *self); //%
void          owm_weather_set_cloud_cover (OwmWeather *self, double cloud_cover); //%
const char   *owm_weather_wind_direction_to_string (double wind_direction);
const char   *owm_weather_get_wind_direction_string (const OwmWeather *self);

#ifdef __CPLUSPLUS
 }
#endif



