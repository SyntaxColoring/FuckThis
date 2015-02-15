#ifndef CODEGEN_H_INCLUDED
#define CODEGEN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
                              
struct bytecode_section
{
	unsigned int max_stack;
	unsigned int max_locals;
	unsigned long code_length;
	unsigned char* code;
};

void generate_code(FILE* source, struct bytecode_section* destination);

size_t write_bytecode_section(const struct bytecode_section*, FILE* stream);

#endif
