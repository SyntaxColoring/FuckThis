#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "intwrite.h"
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

size_t write_name_and_type_constant(unsigned int name_index, unsigned int descriptor_index, FILE* stream)
{
	return (write_8(12, stream) &&
	        write_16(name_index, stream) &&
	        write_16(descriptor_index, stream));
}

int main(void)
{
	struct bytecode_section bytecode_section;
	FILE* of = fopen("FuckJava.class", "w");
	
	if (!of)
	{
		perror("Error creating file");
	}
	
	else
	{
		write_32(0xcafebabe, of); /* Magic number. */
		write_16(0, of); /* Minor version. */
		write_16(51, of); /* Major version. */
		
		write_16(11, of); /* Number of entries in constant pool plus one. */
		write_utf8_constant("FuckJava", of);
		write_class_constant(1, of);
		write_utf8_constant("java/lang/Object", of);
		write_class_constant(3, of);
		write_utf8_constant("main", of);
		write_utf8_constant("([Ljava/lang/String;)V", of);
		write_utf8_constant("Code", of);
		write_utf8_constant("println", of);
		write_utf8_constant("(C)V", of);
		write_name_and_type_constant(8, 9, of);
		
		write_16(0x0001 | 0x0010, of); /* Access flags (public final). */
		
		write_16(2, of); /* This class. */
		write_16(4, of); /* Super class. */
		
		write_16(0, of); /* Interfaces count. */
		write_16(0, of); /* Fields count. */
		                    
		write_16(1, of); /* Methods count. */
		write_16(0x0001 | 0x0008, of); /* Method flags (public static). */
		write_16(5, of); /* Method name index. */
		write_16(6, of); /* Method descriptor index. */
		write_16(1, of); /* Method attributes count. */
		
		write_16(7, of); /* Code attribute name index. */
		generate_code(stdin, &bytecode_section);
		write_bytecode_section(&bytecode_section, of);
		
		write_16(0, of); /* Class attributes count. */
	}
}
