/* util.h: General-purpose stuff. */

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdlib.h> /* For memcpy. */

/* BYOB (Bring Your Own Bool).  :) */
typedef enum {false, true} bool;

/* Typedefs for unsigned types big enough for at least 1, 2 or 4 bytes. */
typedef unsigned char u1;
typedef unsigned int  u2;
typedef unsigned long u4;

/* Functions to allocate memory or die trying. */
void* xmalloc(size_t size);
void* xrealloc(void* pointer, size_t size);

/* A buffer for writing binary data. */
struct byte_buffer
{
	u1* data;
	size_t length;
	size_t capacity;
};

/* Default "constructor" for struct byte_buffer. */
#define BYTE_BUFFER_EMPTY {NULL, 0, 0}

/* Returns a pointer to a sub-array at the end of buffer to which at least
   length u1s can be written. */
u1* bb_append(struct byte_buffer* buffer, size_t length);

/* Functions for writing to a byte_buffer (always in big-endian form). */
void bb_write_u1(struct byte_buffer* buffer, u1 data);
void bb_write_u2(struct byte_buffer* buffer, u2 data);
void bb_write_u4(struct byte_buffer* buffer, u4 data);

/* Appends all the elements in the given initializer to the given buffer.
   Usage example: bb_write_array(foo, {1, 2, 3}); */
#define bb_write_array(buffer_pointer, data_array_initializer) \
	do { \
		u1 data_array = data_array_initializer; \
		memcpy(bb_append(buffer_pointer, sizeof(data_array_initializer)) \
		       &data_array, \
		       sizeof(data_array_initializer)); \
	} while (false)

/* Writes all of a buffer's data into a stream.  Returns whether or not the
   operation succeeded. */
bool bb_write_to_stream(const struct byte_buffer* buffer, FILE* stream);

#endif
