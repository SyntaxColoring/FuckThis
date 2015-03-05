/* util.h: General-purpose stuff. */

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h> /* For FILE*. */

/* BYOB (Bring Your Own Bool).  :) */
typedef enum {false, true} bool;

/* Typedefs for unsigned types big enough for at least 1, 2 or 4 bytes. */
typedef unsigned char u1;
typedef unsigned int  u2;
typedef unsigned long u4;

/* Calls perror() with message and aborts the program. */
void fatal_error(const char* message);

/* Functions to allocate memory or die trying. */
void* malloc_or_die(size_t size);
void* realloc_or_die(void* pointer, size_t size);

/* Returns a string that is a copy of source.  The caller is responsible for
   freeing the new string. */
char* string_clone(const char* source);

/* Returns a string that is a concatenated with b.  The caller is responsible
   for freeing the new string. */
char* string_create_concatenated(const char* a, const char* b);

#endif
