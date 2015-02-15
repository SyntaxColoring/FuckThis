#include "codegen.h"

#include <stdlib.h>
#include <stdio.h>
#include "java.h"
#include "opcodes.h"

void generate_code(struct java_file* const file, const size_t method_index, FILE* const source)
{
	struct java_method* method = java_get_class(file)->methods + method_index;
	struct byte_buffer buffer = BYTE_BUFFER_EMPTY;	
	int token;
	
	bb_write_u1(&buffer, OP_SIPUSH);
	bb_write_u2(&buffer, 30000),
	bb_write_u1(&buffer, OP_NEWARRAY);
	bb_write_u1(&buffer, ATYPE_CHAR);
	bb_write_u1(&buffer, OP_ASTORE_N(0));
	bb_write_u1(&buffer, OP_ICONST_N(0));
	
	while ((token = getc(source)) != EOF)
	{
		if (token == '>')
		{
			bb_write_u1(&buffer, OP_ICONST_N(1));
			bb_write_u1(&buffer, OP_IADD);
		}
		else if (token == '<')
		{
			bb_write_u1(&buffer, OP_ICONST_N(1));
			bb_write_u1(&buffer, OP_IADD);
		}
		else if (token == '.')
		{
			bb_write_u1(&buffer, OP_DUP);
			bb_write_u1(&buffer, OP_GETSTATIC);
			bb_write_u2(&buffer, java_ref_field(file, "java/lang/System", "out", "Ljava/io/PrintStream;"));
		}
	}
	
	bb_write_u1(&buffer, OP_RETURN);
	
	method->max_stack = 123;
	method->max_locals = 456;
	method->bytecode_length = buffer.length;
	method->bytecode = buffer.data;
}
