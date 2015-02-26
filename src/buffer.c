#include "buffer.h"

#include <stddef.h> /* For size_t. */
#include <stdlib.h> /* For free(). */
#include <string.h> /* For memcpy(). */
#include <stdio.h> /* For FILE* and fwrite(). */
#include "util.h" /* For bool, malloc_or_die() and realloc_or_die(). */

struct buffer_implementation
{
	buffer_u1* data;
	size_t length;
	size_t capacity;
};

/* Ensures there is enough space for the given buffer to hold at least
   additional_length more bytes, possibly triggering a reallocation.  The
   buffer's length is incremented by additional_length.  The returned pointer
   points to the beginning of the freshly reserved space. */
static buffer_u1* buffer_extend(buffer handle, size_t additional_length)
{
	const size_t required_capacity = handle->length + additional_length;
	if (handle->capacity < required_capacity)
	{
		const size_t new_capacity = required_capacity*2;
		handle->data = realloc_or_die(handle->data, new_capacity);
		handle->capacity = new_capacity;
	}
	handle->length += additional_length;
	return handle->data + handle->length - additional_length;
}

/* Sets the given buffer to be empty. */
static void buffer_reset(buffer handle)
{
	handle->data = NULL;
	handle->length = 0;
	handle->capacity = 0;
}

buffer buffer_create(size_t initial_capacity)
{
	buffer new_buffer = malloc_or_die(sizeof(struct buffer_implementation));
	buffer_reset(new_buffer);
	new_buffer->data = malloc_or_die(initial_capacity);
	new_buffer->capacity = initial_capacity;
	return new_buffer;
}

void buffer_free(buffer handle)
{
	free(handle->data);
	free(handle);
}

size_t buffer_length(buffer handle)
{
	return handle->length;
}

buffer_u1* buffer_publish(buffer handle)
{
	buffer_u1* const data = handle->data;
	buffer_reset(handle);
	return data;
}

void buffer_write_u1(buffer handle, buffer_u1 u1)
{
	buffer_extend(handle, 1)[0] = u1;
}

void buffer_write_u2(buffer handle, buffer_u2 u2)
{
	buffer_u1* const bytes = buffer_extend(handle, 2);
	bytes[0] = u2 >> 8;
	bytes[1] = u2;
}

void buffer_write_u4(buffer handle, buffer_u4 u4)
{
	buffer_u1* const bytes = buffer_extend(handle, 4);
	bytes[0] = u4 >> 24;
	bytes[1] = u4 >> 16;
	bytes[2] = u4 >> 8;
	bytes[3] = u4;
}

void buffer_write_array(buffer handle, const buffer_u1* array, size_t length)
{
	memcpy(buffer_extend(handle, length), array, length);
}

void buffer_write_buffer(buffer destination_handle, buffer source_handle)
{
	buffer_write_array(destination_handle, source_handle->data, source_handle->length);
}

bool buffer_write_to_stream(buffer handle, FILE* stream)
{
	return (fwrite(handle->data, 1, handle->length, stream) == handle->length);
}
