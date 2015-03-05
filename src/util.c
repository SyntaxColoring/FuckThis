#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void fatal_error(const char* message)
{
	perror(message);
	abort();
}

void* malloc_or_die(const size_t size)
{
	void* const result = malloc(size);
	if (!result) fatal_error("Allocation failed");
	return result;
}

void* realloc_or_die(void* const pointer, const size_t size)
{
	void* const result = realloc(pointer, size);
	if (!result) fatal_error("Allocation failed");
	return result;
}

char* string_clone(const char* source)
{
	const size_t buffer_length = strlen(source)+1;
	
	/* memcpy appears to be slightly faster than strcpy here. */
	return memcpy(malloc_or_die(buffer_length), source, buffer_length);
}

char* string_create_concatenated(const char* a, const char* b)
{
	const size_t a_length = strlen(a);
	const size_t b_length = strlen(b);
	
	char* const result = malloc_or_die(a_length + b_length + 1);
	memcpy(result, a, a_length);
	memcpy(result + a_length, b, b_length + 1);
	return result;
}
