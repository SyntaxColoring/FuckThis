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

/* Functions to allocate memory or die trying. */
void* malloc_or_die(size_t size);
void* realloc_or_die(void* pointer, size_t size);

/* A buffer for writing binary data. */
struct byte_buffer
{
	u1* data;
	size_t length;
	size_t capacity;
};

/* Default "constructor" for struct byte_buffer. */
#define BYTE_BUFFER_EMPTY {NULL, 0, 0}

/* Releases the resources held by buffer and resets its members. */
void bb_free(struct byte_buffer* buffer);

/* Gives buffer at least enough space to hold capacity u1s.  If capacity is
   smaller than buffer's current capacity, data will be truncated.  If
   allocation fails, the program is aborted.  This function is called as needed
   by bb_append() and all bb_write_*() functions. */
void bb_reserve(struct byte_buffer* buffer, size_t capacity);

/* Returns a pointer to a sub-array at the end of buffer to which at least
   length u1s can be written. */
u1* bb_append(struct byte_buffer* buffer, size_t length);

/* Functions for writing to a byte_buffer (always in big-endian form). */
void bb_write_u1(struct byte_buffer* buffer, u1 data);
void bb_write_u2(struct byte_buffer* buffer, u2 data);
void bb_write_u4(struct byte_buffer* buffer, u4 data);

/* Writes length elements from array into buffer. */
void bb_write_array(struct byte_buffer* buffer, const u1* array, size_t length);

/* Appends all the elements in the given initializer to the given buffer.
   Usage example: bb_write_static_array(foo, {1, 2, 3}); */
#define bb_write_static_array(buffer_pointer, array_initializer) \
	do { \
		u1 array[] = array_initializer; \
		bb_write_array(buffer_pointer, array, sizeof(array)); \
	} while (false)

/* Writes all of a buffer's data into a stream.  Returns whether or not the
   operation succeeded. */
bool bb_write_to_stream(const struct byte_buffer* buffer, FILE* stream);

#endif
