#include "codegen.h"

#include <stdlib.h>
#include <string.h>
#include "intwrite.h"
#include "opcodes.h"

static char CODE_PROLOGUE[] =
{
	OP_SIPUSH, SPLIT_16(30000),
	OP_NEWARRAY, ATYPE_CHAR,
	OP_ASTORE_N(0),
	OP_ICONST_N(0)
};

static char CODE_EPILOGUE[] =
{
	OP_RETURN
};
                              
struct buffer
{
	size_t length;
	size_t reserved_length;
	unsigned char* data;
};

void buffer_init(struct buffer* const buffer)
{
	buffer->length = 0;
	buffer->reserved_length = 1;
	buffer->data = malloc(buffer->reserved_length);
}

unsigned char* buffer_write(struct buffer* const buffer, const size_t length)
{
	while (buffer->length+length > buffer->reserved_length)
		buffer->data = realloc(buffer->data, buffer->reserved_length *= 2);
	buffer->length += length;
	return buffer->data + buffer->length - length;
}

#define BUFFER_WRITE_ARRAY(b, a) memcpy(buffer_write(b, sizeof(a)), a, sizeof(a))

void generate_code(FILE* const source, struct bytecode_section* const destination)
{
	int token;
	struct buffer buffer;
	unsigned char* current;
	
	buffer_init(&buffer);
	BUFFER_WRITE_ARRAY(&buffer, CODE_PROLOGUE);
	
	while ((token = getc(source)) != EOF)
	{
		if (token == '>')
		{
			current = buffer_write(&buffer, 2);
			current[0] = OP_ICONST_N(1);
			current[1] = OP_IADD;
		}
		
		else if (token == '<')
		{
			current = buffer_write(&buffer, 2);
			current[0] = OP_ICONST_N(-1);
			current[1] = OP_IADD;
		}
		
		else if (token == '.')
		{
			current = buffer_write(&buffer, 6);
			current[0] = OP_DUP;
			current[1] = OP_GETSTATIC;
			current[2] = 0;
			current[3] = 16;
			current[4] = OP_SWAP;
			current[5] = OP_INVOKEVIRTUAL;
		}
	}
	
	BUFFER_WRITE_ARRAY(&buffer, CODE_EPILOGUE);
	
	destination->max_stack = 123;
	destination->max_locals = 456;
	destination->code_length = buffer.length;
	destination->code = buffer.data;
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
	
	return 1;
}
