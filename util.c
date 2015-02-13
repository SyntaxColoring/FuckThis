#include "util.h"

#include <stdlib.h>
#include <stdio.h>

void* malloc_or_die(const size_t size)
{
	void* const result = malloc(size);
	if (!result)
	{
		perror("Allocation failed");
		abort();
	}
	return result;
}

void* realloc_or_die(void* const pointer, const size_t size)
{
	void* const result = realloc(pointer, size);
	if (!result)
	{
		perror("Reallocation failed");
		abort();
	}
	return result;
}

void bb_reserve(struct byte_buffer* const buffer, const size_t capacity)
{
	buffer->data = realloc_or_die(buffer->data, buffer->capacity = capacity);
	if (buffer->length > capacity) buffer->length = capacity;
}

u1* bb_append(struct byte_buffer* const buffer, const size_t length)
{
	const size_t required_capacity = buffer->length + length;
	u1* sub_array;
	
	/* Allocate extra space if we need to. */
	if (buffer->capacity < required_capacity)
		bb_reserve(buffer, required_capacity*2);
	
	sub_array = buffer->data + buffer->length;
	buffer->length += length;
	return sub_array;
}

void bb_write_u1(struct byte_buffer* const buffer, const u1 data)
{
	bb_append(buffer, 1)[0] = data;
}

void bb_write_u2(struct byte_buffer* const buffer, const u2 data)
{
	u1* const destination = bb_append(buffer, 2);
	destination[0] = data >> 8;
	destination[1] = data;
}

void bb_write_u4(struct byte_buffer* const buffer, const u4 data)
{
	u1* const destination = bb_append(buffer, 4);
	destination[0] = data >> 24;
	destination[1] = data >> 16;
	destination[2] = data >> 8;
	destination[3] = data;
}

bool bb_write_to_stream(const struct byte_buffer* const buffer, FILE* const stream)
{
	return (fwrite(buffer->data, 1, buffer->length, stream) == buffer->length);
}
