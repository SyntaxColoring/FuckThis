#include "codegen.h"

#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"
#include "java.h"
#include "opcodes.h"

/* Returns whether or not the given character is a Brainfuck token. */
static bool is_token(int character)
{
	return (character == '<' || character == '>' ||
	        character == '-' || character == '+' ||
	        character == '.' || character == ',' ||
	        character == '[' || character == ']');
}

/* Consumes all characters up to and including the next Brainfuck token and
   returns the token, or EOF if there are no more tokens. */
static int get_token(FILE* source)
{
	int character;
	while ((character = getc(source)) != EOF)
		if (is_token(character)) break;
	return character;
}

/* Returns the next thing that get_token() would return without consuming it. */
static int peek_token(FILE* source)
{
	return ungetc(get_token(source), source);
}

/* Consumes tokens from source until the next character is neither incrementer
   nor decrementer.  Returns the number of incrementer occurrences minus the
   number of decrementer occurrences. */
static int collapse(FILE* source, char decrementer, char incrementer)
{
	int net_change = 0;
	while (peek_token(source) == decrementer || peek_token(source) == incrementer)
	{
		if (get_token(source) == decrementer) net_change--;
		else net_change++;
	}
	return net_change;
}

/* Writes bytecode to destination that will act to push value to the stack.  For
   small values, the bytecode will be optimized to a single instruction. */
static void write_integer_push(buffer destination, int value)
{
	if (value >= OP_ICONST_N_MIN && value <= OP_ICONST_N_MAX)
	{
		buffer_write_u1(destination, OP_ICONST_N(value));
	}
	
	else
	{
		buffer_write_u1(destination, OP_SIPUSH);
		buffer_write_u2(destination, value);
	}
}

static void write_method_prologue(buffer destination)
{
	write_integer_push(destination, 30000);
	buffer_write_u1(destination, OP_NEWARRAY);
	buffer_write_u1(destination, ATYPE_CHAR);
	write_integer_push(destination, 0);
}

static void write_method_epilogue(buffer destination)
{
	buffer_write_u1(destination, OP_RETURN);
}

static void write_index_change(buffer destination, int change)
{
	if (change)
	{
		write_integer_push(destination, change);
		buffer_write_u1(destination, OP_IADD);
	}
}

static void write_value_change(buffer destination, int change)
{
	if (change)
	{
		buffer_write_u1(destination, OP_DUP2);
		buffer_write_u1(destination, OP_DUP2);
		buffer_write_u1(destination, OP_CALOAD);
		write_integer_push(destination, change);
		buffer_write_u1(destination, OP_IADD);
		buffer_write_u1(destination, OP_CASTORE);
	}
}

static void write_output(buffer destination, java_file file)
{
	buffer_write_u1(destination, OP_DUP2);
	buffer_write_u1(destination, OP_CALOAD);
	buffer_write_u1(destination, OP_GETSTATIC);
	buffer_write_u2(destination, java_ref_field(file, "java/lang/System", "out", "Ljava/io/PrintStream;"));
	buffer_write_u1(destination, OP_SWAP);
	buffer_write_u1(destination, OP_INVOKEVIRTUAL);
	buffer_write_u2(destination, java_ref_method(file, "java/io/PrintStream", "print", "(C)V"));
}

static void write_input(buffer destination, java_file file)
{
	buffer_write_u1(destination, OP_DUP2);
	buffer_write_u1(destination, OP_GETSTATIC);
	buffer_write_u2(destination, java_ref_field(file, "java/lang/System", "in", "Ljava/io/InputStream;"));
	buffer_write_u1(destination, OP_INVOKEVIRTUAL);
	buffer_write_u2(destination, java_ref_method(file, "java/io/InputStream", "read", "()I"));
	buffer_write_u1(destination, OP_CASTORE);
}

static void write_loop_prologue(buffer destination, size_t loop_body_length)
{
	buffer_write_u1(destination, OP_DUP2);
	buffer_write_u1(destination, OP_CALOAD);
	
	buffer_write_u1(destination, OP_IFEQ);
	buffer_write_u2(destination, loop_body_length + 6);
}

static void write_loop_epilogue(buffer destination, size_t loop_body_length)
{
	buffer_write_u1(destination, OP_GOTO);
	buffer_write_u2(destination, -loop_body_length - 5);
}

static void write_loop(buffer destination, FILE* source, java_file file);

/* Consumes and generates bytecode for all characters up to, but not including,
   the next ']' or EOF. */
static void write_section(buffer destination, FILE* source, java_file file)
{
	while (peek_token(source) != ']' && peek_token(source) != EOF)
	{	
		write_index_change(destination, collapse(source, '<', '>'));
		write_value_change(destination, collapse(source, '-', '+'));
		
		switch (peek_token(source))
		{
			case '.':
				get_token(source);
				write_output(destination, file);
				break;
			case ',':
				get_token(source);
				write_input(destination, file);
				break;
			case '[':
				get_token(source);
				write_loop(destination, source, file);
				break;
		}
	}
}

/* Consumes and generates bytecode for all characters up to and including the
   next ']' or EOF.  This function is similar to write_section(), except that
   this function assumes the next token is the beginning of a loop body. */
static void write_loop(buffer destination, FILE* source, java_file file)
{
	bool success;
	buffer loop_body = buffer_create(64);
	write_section(loop_body, source, file);
	
	success = (get_token(source) == ']'); /* Otherwise it's EOF. */
	
	if (success)
	{
		write_loop_prologue(destination, buffer_length(loop_body));
		buffer_write_buffer(destination, loop_body);
		write_loop_epilogue(destination, buffer_length(loop_body));
	}
	
	buffer_free(loop_body);
	
	if (!success)
	{
		printf("Error: Unmatched \"[\".\n");
		abort();
	}	
}

void generate_code(java_file file, size_t method_index, FILE* source)
{
	struct java_method* method = java_get_class(file)->methods + method_index;
	buffer bytecode_buffer = buffer_create(1024);
	bool success;
	
	write_method_prologue(bytecode_buffer);
	write_section(bytecode_buffer, source, file);
	write_method_epilogue(bytecode_buffer);
	
	success = (peek_token(source) == EOF); /* Otherwise it's an unmatched ']'. */
	
	if (success)
	{
		method->max_stack = 123;
		method->max_locals = 456;
		method->bytecode_length = buffer_length(bytecode_buffer);
		method->bytecode = buffer_publish(bytecode_buffer);
	}
	
	buffer_free(bytecode_buffer);
	
	if (!success)
	{
		printf("Error: Unmatched \"]\".\n");
		abort();
	}
}
