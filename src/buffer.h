/* buffer.h - A growing buffer type. */

#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <stddef.h> /* For size_t. */
#include <stdio.h> /* For FILE*. */
#include "util.h" /* For bool. */

/* Opaque handle type. */
typedef struct buffer_implementation* buffer;

/* Unsigned integer types big enough to hold at least 1, 2 or 4 bytes, respectively. */
typedef unsigned char buffer_u1;
typedef unsigned int  buffer_u2;
typedef unsigned long buffer_u4;

/* Creates a new buffer.  At least initial_capacity bytes can be written to the
   new buffer before a reallocation occurs.  It's the caller's responsibility to
   call buffer_free() when she is done with the buffer. */
buffer buffer_create(size_t initial_capacity);

/* Frees all the resources held by the given buffer.  It's undefined behavior
   to call any buffer function on a freed buffer. */
void buffer_free(buffer handle);

/* Returns the number of bytes that have been written to the given buffer. */
size_t buffer_length(buffer handle);

/* Returns a pointer to all the data written to the given buffer.  The buffer will
   relinquish its ownership of the data; it will be the caller's responsibility to
   free() it.  A buffer is left empty after it is published, as if it were just
   created.  Calling buffer_publish() does not absolve the caller of the
   responsibility to call buffer_free() when she is done with the buffer. */
buffer_u1* buffer_publish(buffer handle);

/* Note: All buffer_write_*() functions can trigger reallocations. */

/* Writes 1 byte. */
void buffer_write_1(buffer handle, buffer_u1 u1);

/* Writes a 2-byte integer in big-endian order. */
void buffer_write_2(buffer handle, buffer_u2 u2);

/* Writes a 4-byte integer in big-endian order. */
void buffer_write_4(buffer handle, buffer_u4 u4);

/* Writes length bytes from array into the given buffer. */
void buffer_write_array(buffer handle, const buffer_u1* array, size_t length);

/* Copies the contents of the given source buffer into the given destination buffer. */
void buffer_write_buffer(buffer destination_handle, buffer source_handle);

/* Attempts to write the given buffer to the given stream and returns whether or
   not the write succeeded. */
bool buffer_write_to_stream(buffer handle, FILE* stream);

#endif
