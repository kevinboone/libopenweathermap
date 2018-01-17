/*============================================================================
 * libopenweathermap
 * owm_curl.h
 * Copyright (c)2018 Kevin Boone, GPL v3.0
 * =========================================================================*/

#pragma once

#ifdef __CPLUSPLUS
  extern "C" {
#endif

void owm_curl_get (const char *uri, char **result, char **error);

#ifdef __CPLUSPLUS
  } 
#endif



