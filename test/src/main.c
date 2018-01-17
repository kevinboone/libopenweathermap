/*============================================================================
 * Test driver for libopenweathermap
 * Copyright (c)2018 Kevin Boone, GPL v3.0
 * Usage: ./openweathermap [APPID] [city_code]
 * You will need an OpenWeatherMap APP ID to run this program. For 
 * city codes, refer to the included city.list.json, but a more up-to-date
 * version is probably available on the OWM website.
 * =========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <owm/owm.h>


/*============================================================================
 * format_weather 
 * Writes some useful information from an OwmWeather structure
 * =========================================================================*/
void format_weather (const OwmWeather *weather)
  {
  time_t t1 = owm_weather_get_start_time (weather);
  char buff[100];
  struct tm *tm = localtime (&t1);
  strftime (buff, sizeof (buff), "%a %x %X", tm);
  printf ("%s ", buff);
  printf (owm_weather_get_conditions_string (weather));
  printf (", ");
  printf ("%d deg C,  ", (int)owm_weather_get_temperature (weather));
  printf ("wind %d mph ", (int) owm_weather_get_wind_speed (weather));
  printf ("%s ", owm_weather_get_wind_direction_string (weather));
  printf ("\n");

  // Other weather values we could use 
  /*
  printf ("precip=%s\n", owm_weather_get_precipitation_name 
      (weather));
  printf ("pressure=%f\n", owm_weather_get_pressure
      (weather));
  printf ("humidity=%f\n", owm_weather_get_humidity
      (weather));
  printf ("cloud cover=%f\n", owm_weather_get_cloud_cover
      (weather));
  */
  }


/*============================================================================
 * get_dow 
 * Store the current day-of-week in a buffer dow of max size len
 * =========================================================================*/
void get_dow (time_t t, char *dow, int len)
  {
  struct tm *tm = localtime (&t);
  strftime (dow, len, "%a", tm);
  }


/*============================================================================
 * format_forecast_summary 
 * Use own_get_daily_summary to print a forecast summary for time t,
 *   provided that time lies within the range of times captured by the
 *   forecast structure
 * =========================================================================*/
void format_forecast_summary (time_t t, const OwmForecast *forecast)
  {
  char *error = NULL;

  double min_temp, max_temp, wind_dir, wind_speed;
  OwmConditions modal_conditions;
  owm_forecast_get_daily_summary (forecast, t, &min_temp, &max_temp, 
      &modal_conditions, &wind_dir, &wind_speed, &error);
  const char *conditions_string = owm_weather_conditions_to_string 
      (modal_conditions);
  printf ("%s %d - %d deg C, wind %d mph %s", conditions_string, 
    (int)min_temp, (int)max_temp, (int)wind_speed, 
    owm_weather_wind_direction_to_string (wind_dir));
  }


/*============================================================================
 * main
 * =========================================================================*/
int main (int argc, char **argv)
  {
  if (argc != 3)
    {
    fprintf (stderr, "Usage: %s [APPID] [city_code]\n", argv[0]);
    exit (-1);
    }

  const char *APPID = argv[1];
  const char *city_code = argv[2];

  // Get the 5-day forecast starting at the current time, for the
  //  given location
  char *error = NULL;
  OwmForecast *forecast = owm_forecast_get (APPID, city_code, &error); 
  if (error == NULL)
    {
    // Print the current weather, which should be the first data point
    // in the forecast list
    const OwmWeather *weather_now = owm_forecast_get_point (forecast, 0);
    format_weather (weather_now);

    // Format and print the sunrise and sunset times from the
    //  forecast
    time_t rise, set;
    char buff[100];
    owm_forecast_get_rise_set (forecast, &rise, &set);
    struct tm *tm = localtime (&rise);
    strftime (buff, sizeof (buff), "%H:%M", tm);
    printf ("     Sunrise %s", buff);
    tm = localtime (&set);
    strftime (buff, sizeof (buff), "%H:%M", tm);
    printf (", sunset %s\n", buff);
    

    // Print the forecast summary for today and the next two days
    time_t now = time (NULL);

    printf ("%10s: ", "Today");
    format_forecast_summary (now, forecast);
    printf ("\n");

    now += 24 * 3600 * 1;
    char dow[20];
    get_dow (now, dow, sizeof (dow));
    printf ("%10s: ", dow);
    format_forecast_summary (now, forecast);
    printf ("\n");

    now += 24 * 3600 * 1;
    get_dow (now, dow, sizeof (dow));
    printf ("%10s: ", dow);
    format_forecast_summary (now, forecast);
    printf ("\n");

    // Clean up
    owm_forecast_destroy (forecast);
    }
  else
    {
    fprintf (stderr, "%s: %s\n", argv[0], error);
    free (error);
    }
  }



