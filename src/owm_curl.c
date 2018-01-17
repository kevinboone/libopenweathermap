/*============================================================================
 * libopenweathermap
 * owm_curl.c
 * Copyright (c)2018 Kevin Boone, GPL v3.0
 * =========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <owm/owm_defs.h>
#include <owm/owm_config.h>
#include <owm/owm_string.h>

#define EASY_INIT_FAIL "Cannot initialize curl"

/*---------------------------------------------------------------------------
Private structs
---------------------------------------------------------------------------*/
struct DBWriteStruct 
  {
  char *memory;
  size_t size;
  };


/*---------------------------------------------------------------------------
feed_write_callback
Callback for storing server response into an expandable memory block
---------------------------------------------------------------------------*/
static size_t owm_curl_write_callback (void *contents, size_t size, 
    size_t nmemb, void *userp)
  {
  size_t realsize = size * nmemb;
  struct DBWriteStruct *mem = (struct DBWriteStruct *)userp;
  mem->memory = realloc (mem->memory, mem->size + realsize + 1);
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
  }



void owm_curl_get (const char *uri, char **result, char **error)
  {
  CURL* curl = curl_easy_init();
  if (curl)
   {
   struct DBWriteStruct response;
   response.memory = malloc(1);  
   response.size = 0;    
   
   struct curl_slist *headers = NULL;

    curl_easy_setopt (curl, CURLOPT_URL, uri);
	  
    char curl_error [CURL_ERROR_SIZE];
    curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, curl_error);
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, owm_curl_write_callback);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);

    CURLcode curl_code = curl_easy_perform (curl);
    if (curl_code == 0)
      {
      long codep = 0;
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &codep); 
      if (codep == 200)
        {
        char *resp = response.memory;
        *result = strdup (resp);
        }
      else
        {
        if (error)
          {
          asprintf (error, "Server returned error %d", (int)codep);
          }
        }
      }
    else
      {
      if (error)
        *error = strdup (curl_error); 
      }

    free (response.memory);
    curl_slist_free_all (headers); 
    curl_easy_cleanup (curl);
    }
  else
    {
    if (error)
      *error = strdup (EASY_INIT_FAIL); 
    }
  }


