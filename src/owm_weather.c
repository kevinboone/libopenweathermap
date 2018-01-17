/*============================================================================
 * libopenweathermap
 * owm_weather.c
 * Copyright (c)2018 Kevin Boone, GPL v3.0
 * =========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <owm/owm_defs.h>
#include <owm/owm_config.h>
#include <owm/owm_data.h>
#include <owm/owm_weather.h>
#include "sxmlc.h"


/*============================================================================
 * OwmWeather opaque struct 
 * =========================================================================*/
struct _OwmWeather
  {
  int valid;
  time_t start_time;
  time_t end_time;
  OwmConditions conditions;
  double temp;
  double wind_direction;
  double wind_speed;
  double pressure;
  double humidity;
  double cloud_cover;
  OwmPrecipitation precipitation;
  };


/*============================================================================
 * owm_weather_create
 * =========================================================================*/
OwmWeather *owm_weather_create (void)
  {
  OwmWeather *self = malloc (sizeof (OwmWeather));
  memset (self, 0, sizeof (OwmWeather));
  return self;
  }


/*============================================================================
 * owm_weather_destroy
 * =========================================================================*/
void owm_weather_destroy (OwmWeather *self)
  {
  free (self);
  }


/*============================================================================
 * owm_weather_get_conditions
 * =========================================================================*/
OwmConditions owm_weather_get_conditions (const OwmWeather *self)
  {
  return self->conditions;
  }


/*============================================================================
 * owm_weather_get_conditions
 * =========================================================================*/
void owm_weather_set_conditions (OwmWeather *self, OwmConditions conditions)
  {
  self->valid |= OWM_VALID_CONDITIONS;
  self->conditions = conditions;
  }


/*============================================================================
 * owm_weather_get_conditions_name
 * =========================================================================*/
const char *owm_weather_get_conditions_string (const OwmWeather *self)
  {
  return owm_weather_conditions_to_string (self->conditions);
  }


/*============================================================================
 * owm_weather_conditions_to_string
 * =========================================================================*/
const char *owm_weather_conditions_to_string (const OwmConditions conditions)
  {
  switch (conditions)
    {
    case OWM_COND_THUNDERSTORMS_LIGHT_RAIN: 
      return "thunderstorms with light rain";
    case OWM_COND_THUNDERSTORMS_RAIN: 
      return "thunderstorms with rain";
    case OWM_COND_THUNDERSTORMS_HEAVY_RAIN: 
      return "thunderstorms with heavy rain";
    case OWM_COND_LIGHT_THUNDERSTORMS: 
      return "light thunderstorms";
    case OWM_COND_THUNDERSTORM: 
      return "thunderstorms";
    case OWM_COND_HEAVY_THUNDERSTORMS: 
      return "heavy thunderstorms";
    case OWM_COND_RAGGED_THUNDERSTORMS: 
      return "ragged thunderstorms";
    case OWM_COND_THUNDERSTORSMS_LIGHT_DRIZZLE: 
      return "thunderstorms with light drizzle";
    case OWM_COND_THUNDERSTORSMS_DRIZZLE: 
      return "thunderstorms with drizzle";
    case OWM_COND_THUNDERSTORSMS_HEAVYDRIZZLE: 
      return "thunderstorms with heavy drizzle";
    case OWM_COND_LIGHT_DRIZZLE:
      return "light drizzle";
    case OWM_COND_DRIZZLE:
      return "drizzle";
    case OWM_COND_HEAVY_DRIZZLE:
      return "heavy drizzle";
    case OWM_COND_LIGHT_DRIZZLE_RAIN:
      return "light drizzle and rain";
    case OWM_COND_DRIZZLE_RAIN:
      return "drizzle and rain";
    case OWM_COND_HEAVY_DRIZZLE_RAIN:
      return "heavy drizzle and rain";
    case OWM_COND_DRIZZLE_RAIN_SHOWERS:
      return "drizzle and rain showers";
    case OWM_COND_HEAVY_DRIZZLE_RAIN_SHOWERS:
      return "heavy drizzle and rain showers";
    case OWM_COND_DRIZZLE_SHOWERS:
      return "drizzle showers";
    case OWM_COND_LIGHT_RAIN:
      return "light rain";
    case OWM_COND_MODERATE_RAIN:
      return "moderate rain";
    case OWM_COND_HEAVY_RAIN:
      return "heavy rain";
    case OWM_COND_VERY_HEAVY_RAIN:
      return "very heavy rain";
    case OWM_COND_TORRENTIAL_RAIN:
      return "torrential rain";
    case OWM_COND_FREEZING_RAIN:
      return "freezing rain";
    case OWM_COND_LIGHT_RAIN_SHOWERS:
      return "light rain showers";
    case OWM_COND_RAIN_SHOWERS:
      return "rain showers";
    case OWM_COND_HEAVY_RAIN_SHOWERS:
      return "heavy rain showers";
    case OWM_COND_RAGGED_RAIN_SHOWERS:
      return "ragged rain showers";
    case OWM_COND_LIGHT_SNOW:
      return "light snow";
    case OWM_COND_SNOW:
      return "snow";
    case OWM_COND_HEAVY_SNOW:
      return "heavy snow";
    case OWM_COND_SLEET:
      return "sleet";
    case OWM_COND_SLEET_SHOWERS:
      return "sleet showers";
    case OWM_COND_LIGHT_RAIN_SNOW:
      return "light rain and snow";
    case OWM_COND_RAIN_SNOW:
      return "rain and snow";
    case OWM_COND_LIGHT_SNOW_SHOWERS:
      return "light snow showers";
    case OWM_COND_SNOW_SHOWERS:
      return "snow showers";
    case OWM_COND_HEAVY_SNOW_SHOWERS:
      return "heavy snow showers";
    case OWM_COND_MIST:
      return "mist";
    case OWM_COND_SMOKE:
      return "smoke";
    case OWM_COND_HAZE:
      return "haze";
    case OWM_COND_SAND_DUST_SWIRLS:
      return "sand or dust swirls";
    case OWM_COND_FOG:
      return "fog";
    case OWM_COND_SAND:
      return "sand";
    case OWM_COND_DUST:
      return "dust";
    case OWM_COND_VOLCANIC_ASH:
      return "volcanic ash";
    case OWM_COND_SQUALLS:
      return "squalls";
    case OWM_COND_TORNADO1:
      return "tornado";
    case OWM_COND_CLEAR_SKY:
      return "clear sky";
    case OWM_COND_LIGHT_CLOUD:
      return "light cloud";
    case OWM_COND_SCATTERED_CLOUD:
      return "scattered cloud";
    case OWM_COND_BROKEN_CLOUD:
      return "broken cloud";
    case OWM_COND_OVERCAST:
      return "overcast";
    case OWM_COND_TORNADO2:
      return "tornado";
    case OWM_COND_TROPICAL_STORM:
      return "tropical storm";
    case OWM_COND_HURRICANE1:
      return "hurricane";
    case OWM_COND_COLD:
      return "cold";
    case OWM_COND_HOT:
      return "hot";
    case OWM_COND_WINDY:
      return "windy";
    case OWM_COND_HAIL:
      return "hail";
    case OWM_COND_CALM:
      return "calm";
    case OWM_COND_LIGHT_BREEZE:
      return "light breeze";
    case OWM_COND_GENTLE_BREEZE:
      return "gentle breeze";
    case OWM_COND_MODERATE_BREEZE:
      return "moderate breeze";
    case OWM_COND_FRESH_BREEZE:
      return "fresh breeze";
    case OWM_COND_STRONG_BREEZE:
      return "string breeze";
    case OWM_COND_GALE:
      return "gale";
    case OWM_COND_SEVERE_GALE:
      return "severe gale";
    case OWM_COND_STORM:
      return "storm";
    case OWM_COND_VIOLENT_STORM:
      return "violent storm";
    case OWM_COND_HURRICANE2:
      return "hurricane";
    default: 
      return "unknown";
    }
  return "unknown";
  }



/*============================================================================
 * owm_weather_set_temperature
 * =========================================================================*/
void owm_weather_set_temperature (OwmWeather *self, double temp)
  {
  self->temp = temp;
  self->valid |= OWM_VALID_TEMP;
  }


/*============================================================================
 * owm_weather_get_temperature
 * =========================================================================*/
double owm_weather_get_temperature (const OwmWeather *self)
  {
  return self->temp;
  }


/*============================================================================
 * owm_weather_get_valid
 * =========================================================================*/
int owm_weather_get_valid (const OwmWeather *self)
  {
  return self->valid;
  }


/*============================================================================
 * owm_weather_get_start_time
 * =========================================================================*/
time_t owm_weather_get_start_time (const OwmWeather *self)
  {
  return self->start_time;
  }

/*============================================================================
 * owm_weather_set_start_time
 * =========================================================================*/
void owm_weather_set_start_time (OwmWeather *self, time_t t)
  {
  self->start_time = t;
  self->valid |= OWM_VALID_START;
  }

/*============================================================================
 * owm_weather_get_end_time
 * =========================================================================*/
time_t owm_weather_get_end_time (const OwmWeather *self)
  {
  return self->end_time;
  }

/*============================================================================
 * owm_weather_set_end_time
 * =========================================================================*/
void owm_weather_set_end_time (OwmWeather *self, time_t t)
  {
  self->valid |= OWM_VALID_END;
  self->end_time = t;
  }


/*============================================================================
 * owm_weather_get_precipitation
 * =========================================================================*/
OwmPrecipitation owm_weather_get_precipitation (const OwmWeather *self)
  {
  return self->precipitation;
  }


/*============================================================================
 * owm_weather_get_precipitation
 * =========================================================================*/
const char *owm_weather_get_precipitation_name (const OwmWeather *self)
  {
  OwmPrecipitation precip =  self->precipitation;
  switch (precip)
    {
    case OWM_PRECIP_NONE:
      return "none";
    case OWM_PRECIP_DRIZZLE:
      return "none";
    case OWM_PRECIP_RAIN:
      return "rain";
    case OWM_PRECIP_SLEET:
      return "sleet";
    case OWM_PRECIP_SNOW:
      return "snow";
    case OWM_PRECIP_GRAUPEL:
      return "graupel";
    case OWM_PRECIP_HAIL:
      return "hail";
    default:
      return "unknown";
    } 
  }


/*============================================================================
 * owm_weather_set_precipitation
 * =========================================================================*/
void owm_weather_set_precipitation (OwmWeather *self, 
                 OwmPrecipitation precip)
  {
  self->precipitation = precip;
  }


/*============================================================================
 * owm_weather_set_wind_direction
 * =========================================================================*/
void owm_weather_set_wind_direction (OwmWeather *self, double wind)
  {
  self->wind_direction = wind;
  self->valid |= OWM_VALID_WIND_DIRECTION;
  }


/*============================================================================
 * owm_weather_get_temperature
 * =========================================================================*/
double owm_weather_get_wind_direction (const OwmWeather *self)
  {
  return self->wind_direction;
  }


/*============================================================================
 * owm_weather_set_wind_speed
 * =========================================================================*/
void owm_weather_set_wind_speed (OwmWeather *self, double wind)
  {
  self->wind_speed = wind;
  self->valid |= OWM_VALID_WIND_SPEED;
  }


/*============================================================================
 * owm_weather_get_temperature
 * =========================================================================*/
double owm_weather_get_wind_speed (const OwmWeather *self)
  {
  return self->wind_speed;
  }


/*============================================================================
 * owm_weather_set_pressure
 * =========================================================================*/
void owm_weather_set_pressure (OwmWeather *self, double press)
  {
  self->pressure = press;
  self->valid |= OWM_VALID_PRESSURE;
  }


/*============================================================================
 * owm_weather_get_temperature
 * =========================================================================*/
double owm_weather_get_pressure (const OwmWeather *self)
  {
  return self->pressure;
  }


/*============================================================================
 * owm_weather_set_humidity
 * =========================================================================*/
void owm_weather_set_humidity (OwmWeather *self, double humidity)
  {
  self->humidity = humidity;
  self->valid |= OWM_VALID_HUMIDITY;
  }


/*============================================================================
 * owm_weather_get_temperature
 * =========================================================================*/
double owm_weather_get_humidity (const OwmWeather *self)
  {
  return self->humidity;
  }


/*============================================================================
 * owm_weather_set_cloud_cover
 * =========================================================================*/
void owm_weather_set_cloud_cover (OwmWeather *self, double cloud_cover)
  {
  self->cloud_cover = cloud_cover;
  self->valid |= OWM_VALID_CLOUD_COVER;
  }


/*============================================================================
 * owm_weather_get_temperature
 * =========================================================================*/
double owm_weather_get_cloud_cover (const OwmWeather *self)
  {
  return self->cloud_cover;
  }


/*============================================================================
 * owm_weather_wind_direction_to_string
 * =========================================================================*/
const char *owm_weather_wind_direction_to_string (double wind_direction)
  {
  if (wind_direction > 11.25 && wind_direction <= 33.75) return "NNE";
  if (wind_direction > 33.75 && wind_direction <= 56.25) return "NE";
  if (wind_direction > 56.25 && wind_direction <= 78.25) return "ENE";
  if (wind_direction > 78.25 && wind_direction <= 101.25) return "E";
  if (wind_direction > 101.25 && wind_direction <= 123.75) return "ESE";
  if (wind_direction > 123.75 && wind_direction <= 146.25) return "SE";
  if (wind_direction > 146.25 && wind_direction <= 168.75) return "SSE";
  if (wind_direction > 168.75 && wind_direction <= 191.25) return "S";
  if (wind_direction > 191.25 && wind_direction <= 213.75) return "SSW";
  if (wind_direction > 213.75 && wind_direction <= 236.25) return "SW";
  if (wind_direction > 236.25 && wind_direction <= 258.75) return "WSW";
  if (wind_direction > 258.75 && wind_direction <= 281.25) return "W";
  if (wind_direction > 281.25 && wind_direction <= 303.75) return "WNW";
  if (wind_direction > 303.75 && wind_direction <= 326.25) return "NW";
  if (wind_direction > 326.25 && wind_direction <= 348.75) return "NNW";
  return "N";
  }

/*============================================================================
 * owm_weather_get_wind_direction_string
 * =========================================================================*/
const char *owm_weather_get_wind_direction_string (const OwmWeather *self)
  {
  return owm_weather_wind_direction_to_string (self->wind_direction);
  }




