/*============================================================================
  owm_owm_string.c
  Copyright (c)2017 Kevin Boone, GPL v3.0
============================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <owm/owm_defs.h> 
#include <owm/owm_string.h> 

struct _OwmString
  {
  char *str;
  }; 


/*==========================================================================
owm_string_create_empty 
*==========================================================================*/
OwmString *owm_string_create_empty (void)
  {
  return owm_string_create ("");
  }


/*==========================================================================
owm_string_create
*==========================================================================*/
OwmString *owm_string_create (const char *s)
  {
  OwmString *self = malloc (sizeof (OwmString));
  self->str = strdup (s);
  return self;
  }


/*==========================================================================
owm_string_destroy
*==========================================================================*/
void owm_string_destroy (OwmString *self)
  {
  if (self)
    {
    if (self->str) free (self->str);
    free (self);
    }
  }


/*==========================================================================
owm_string_cstr
*==========================================================================*/
const char *owm_string_cstr (const OwmString *self)
  {
  return self->str;
  }


/*==========================================================================
owm_string_cstr_safe
*==========================================================================*/
const char *owm_string_cstr_safe (const OwmString *self)
  {
  if (self)
    {
    if (self->str) 
      return self->str;
    else
      return "";
    }
  else
    return "";
  }


/*==========================================================================
owm_string_append
*==========================================================================*/
void owm_string_append (OwmString *self, const char *s) 
  {
  if (!s) return;
  if (self->str == NULL) self->str = strdup ("");
  int newlen = strlen (self->str) + strlen (s) + 2;
  self->str = realloc (self->str, newlen);
  strcat (self->str, s);
  }


/*==========================================================================
owm_string_prepend
*==========================================================================*/
void owm_string_prepend (OwmString *self, const char *s) 
  {
  if (!s) return;
  if (self->str == NULL) self->str = strdup ("");
  int newlen = strlen (self->str) + strlen (s) + 2;
  char *temp = strdup (self->str); 
  free (self->str);
  self->str = malloc (newlen);
  strcpy (self->str, s);
  strcat (self->str, temp);
  free (temp);
  }


/*==========================================================================
owm_string_append_printf
*==========================================================================*/
void owm_string_append_printf (OwmString *self, const char *fmt,...) 
  {
  if (self->str == NULL) self->str = strdup ("");
  va_list ap;
  va_start (ap, fmt);
  char *s;
  vasprintf (&s, fmt, ap);
  owm_string_append (self, s);
  free (s);
  va_end (ap);
  }


/*==========================================================================
owm_string_length
*==========================================================================*/
int owm_string_length (const OwmString *self)
  {
  if (self == NULL) return 0;
  if (self->str == NULL) return 0;
  return strlen (self->str);
  }


/*==========================================================================
owm_string_clone
*==========================================================================*/
OwmString *owm_string_clone (const OwmString *self)
  {
  if (!self) return NULL;
  if (!self->str) return owm_string_create_empty();
  return owm_string_create (owm_string_cstr (self));
  }


/*==========================================================================
owm_string_find
*==========================================================================*/
int owm_string_find (const OwmString *self, const char *search)
  {
  if (!self) return -1;
  if (!self->str) return -1;
  const char *p = strstr (self->str, search);
  if (p)
    return p - self->str;
  else
    return -1;
  }


/*==========================================================================
owm_string_delete
*==========================================================================*/
void owm_string_delete (OwmString *self, const int pos, const int len)
  {
  char *str = self->str;
  if (pos + len > strlen (str))
    owm_string_delete (self, pos, strlen(str) - len);
  else
    {
    char *buff = malloc (strlen (str) - len + 2);
    strncpy (buff, str, pos); 
    strcpy (buff + pos, str + pos + len);
    free (self->str);
    self->str = buff;
    }
  }


/*==========================================================================
owm_string_insert
*==========================================================================*/
void owm_string_insert (OwmString *self, const int pos, 
    const char *replace)
  {
  char *buff = malloc (strlen (self->str) + strlen (replace) + 2);
  char *str = self->str;
  strncpy (buff, str, pos);
  buff[pos] = 0;
  strcat (buff, replace);
  strcat (buff, str + pos); 
  free (self->str);
  self->str = buff;
  }



/*==========================================================================
owm_string_substitute_all
*==========================================================================*/
OwmString *owm_string_substitute_all (const OwmString *self, 
    const char *search, const char *replace)
  {
  OwmString *working = owm_string_clone (self);
  BOOL cont = TRUE;
  while (cont)
    {
    int i = owm_string_find (working, search);
    if (i >= 0)
      {
      owm_string_delete (working, i, strlen (search));
      owm_string_insert (working, i, replace);
      }
    else
      cont = FALSE;
    }
  return working;
  }


/*==========================================================================
  owm_string_create_from_utf8_file 
*==========================================================================*/
BOOL owm_string_create_from_utf8_file (const char *filename, 
    OwmString **result, char **error)
  {
  OwmString *self = NULL;
  BOOL ok = FALSE; 
  int f = open (filename, O_RDONLY);
  if (f > 0)
    {
    self = malloc (sizeof (OwmString));
    struct stat sb;
    fstat (f, &sb);
    int64_t size = sb.st_size;
    char *buff = malloc (size + 2);
    read (f, buff, size);
    self->str = buff; 
    self->str[size] = 0;
    *result = self;
    ok = TRUE;
    }
  else
    {
    asprintf (error, "Can't open file '%s' for reading: %s", 
      filename, strerror (errno));
    ok = FALSE;
    }

  return ok;
  }


/*==========================================================================
  owm_string_encode_url
*==========================================================================*/
static char to_hex(char code)
  {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
  }


OwmString *owm_string_encode_url (const char *str)
  {
  if (!str) return owm_string_create_empty();;
  const char *pstr = str; 
  char *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr)
    {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_'
      || *pstr == '.' || *pstr == '~')
      *pbuf++ = *pstr;
    else if (*pstr == ' ')
      *pbuf++ = '+';
    else
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4),
         *pbuf++ = to_hex(*pstr & 15);
    pstr++;
    }
  *pbuf = '\0';
  OwmString *result = owm_string_create (buf);
  free (buf);
  return (result);
  }


/*==========================================================================
  owm_string_append_byte
*==========================================================================*/
void owm_string_append_byte (OwmString *self, const BYTE byte)
  {
  char buff[2];
  buff[0] = byte;
  buff[1] = 0;
  owm_string_append (self, buff);
  }


/*==========================================================================
  owm_string_append_c
*==========================================================================*/
void owm_string_append_c (OwmString *self, const uint32_t ch)
  {
  if (ch < 0x80) 
    {
    owm_string_append_byte (self, (BYTE)ch);
    }
  else if (ch < 0x0800) 
    {
    owm_string_append_byte (self, (BYTE)((ch >> 6) | 0xC0));
    owm_string_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  else if (ch < 0x10000) 
    {
    owm_string_append_byte (self, (BYTE)((ch >> 12) | 0xE0));
    owm_string_append_byte (self, (BYTE)((ch >> 6 & 0x3F) | 0x80));
    owm_string_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  else 
    {
    owm_string_append_byte (self, (BYTE)((ch >> 18) | 0xF0));
    owm_string_append_byte (self, (BYTE)(((ch >> 12) & 0x3F) | 0x80));
    owm_string_append_byte (self, (BYTE)(((ch >> 6) & 0x3F) | 0x80));
    owm_string_append_byte (self, (BYTE)((ch & 0x3F) | 0x80));
    }
  }




