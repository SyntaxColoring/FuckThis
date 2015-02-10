#include "codegen.h"

#include <stdlib.h>
#include "intwrite.h"

void generate_code(FILE* const source, struct bytecode_section* const destination)
{
	int token;
	size_t reserved_length = 1; /* To do: Come up with a reasonable value. */
	
	destination->max_stack = 123;
	destination->max_locals = 456;
	destination->code_length = 0;
	destination->code = malloc(reserved_length); /* To do: Make sure this isn't NULL. */
	
	while ((token = getc(source)) != EOF)
	{
		destination->code_length++;	
		while (reserved_length < destination->code_length)
		{
			/* To do: More efficient implementation. */
			destination->code = realloc(destination->code, reserved_length *= 2);
		}
		
		destination->code[destination->code_length - 1] = 0;
	}
}

size_t write_bytecode_section(const struct bytecode_section* const source, FILE* const stream)
{
	write_32(12 + source->code_length, stream); /* Total length (not including first 6 bytes. */
	write_16(source->max_stack, stream);
	write_16(source->max_locals, stream);
	write_32(source->code_length, stream);
	fwrite(source->code, 1, source->code_length, stream); /* To do: Return value checking. */
	write_16(0, stream); /* Exception table length. */
	write_16(0, stream); /* Sub-attributes count. */
}
