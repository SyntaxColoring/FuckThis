#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "intwrite.h"
#include "javaclass.h"
#include "codegen.h"

size_t write_utf8_constant(const char* string, FILE* stream)
{
	size_t length = strlen(string);
	return (write_8(1, stream) &&
	        write_16(length, stream) &&
	        fwrite(string, 1, length, stream));
}

size_t write_class_constant(unsigned int name_index, FILE* stream)
{
	return (write_8(7, stream) && write_16(name_index, stream));
}

size_t write_fieldref_constant(unsigned int class_index, unsigned int descriptor_index, FILE* stream)
{
	return (write_8(9, stream) &&
	        write_16(class_index, stream) &&
	        write_16(descriptor_index, stream));
}

size_t write_name_and_type_constant(unsigned int name_index, unsigned int descriptor_index, FILE* stream)
{
	return (write_8(12, stream) &&
	        write_16(name_index, stream) &&
	        write_16(descriptor_index, stream));
}

int main(void)
{
	struct bytecode_section bytecode_section;
	java_class class_file;
	FILE* of = fopen("FuckJava.class", "w");
	FILE* source = fopen("FuckJava.bf", "r");
	
	if (!of || !source)
	{
		perror("Error creating file");
	}
	
	else
	{
		class_file = java_class_create("FuckJava", "java/lang/Object", JAVA_CLASS_PUBLIC | JAVA_CLASS_FINAL);
		java_class_write(class_file, of);
	}
}
