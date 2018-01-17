/*============================================================================
  owm_string.c
  Copyright (c)2017 Kevin Boone, GPL v3.0
============================================================================*/

#pragma once

#include <stdint.h>

struct _OwmString;
typedef struct _OwmString OwmString;

#ifdef __CPLUSPLUS
  extern "C" {
#endif

OwmString   *owm_string_create_empty (void);
OwmString   *owm_string_create (const char *s);
OwmString   *owm_string_clone (const OwmString *self);
int          owm_string_find (const OwmString *self, const char *search);
void         owm_string_destroy (OwmString *self);
const char  *owm_string_cstr (const OwmString *self);
const char  *owm_string_cstr_safe (const OwmString *self);
void         owm_string_append_printf (OwmString *self, const char *fmt,...);
void         owm_string_append (OwmString *self, const char *s);
void         owm_string_append_c (OwmString *self, const uint32_t c);
void         owm_string_prepend (OwmString *self, const char *s);
int          owm_string_length (const OwmString *self);
OwmString   *owm_string_substitute_all (const OwmString *self, 
                const char *search, const char *replace);
void        owm_string_delete (OwmString *self, const int pos, 
                const int len);
void        owm_string_insert (OwmString *self, const int pos, 
                const char *replace);
BOOL        owm_string_create_from_utf8_file (const char *filename, 
                OwmString **result, char **error);
OwmString   *owm_string_encode_url (const char *s);
void        owm_string_append_byte (OwmString *self, const BYTE byte);

#ifdef __CPLUSPLUS
 }
#endif


