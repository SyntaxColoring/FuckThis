#include "codegen.h"

#include <stdlib.h>
#include <stdio.h>
#include "java.h"
#include "opcodes.h"

void generate_code(java_file file, const size_t method_index, FILE* const source)
{
	struct java_method* method = java_get_class(file)->methods + method_index;
	struct byte_buffer buffer = BYTE_BUFFER_EMPTY;
	size_t last_open_bracket = 0;
	int token;
	
	bb_write_u1(&buffer, OP_SIPUSH);
	bb_write_u2(&buffer, 30000),
	bb_write_u1(&buffer, OP_NEWARRAY);
	bb_write_u1(&buffer, ATYPE_CHAR);
	
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
			bb_write_u1(&buffer, OP_ICONST_N(-1));
			bb_write_u1(&buffer, OP_IADD);
		}
		else if (token == '+')
		{
			bb_write_u1(&buffer, OP_DUP2);
			bb_write_u1(&buffer, OP_DUP2);
			bb_write_u1(&buffer, OP_CALOAD);
			bb_write_u1(&buffer, OP_ICONST_N(1));
			bb_write_u1(&buffer, OP_IADD);
			bb_write_u1(&buffer, OP_CASTORE);
		}
		else if (token == '-')
		{
			bb_write_u1(&buffer, OP_DUP2);
			bb_write_u1(&buffer, OP_DUP2);
			bb_write_u1(&buffer, OP_CALOAD);
			bb_write_u1(&buffer, OP_ICONST_N(-1));
			bb_write_u1(&buffer, OP_IADD);
			bb_write_u1(&buffer, OP_CASTORE);
		}
		else if (token == '.')
		{
			bb_write_u1(&buffer, OP_DUP2);
			bb_write_u1(&buffer, OP_CALOAD);
			bb_write_u1(&buffer, OP_GETSTATIC);
			bb_write_u2(&buffer, java_ref_field(file, "java/lang/System", "out", "Ljava/io/PrintStream;"));
			bb_write_u1(&buffer, OP_SWAP);
			bb_write_u1(&buffer, OP_INVOKEVIRTUAL);
			bb_write_u2(&buffer, java_ref_method(file, "java/io/PrintStream", "print", "(C)V"));
		}
		else if (token == '[')
		{
			bb_write_u1(&buffer, OP_DUP2);
			bb_write_u1(&buffer, OP_CALOAD);
			
			bb_write_u1(&buffer, OP_IFEQ);
			bb_write_u2(&buffer, last_open_bracket);
			last_open_bracket = buffer.length - 2;
		}
		else if (token == ']')
		{
			size_t open_bracket = last_open_bracket;
			size_t offset_to_open_bracket = last_open_bracket-buffer.length-1;
			bb_write_u1(&buffer, OP_GOTO);
			bb_write_u2(&buffer, offset_to_open_bracket-2);
			
			last_open_bracket = buffer.data[open_bracket] << 8
			                  | buffer.data[open_bracket+1];
			
			buffer.data[open_bracket] = (buffer.length-open_bracket+1) << 8;
			buffer.data[open_bracket+1] = buffer.length-open_bracket+1; 
		}
	}
	
	bb_write_u1(&buffer, OP_RETURN);
	
	method->max_stack = 123;
	method->max_locals = 456;
	method->bytecode_length = buffer.length;
	method->bytecode = buffer.data;
}
