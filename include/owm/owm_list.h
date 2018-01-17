/*============================================================================
  owm_list.h
  Copyright (c)2000-2017 Kevin Boone, GPL v3.0
============================================================================*/

#pragma once

#include <owm/owm_defs.h>

struct _OwmList;
typedef struct _OwmList OwmList;

// The comparison function should return -1, 0, +1, like strcmp 
typedef int (*OwmListCompareFn) (const void *i1, const void *i2);
typedef void* (*OwmListCopyFn) (const void *orig);
typedef void (*OwmListItemFreeFn) (void *);

OwmList   *owm_list_create (OwmListItemFreeFn free_fn);
void    owm_list_destroy (OwmList *);
void    owm_list_append (OwmList *self, void *item);
void    owm_list_prepend (OwmList *self, void *item);
void   *owm_list_get (OwmList *self, int index);
void    owm_list_dump (OwmList *self);
int     owm_list_length (OwmList *self);
BOOL    owm_list_contains (OwmList *self, const void *item, OwmListCompareFn fn);
BOOL    owm_list_contains_string (OwmList *self, const char *item);
void    owm_list_remove (OwmList *self, const void *item, OwmListCompareFn fn);
void    owm_list_remove_string (OwmList *self, const char *item);
OwmList   *owm_list_clone (OwmList *self, OwmListCopyFn copyFn);
OwmList   *owm_list_create_strings (void);

