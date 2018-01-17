/*============================================================================
  epub2txt v2 
  owm_list.c
  Copyright (c)2000-2017 Kevin Boone, GPL v3.0
============================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <pthread.h>
#include <owm/owm_list.h>

typedef struct _OwmListItem
  {
  struct _OwmListItem *next;
  void *data;
  } OwmListItem;

struct _OwmList
  {
  pthread_mutex_t mutex;
  OwmListItemFreeFn free_fn; 
  OwmListItem *head;
  };

/*==========================================================================
owm_list_create
*==========================================================================*/
OwmList *owm_list_create (OwmListItemFreeFn free_fn)
  {
  OwmList *owm_list = malloc (sizeof (OwmList));
  memset (owm_list, 0, sizeof (OwmList));
  owm_list->free_fn = free_fn;
  pthread_mutex_init (&owm_list->mutex, NULL);
  return owm_list;
  }

/*==========================================================================
owm_list_create_strings 
*==========================================================================*/
OwmList *owm_list_create_strings (void)
  {
  return owm_list_create (free);
  }

/*==========================================================================
owm_list_destroy
*==========================================================================*/
void owm_list_destroy (OwmList *self)
  {
  if (!self) return;

  pthread_mutex_lock (&self->mutex);
  OwmListItem *l = self->head;
  while (l)
    {
    if (self->free_fn)
      self->free_fn (l->data);
    OwmListItem *temp = l;
    l = l->next;
    free (temp);
    }

  pthread_mutex_unlock (&self->mutex);
  pthread_mutex_destroy (&self->mutex);
  free (self);
  }


/*==========================================================================
owm_list_prepend
Note that the caller must not modify or free the item added to the owm_list. It
will remain on the owm_list until free'd by the owm_list itself, by calling
the supplied free function
*==========================================================================*/
void owm_list_prepend (OwmList *self, void *item)
  {
  pthread_mutex_lock (&self->mutex);
  OwmListItem *i = malloc (sizeof (OwmListItem));
  i->data = item;
  i->next = NULL;

  if (self->head)
    {
    i->next = self->head;
    self->head = i;
    }
  else
    {
    self->head = i;
    }
  pthread_mutex_unlock (&self->mutex);
  }


/*==========================================================================
owm_list_append
Note that the caller must not modify or free the item added to the owm_list. It
will remain on the owm_list until free'd by the owm_list itself, by calling
the supplied free function
*==========================================================================*/
void owm_list_append (OwmList *self, void *item)
  {
  pthread_mutex_lock (&self->mutex);
  OwmListItem *i = malloc (sizeof (OwmListItem));
  i->data = item;
  i->next = NULL;

  if (self->head)
    {
    OwmListItem *l = self->head;
    while (l->next)
      l = l->next;
    l->next = i;
    }
  else
    {
    self->head = i;
    }
  pthread_mutex_unlock (&self->mutex);
  }


/*==========================================================================
owm_list_length
*==========================================================================*/
int owm_list_length (OwmList *self)
  {
  if (!self) return 0;

  pthread_mutex_lock (&self->mutex);
  OwmListItem *l = self->head;
  int i = 0;
  while (l != NULL)
    {
    l = l->next;
    i++;
    }

  pthread_mutex_unlock (&self->mutex);
  return i;
  }

/*==========================================================================
owm_list_get
*==========================================================================*/
void *owm_list_get (OwmList *self, int index)
  {
  if (!self) return NULL;

  pthread_mutex_lock (&self->mutex);
  OwmListItem *l = self->head;
  int i = 0;
  while (l != NULL && i != index)
    {
    l = l->next;
    i++;
    }
  pthread_mutex_unlock (&self->mutex);

  return l->data;
  }


/*==========================================================================
owm_list_dump
*==========================================================================*/
void owm_list_dump (OwmList *self)
  {
  int i, l = owm_list_length (self);
  for (i = 0; i < l; i++)
    {
    const char *s = owm_list_get (self, i);
    printf ("%s\n", s);
    }
  }


/*==========================================================================
owm_list_contains
*==========================================================================*/
BOOL owm_list_contains (OwmList *self, const void *item, OwmListCompareFn fn)
  {
  if (!self) return FALSE;
  pthread_mutex_lock (&self->mutex);
  OwmListItem *l = self->head;
  BOOL found = FALSE;
  while (l != NULL && !found)
    {
    if (fn (l->data, item) == 0) found = TRUE; 
    l = l->next;
    }
  pthread_mutex_unlock (&self->mutex);
  return found; 
  }


/*==========================================================================
owm_list_contains_string
*==========================================================================*/
BOOL owm_list_contains_string (OwmList *self, const char *item)
  {
  return owm_list_contains (self, item, (OwmListCompareFn)strcmp);
  }


/*==========================================================================
owm_list_remove
IMPORTANT -- The "item" argument cannot be a direct reference to an
item already in the owm_list. If that items is removed from the owm_list its
memory will be freed. The "item" argument will this be an invalid
memory reference, and the program will crash. It is necessary
to copy the item first.
*==========================================================================*/
void owm_list_remove (OwmList *self, const void *item, OwmListCompareFn fn)
  {
  if (!self) return;
  pthread_mutex_lock (&self->mutex);
  OwmListItem *l = self->head;
  OwmListItem *last_good = NULL;
  while (l != NULL)
    {
    if (fn (l->data, item) == 0)
      {
      if (l == self->head)
        {
        self->head = l->next; // l-> next might be null
        }
      else
        {
        if (last_good) last_good->next = l->next;
        }
      self->free_fn (l->data);  
      OwmListItem *temp = l->next;
      free (l);
      l = temp;
      } 
    else
      {
      last_good = l;
      l = l->next;
      }
    }
  pthread_mutex_unlock (&self->mutex);
  }

/*==========================================================================
owm_list_remove_string
*==========================================================================*/
void owm_list_remove_string (OwmList *self, const char *item)
  {
  owm_list_remove (self, item, (OwmListCompareFn)strcmp);
  }


/*==========================================================================
owm_list_clone
*==========================================================================*/
OwmList *owm_list_clone (OwmList *self, OwmListCopyFn copyFn)
  {
  OwmListItemFreeFn free_fn = self->free_fn; 
  OwmList *new = owm_list_create (free_fn);

  pthread_mutex_lock (&self->mutex);
  OwmListItem *l = self->head;
  while (l != NULL)
    {
    void *data = copyFn (l->data);
    owm_list_append (new, data);
    l = l->next;
    }
  pthread_mutex_unlock (&self->mutex);

  return new;
  }



