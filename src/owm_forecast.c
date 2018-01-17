/*============================================================================
 * libopenweathermap
 * owm_forecast.c
 * Copyright (c)2018 Kevin Boone, GPL v3.0
 * =========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <owm/owm_defs.h>
#include <owm/owm_config.h>
#include <owm/owm_string.h>
#include <owm/owm_data.h>
#include <owm/owm_forecast.h>
#include <owm/owm_curl.h>
#include <owm/owm_weather.h>
#include <owm/owm_list.h>
#include "sxmlc.h"


/*============================================================================
 * Private function prototypes 
 * =========================================================================*/
void owm_forecast_set_rise_set (OwmForecast *self, time_t rise, time_t set);

/*============================================================================
 *  Opaque data structure for results
 * =========================================================================*/
struct _OwmForecast
  {
  time_t sunrise;
  time_t sunset;
  OwmList *points;
  };


/*============================================================================
 * owm_parse_time_value
 * Parses a time value in OWM format into a time_t. Note that this involves
 *   ugly timezone conversions, because the OWM values are always in UTC,
 *   and the platform might not be
 * =========================================================================*/
static time_t owm_parse_time_value (const char *value) 
  {
  char *old_tz = getenv ("TZ");

  // Ugh. This sucks. There is no UTC/GMT equivalent of mktime() in
  //  C :/
  setenv ("TZ", "UTC0", 1);
  tzset();

  struct tm tm;
  memset (&tm, 0, sizeof (struct tm));
  strptime (value, "%FT%T", &tm);  

  time_t t = mktime (&tm);   

  if (old_tz)
    {
    setenv ("TZ", old_tz, 1);
    tzset();
    }
  else
    {
    unsetenv ("TZ");
    }

  return t;
  }


/*============================================================================
 * owm_parse_times
 * Parse the <times> element from the OWM response for a specific 
 *   weather point
 * =========================================================================*/
static void owm_parse_times (const XMLNode *n, time_t *from, time_t *to)
  {
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "from") == 0)
      {
      char *value= n->attributes[i].value;
      *from = owm_parse_time_value (value);
      }
    else if (strcmp (name, "to") == 0)
      {
      char *value= n->attributes[i].value;
      *to = owm_parse_time_value (value);
      }
    }
  }


/*============================================================================
 * owm_parse_temp 
 * parse the <temperature> element from the OWM response 
 * =========================================================================*/
static double owm_parse_temp (const XMLNode *n)
  {
  double ret = 0;
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "value") == 0)
      {
      char *value= n->attributes[i].value;
      sscanf (value, "%lf", &ret);
      }
    }
  return ret - 273.15;
  }


/*============================================================================
 * owm_parse_wind_direction
 * parse the <windDirection> element from the OWM response 
 * =========================================================================*/
static double owm_parse_wind_direction (const XMLNode *n)
  {
  double ret = 0;
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "deg") == 0)
      {
      char *value= n->attributes[i].value;
      sscanf (value, "%lf", &ret);
      }
    }
  return ret;
  }


/*============================================================================
 * owm_parse_wind_speeed
 * parse the <windSpeed> element from the OWM response 
 * =========================================================================*/
static double owm_parse_wind_speed (const XMLNode *n)
  {
  double ret = 0;
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "mps") == 0)
      {
      char *value= n->attributes[i].value;
      sscanf (value, "%lf", &ret);
      }
    }
  return ret * 2.23694;
  }


/*============================================================================
 * owm_parse_pressure
 * parse the <pressure> element from the OWM response 
 * =========================================================================*/
static double owm_parse_pressure (const XMLNode *n)
  {
  double ret = 0;
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "value") == 0)
      {
      char *value= n->attributes[i].value;
      sscanf (value, "%lf", &ret);
      }
    }
  return ret;
  }


/*============================================================================
 * owm_parse_cloud_cover
 * parse the <cloud> element from the OWM response 
 * =========================================================================*/
static double owm_parse_cloud_cover (const XMLNode *n)
  {
  double ret = 0;
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "all") == 0)
      {
      char *value= n->attributes[i].value;
      sscanf (value, "%lf", &ret);
      }
    }
  return ret;
  }


/*============================================================================
 * owm_parse_humidity
 * parse the <humidity> element from the OWM response 
 * =========================================================================*/
static double owm_parse_humidity (const XMLNode *n)
  {
  double ret = 0;
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "value") == 0)
      {
      char *value= n->attributes[i].value;
      sscanf (value, "%lf", &ret);
      }
    }
  return ret;
  }


/*============================================================================
 * owm_parse_rise_set
 * parse the <sun> element from the OWM response 
 * =========================================================================*/
static void owm_parse_rise_set (const XMLNode *n, time_t *rise, time_t *set)
  {
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "rise") == 0)
      {
      char *value= n->attributes[i].value;
      *rise = owm_parse_time_value (value);
      }
    else if (strcmp (name, "set") == 0)
      {
      char *value= n->attributes[i].value;
      *set = owm_parse_time_value (value);
      }
    }
  }


/*============================================================================
 * owm_parse_symbol
 * parse the <precipitation> element from the OWM response 
 * =========================================================================*/
static OwmPrecipitation owm_parse_precipitation (const XMLNode *n)
  {
  OwmPrecipitation ret = OWM_PRECIP_NONE;

  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "type") == 0)
      {
      char *value= n->attributes[i].value;
      if (strcasecmp (value, "drizzle") == 0)
        ret = OWM_PRECIP_DRIZZLE;
      else if (strcasecmp (value, "rain") == 0)
        ret = OWM_PRECIP_RAIN;
      else if (strcasecmp (value, "sleet") == 0)
        ret = OWM_PRECIP_SLEET;
      else if (strcasecmp (value, "snow") == 0)
        ret = OWM_PRECIP_SNOW;
      else if (strcasecmp (value, "graupel") == 0)
        ret = OWM_PRECIP_GRAUPEL;
      else if (strcasecmp (value, "hail") == 0)
        ret = OWM_PRECIP_HAIL;
      else
        ret = OWM_PRECIP_NONE;
      }
    }
  return ret;
  }


/*============================================================================
 * owm_parse_conditions
 * parse the <symbol> element from the OWM response 
 * =========================================================================*/
static int owm_parse_conditions (const XMLNode *n)
  {
  int ret = 0;
  int i, nattrs = n->n_attributes;
  for (i = 0; i < nattrs; i++)
    {
    char *name = n->attributes[i].name;
    if (strcmp (name, "number") == 0)
      {
      char *value= n->attributes[i].value;
      sscanf (value, "%d", &ret);
      }
    }
  return ret;
  }

/*============================================================================
 * owm_forecast_create
 * Create a new, empty forecast object. 
 * =========================================================================*/
static OwmForecast *owm_forecast_create (void)
  {
  OwmForecast *self = malloc (sizeof (OwmForecast));
  memset (self, 0, sizeof (OwmForecast));
  return self;
  }


/*============================================================================
 * owm_forecast_destroy
 * Cleans up memory reserved by the forecast object
 * =========================================================================*/
void owm_forecast_destroy (OwmForecast *self)
  {
  if (self)
    {
    if (self->points)
      {
      owm_list_destroy (self->points);
      }
    free (self);
    }
  }


/*============================================================================
 * owm_forecast_get_point
 * Get a specific OwmWeather entry from the forecast. Entries start
 * at zero, and continue to owm_forecast_get_points()-1 -- usually 39 
 * =========================================================================*/
const OwmWeather *owm_forecast_get_point (const OwmForecast *self, int n)
  {
  return owm_list_get (self->points, n);
  }


/*============================================================================
 * owm_forecast_get_points
 * Get the number of forecast data points -- usually 40
 * =========================================================================*/
int owm_forecast_get_points (const OwmForecast *self)
  {
  if (self->points)
    return owm_list_length (self->points);
  else
    return 0;
  }

/*============================================================================
 * owm_forecast_parse 
 * Parse the XML data returned from the OWM API call
 * =========================================================================*/
OwmForecast *owm_forecast_parse (const char *xml, char ** error)
  {
  OwmForecast *ret = NULL;

  XMLDoc doc;
  XMLDoc_init (&doc);
  if (XMLDoc_parse_buffer_DOM (xml, "openweathermap", &doc))
    {
    ret = owm_forecast_create();
    ret->points = owm_list_create ((OwmListItemFreeFn)owm_weather_destroy);

    XMLNode *root = XMLDoc_root (&doc);
    int i, l = root->n_children;
    for (i = 0; i < l; i++)
      {
      XMLNode *r1 = root->children[i]; 
      if (strcmp (r1->tag, "sun") == 0)
        {
        time_t rise, set;
        owm_parse_rise_set (r1, &rise, &set);
        owm_forecast_set_rise_set (ret, rise, set);
        }
      else if (strcmp (r1->tag, "forecast") == 0)
        {
        int i, l = r1->n_children;
        for (i = 0; i < l; i++)
          {
          XMLNode *t1 = r1->children[i]; 
          if (strcmp (t1->tag, "time") == 0)
            {
            time_t from, to;
            owm_parse_times (t1, &from, &to);
            double temp = 0;
            double wind_direction = 0;
            double wind_speed = 0;
            double pressure = 0;
            double humidity = 0;
            double cloud_cover = 0;
            OwmConditions conditions = -1;
            OwmPrecipitation precipitation = -1;
            int valid = 0;
            int i, l = t1->n_children;
            for (i = 0; i < l; i++)
              {
              XMLNode *f1 = t1->children[i]; 
              if (strcmp (f1->tag, "temperature") == 0)
                {
                temp = owm_parse_temp (f1); 
                valid |= OWM_VALID_TEMP;
                }
              else if (strcmp (f1->tag, "symbol") == 0)
                {
                conditions = owm_parse_conditions (f1); 
                valid |= OWM_VALID_CONDITIONS;
                }
              else if (strcmp (f1->tag, "precipitation") == 0)
                {
                precipitation = owm_parse_precipitation (f1); 
                valid |= OWM_VALID_PRECIPITATION;
                }
              else if (strcmp (f1->tag, "windDirection") == 0)
                {
                wind_direction = owm_parse_wind_direction (f1); 
                valid |= OWM_VALID_WIND_DIRECTION;
                }
              else if (strcmp (f1->tag, "windSpeed") == 0)
                {
                wind_speed = owm_parse_wind_speed (f1); 
                valid |= OWM_VALID_WIND_SPEED;
                }
              else if (strcmp (f1->tag, "pressure") == 0)
                {
                pressure = owm_parse_pressure (f1); 
                valid |= OWM_VALID_PRESSURE;
                }
              else if (strcmp (f1->tag, "humidity") == 0)
                {
                humidity = owm_parse_humidity (f1); 
                valid |= OWM_VALID_HUMIDITY;
                }
              else if (strcmp (f1->tag, "clouds") == 0)
                {
                cloud_cover = owm_parse_cloud_cover (f1); 
                valid |= OWM_VALID_CLOUD_COVER;
                }
              
              }
            if (valid != 0)
              {
              OwmWeather *weather = owm_weather_create (); 
              owm_weather_set_start_time (weather, from);
              owm_weather_set_end_time (weather, to);
              if (valid & OWM_VALID_TEMP)
                owm_weather_set_temperature (weather, temp);
              if (valid & OWM_VALID_CONDITIONS)
                owm_weather_set_conditions (weather, conditions);
              if (valid & OWM_VALID_PRECIPITATION)
                owm_weather_set_precipitation (weather, precipitation);
              if (valid & OWM_VALID_WIND_DIRECTION)
                owm_weather_set_wind_direction (weather, wind_direction);
              if (valid & OWM_VALID_WIND_SPEED)
                owm_weather_set_wind_speed (weather, wind_speed);
              if (valid & OWM_VALID_PRESSURE)
                owm_weather_set_pressure (weather, pressure);
              if (valid & OWM_VALID_HUMIDITY)
                owm_weather_set_humidity (weather, humidity);
              if (valid & OWM_VALID_CLOUD_COVER)
                owm_weather_set_cloud_cover (weather, cloud_cover);

              owm_list_append (ret->points, weather);
              }
            }
          }
        } 
      }
    XMLDoc_free (&doc);
    }
  else
    {
    asprintf (error, "Can't parse XML");
    }
  return ret;
  }


/*============================================================================
 * owm_forecast_set_rise_set
 * =========================================================================*/
void owm_forecast_set_rise_set (OwmForecast *self, time_t rise, time_t set)
  {
  self->sunrise = rise;
  self->sunset = set;
  }


/*============================================================================
 * owm_forecast_get_rise_set
 * =========================================================================*/
void owm_forecast_get_rise_set (OwmForecast *self, time_t *rise, time_t *set)
  {
  *rise = self->sunrise;
  *set = self->sunset;
  }


/*============================================================================
 * owm_forecast_get
 * Gets a five-day forecast, generally starting from a point up to three
 * hours before the current time. This is generally the first method
 * that any client code will call. All other methods relate to data
 * stored in the OwmForecast structure. 
 * =========================================================================*/
OwmForecast *owm_forecast_get (const char *app_id, const char *location_id, 
    char **error)
  {
  OwmForecast *ret = NULL;

  OwmString *uri = owm_string_create_empty();
  owm_string_append_printf (uri, OWM_HOST OWM_URI, "forecast", 
    location_id, app_id);

  const char *s_uri = owm_string_cstr (uri);

  char *result = NULL;

  owm_curl_get (s_uri, &result, error);
  if (*error == NULL)
    {
    ret = owm_forecast_parse (result, error);
    free (result);
    }

  owm_string_destroy (uri);

  return ret;
  }


/*============================================================================
 * owm_forecast_get_daily_summary
 * Given a time_t argument, extract a summary of conditions for the day in
 * which it falls. Note that there are certain mathematical objections to
 * trying to "average" a set of wind directions, but in practice it isn't
 * usually a problem, as the prevailing wind direction tends to be similar
 * during a day. We return maximum and minimum temperatures; average daily
 * temperaturs are not reall the same as the average of these two values, but
 * they are likely to be similar.
 * =========================================================================*/
void owm_forecast_get_daily_summary (const OwmForecast *forecast, time_t t, 
      double *min_temp, double *max_temp, OwmConditions *modal_conditions, 
      double *wind_direction, double *wind_speed, char **error)
  {
  *min_temp = 1000;
  *max_temp = -1000;
  BOOL got_cond = FALSE;
  BOOL got_temp = FALSE;
  int conds[OWM_MAX_CONDS];
  memset (conds, 0, sizeof (conds));
  struct tm tm;
  memcpy (&tm, gmtime (&t), sizeof (tm));
  int i, n = owm_forecast_get_points (forecast);
  int wind_dir_points = 0;
  int wind_speed_points = 0;
  double total_wind_speed = 0;
  double total_sin_wind_dir = 0;
  double total_cos_wind_dir = 0;
  for (i = 0; i < n; i++)
    {
    const OwmWeather *w = owm_forecast_get_point (forecast, i);
    time_t t1 = owm_weather_get_start_time (w);
    struct tm t1_tm;
    memcpy (&t1_tm, gmtime (&t1), sizeof (t1_tm));
    if (tm.tm_mday == t1_tm.tm_mday && 
      tm.tm_mon == t1_tm.tm_mon && 
      tm.tm_year == t1_tm.tm_year)
      {
      if (owm_weather_get_valid (w) & OWM_VALID_CONDITIONS)
        {
        OwmConditions cond = owm_weather_get_conditions (w);
        got_cond = TRUE;
        if (cond > 0 && cond < OWM_MAX_CONDS)
          {
          conds [cond]++;
          }
        }
      if (owm_weather_get_valid (w) & OWM_VALID_TEMP)
        {
        double temp = owm_weather_get_temperature (w);
        if (temp > *max_temp) *max_temp = temp;
        if (temp < *min_temp) *min_temp = temp;
        got_temp = TRUE;
        }
      if (owm_weather_get_valid (w) & OWM_VALID_WIND_SPEED)
        {
        double wind_speed = owm_weather_get_wind_speed (w);
        total_wind_speed += wind_speed;
        wind_speed_points++;
        }
      if (owm_weather_get_valid (w) & OWM_VALID_WIND_DIRECTION)
        {
        double wind_dir = owm_weather_get_wind_direction (w);
        total_cos_wind_dir += cos (wind_dir / 360.0 * 2.0 * M_PI);
        total_sin_wind_dir += sin (wind_dir / 360.0 * 2.0 * M_PI);
        wind_dir_points++;
        }
      }
    }
  if (!got_temp || !got_cond || wind_speed_points == 0 || wind_dir_points == 0)
    {
    *error = strdup ("Insufficient data for daily summary");
    }
  else
    {
    int cond_max = 0;
    int cond_num = 0;
    for (i = 0; i < OWM_MAX_CONDS; i++)
      {
      int c = conds[i];
      if (c > cond_max) 
         {
         cond_max = c;
         cond_num = i;
         } 
      }
    *modal_conditions = cond_num;
    *wind_speed = total_wind_speed / (double) wind_speed_points;
    // Avoid a div by zero here -- wind directions can, in theory cancel out
    if (total_cos_wind_dir == 0.0)
      *wind_direction = 0.0;
    else
      *wind_direction = atan2 (total_sin_wind_dir, total_cos_wind_dir) 
        * 360.0 / 2.0 / M_PI;
    if (*wind_direction < 0) *wind_direction += 360.0;
    }

  }


